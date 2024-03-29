#include "bupt_can/bupt_can.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <linux/can/raw.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include <iostream>

Can::Can(const std::string &can_name)
{
    this->can_name = can_name;
    can_fd_read = socket(AF_CAN,SOCK_RAW,CAN_RAW);
    can_fd_write = socket(AF_CAN,SOCK_RAW,CAN_RAW);

    std::strcpy(ifr.ifr_name,can_name.c_str());

    ioctl(can_fd_read,SIOCGIFINDEX,&ifr);
    ioctl(can_fd_write,SIOCGIFINDEX,&ifr);

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    bind(can_fd_read,(struct sockaddr*)&addr,sizeof(addr));
    bind(can_fd_write,(struct sockaddr*)&addr,sizeof(addr));

    isDestroyed = false;
    Started = false;
    filter_size = 0;

    filters[filter_size].can_id = 0x7FC;
    filters[filter_size].can_mask = CAN_SFF_MASK;
    filter_size++;
}

Can::~Can()
{
    send_can(0x7FC,CAN_ID_STD,0,{0,0,0,0,0,0,0,0}); // 结束帧，用于结束接收线程
    send_thread_->join();
    recv_thread_->join();
}

void Can::can_start()
{
    if (Started) return;
    set_recv_filter();
    recv_thread_ = std::make_unique<std::thread>(std::bind(&Can::receive_thread,this));
    send_thread_ = std::make_unique<std::thread>(std::bind(&Can::send_thread,this));
    Started = true;
}

uint32_t Can::set_id_type(const CAN_ID_TYPE &id_type, const uint32_t &id)
{
    uint32_t ret = id;
    if (id_type == CAN_ID_TYPE::CAN_ID_EXT)
    {
        ret |= CAN_EFF_FLAG;
    }
    else if (id_type == CAN_ID_TYPE::CAN_ID_RTR)
    {
        ret |= CAN_RTR_FLAG;
    }
    else if (id_type == CAN_ID_TYPE::CAN_ID_ERR)
    {
        ret |= CAN_ERR_FLAG;
    }
    return ret;
}

void Can::set_callback_map(const uint32_t &id, const std::function<void(const std::shared_ptr<can_frame>&)> callback)
{
    std::lock_guard<std::mutex> lock(recv_callback_map_mutex);
    recv_callback_map[id] = callback;
}

void Can::register_msg(const uint32_t &id, const CAN_ID_TYPE &id_type, const std::function<void(const std::shared_ptr<can_frame>&)> callback)
{
    set_callback_map(id, callback);
    filters[filter_size].can_id = id ;
    filters[filter_size].can_id = set_id_type(id_type,filters[filter_size].can_id);
    if (id_type == CAN_ID_STD)
        filters[filter_size].can_mask = CAN_SFF_MASK;
    else if (id_type == CAN_ID_EXT)
        filters[filter_size].can_mask = CAN_EFF_MASK,printf("1\n");
    else if (id_type == CAN_ID_ERR)
        filters[filter_size].can_mask = CAN_ERR_MASK;
    filter_size++;
}

void Can::send_can(const uint32_t &id, const CAN_ID_TYPE &id_type, const int &dlc,const std::array<uint8_t,8> &data)
{
    struct can_frame frame;
    
    frame.can_id = set_id_type(id_type,id);
    frame.can_dlc = dlc;
    for (int i = 0;i < dlc;i++)
    {
        frame.data[i] = data[i];
    }
    send_can(frame);
}

void Can::send_can(const can_frame &frame)
{
    std::lock_guard<std::mutex> lock(send_que_mutex);
    send_que.push(frame);
    if (send_que.size()==1)
    {
        send_que_cv.notify_one();
    }
}

void Can::receive_thread()
{
    while (true)
    {
        auto frame_ptr = std::make_shared<can_frame>();
        int nbytes = read(can_fd_read,frame_ptr.get(),sizeof(struct can_frame));
        if (nbytes > 0)
        {
            if (frame_ptr->can_id & CAN_ERR_FLAG)
            {
                frame_ptr->can_id &= CAN_ERR_MASK;
            }
            else if (frame_ptr->can_id & CAN_EFF_FLAG)
            {
                frame_ptr->can_id &= CAN_EFF_MASK;
            }
            else if (frame_ptr->can_id & CAN_RTR_FLAG)
            {
                frame_ptr->can_id &= CAN_SFF_MASK;
            }
            else
            {
                frame_ptr->can_id &= CAN_SFF_MASK;
            }
            std::lock_guard<std::mutex> lock(recv_callback_map_mutex);
            if (recv_callback_map.find(frame_ptr->can_id) != recv_callback_map.end())
            {
                recv_callback_map[frame_ptr->can_id](frame_ptr);
            }
            if (frame_ptr->can_id == 0x7FC) break;
        }
    }
}

void Can::send_thread()
{
    while (true)
    {
        std::unique_lock<std::mutex> lock1(send_que_mutex);
        send_que_cv.wait(lock1, [this](){return !send_que.empty();});
        if (send_que.empty()) break;
        struct can_frame frame = send_que.front();
        send_que.pop();
        lock1.unlock();
        std::lock_guard<std::mutex> lock2(can_fd_write_mutex);
        ssize_t ret = write(can_fd_write, &frame, sizeof(frame));
        if (ret == -1) {
            std::cerr << "Error writing to CAN bus" << std::endl;
        }
        if (frame.can_id == 0x7FC) break;
    }
}

bool Can::send_can_with_respond(const uint32_t &id, const CAN_ID_TYPE &id_type, const int &dlc, const std::array<uint8_t, 8> &data)
{
    struct can_frame frame;
    frame.can_id = id;
    frame.can_dlc = dlc;
    for (int i = 0;i < dlc;i++)
    {
        frame.data[i] = data[i];
    }
    return send_can_with_respond(frame);
}

bool Can::send_can_with_respond(const can_frame &frame)
{
    std::lock_guard<std::mutex> lock(can_fd_write_mutex);
    int ret=write(can_fd_write,&frame,sizeof(frame));
    return ret!=-1;
}

int Can::set_recv_filter()
{
    setsockopt(can_fd_read,SOL_CAN_RAW,CAN_RAW_FILTER,
        filters.data(),sizeof(struct can_filter)*filter_size);
    return 0;
}

bool Can::isStarted() const
{
    return Started;
}


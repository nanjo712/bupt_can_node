#include "bupt_can.h"
#include<sys/types.h>
#include<sys/socket.h>
#include<net/if.h>
#include<sys/ioctl.h>
#include<linux/can/raw.h>
#include<unistd.h>
#include <string>
#include <cstring>

Can::Can(const std::string &can_name)
{
    this->can_name = can_name;
    can_fd = socket(AF_CAN,SOCK_RAW,CAN_RAW);
    std::strcpy(ifr.ifr_name,can_name.c_str());
    ioctl(can_fd,SIOCGIFINDEX,&ifr);
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    bind(can_fd,(struct sockaddr*)&addr,sizeof(addr));
}

Can::Can(const Can& other)
{
    this->can_name = other.can_name;
    can_fd = socket(AF_CAN,SOCK_RAW,CAN_RAW);
    std::strcpy(ifr.ifr_name,can_name.c_str());
    ioctl(can_fd,SIOCGIFINDEX,&ifr);
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    bind(can_fd,(struct sockaddr*)&addr,sizeof(addr));
}

Can::~Can()
{
    close_can();
}

int Can::set_recv_filter(const int &id)
{
    struct can_filter filter;
    filter.can_id = id;
    filter.can_mask = CAN_SFF_MASK;
    setsockopt(can_fd,SOL_CAN_RAW,CAN_RAW_FILTER,&filter,sizeof(filter));
    return 0;
}

can_frame Can::rece_can()
{
    struct can_frame frame;
    int recv_result = read(can_fd,&frame,sizeof(frame));
    return frame;
}

int Can::send_can(const int &id,const int &dlc,const char* data)
{
    struct can_frame frame;
    frame.can_id = id;
    frame.can_dlc = dlc;
    for (int i = 0;i < dlc;i++)
    {
        frame.data[i] = data[i];
    }
    write(can_fd,&frame,sizeof(frame));
    return 0;
}

int Can::send_can(const can_frame &frame)
{
    write(can_fd,&frame,sizeof(frame));    
    return 0;
}

int Can::close_can()
{
    close(can_fd);
    return 0;
}

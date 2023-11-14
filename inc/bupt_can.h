#ifndef BUPT_CAN_H
#define BUPT_CAN_H

#include <string>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <cstdint>
#include <array>
#include <mutex>
#include <queue>
#include <functional>
#include <condition_variable>
#include <memory>
#include <map>
#include <thread>

class Can
{
private:
    enum CAN_ID_TYPE{
        CAN_ID_STD,
        CAN_ID_EXT
    };

    std::string can_name;
    struct ifreq ifr;
    struct sockaddr_can addr;

    int can_fd_read; // read can socket
    int can_fd_write; // write can socket

    std::queue<can_frame> send_que;
    std::mutex send_que_mutex; 

    std::map< uint32_t, std::function<void(std::shared_ptr<can_frame>)> > recv_callback_map;
    std::mutex recv_callback_map_mutex;

    std::array<struct can_filter,512> filters;
    int filter_size;

    std::unique_ptr<std::thread> send_thread_;
    std::unique_ptr<std::thread> recv_thread_;

    bool isDestroyed;

    void receive_thread();
    void send_thread();
public:
    explicit Can(const std::string &can_name); // constructor
    ~Can(); // destructor

    void can_start();

    bool register_msg(const uint32_t id,const std::function<void(std::shared_ptr<can_frame>)> callback);

    void send_can(const int &id, const int &dlc, const std::array<uint8_t,8> &data);
    void send_can(const can_frame &frame);

    int set_recv_filter();
};

#endif // BUPT_CAN_H
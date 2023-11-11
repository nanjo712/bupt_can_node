#ifndef BUPT_CAN_H
#define BUPT_CAN_H

#include <string>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>

enum CAN_ID_TYPE{
    CAN_ID_STD = 0,
    CAN_ID_EXT = 1
};

class Can
{
private:
    std::string can_name;
    int can_fd;
    struct ifreq ifr;
    struct sockaddr_can addr;

public:
    Can(const std::string &can_name); // constructor
    Can(const Can &other); // copy constructor
    ~Can();
    int set_recv_filter(const int &id);
    can_frame rece_can();
    int send_can(const int &id, const int &dlc, const char *data);
    int close_can();
};

#endif // BUPT_CAN_H
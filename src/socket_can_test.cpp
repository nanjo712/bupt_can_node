#include "bupt_can.h"
#include <unistd.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <functional>

void print_the_msg(const std::shared_ptr<can_frame> frame)
{
    std::cout << "id: " << frame->can_id << std::endl;
    std::cout << "dlc: " << frame->can_dlc << std::endl;
    std::cout << "data: ";
    for (int i = 0;i < frame->can_dlc;i++)
    {
        std::cout << std::hex << (int)frame->data[i] << " ";
    }
    std::cout << std::endl;
}

int main()
{
    Can can("can0");
    can.register_msg(0x205, std::bind(print_the_msg, std::placeholders::_1));
    can.set_recv_filter();
    printf("set recv filter\n");
    can.can_start();
    printf("can start\n");

    std::thread send_thread([&can](){
        while (true)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            std::array<uint8_t,8> data = {0x1F,0xFF,0x00,0x00,0x00,0x00,0x00};
            can.send_can(0x1FF,8,data);
        }
    });
    
    send_thread.detach();
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    return 0;
}


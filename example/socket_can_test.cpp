#include "bupt_can.h"
#include <unistd.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <functional>

void print_the_msg(const std::shared_ptr<can_frame> &frame)
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
    Can can("vcan0");
    can.register_msg(0x205, Can::CAN_ID_STD, std::bind(print_the_msg, std::placeholders::_1));
    can.register_msg(0x12345678, Can::CAN_ID_EXT, std::bind(print_the_msg, std::placeholders::_1));
    // 注册CAN消息回调函数
    // 这个函数会把对应的CAN消息的ID和回调函数绑定起来
    // 并且会把对应的ID添加到过滤器中
    
    can.set_recv_filter();
    // 设置过滤器
    // 每当你注册了新消息，你都应该调用这个函数以更新过滤器
    // 否则过滤器不会生效
    // 一个推荐的做法是在注册完所有消息后调用一次这个函数

    can.can_start();
    // 启动CAN收发线程，这个函数会立即返回
    // 在启动收发线程之后你不应该尝试修改过滤器
    // 这一行为未经测试，可能会导致未知的错误

    std::thread send_thread([&can](){
        while (true)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            std::array<uint8_t,8> data = {0x1F,0xFF,0x00,0x00,0x00,0x00,0x00};
            can.send_can(0x12345678,Can::CAN_ID_EXT,8,data);
            can.send_can(0x205,Can::CAN_ID_STD,8,data);
            // 请注意，不要无延时全速发消息
            // 这样会导致CAN总线拥塞
            // 这样的场景很罕见，但是还是要注意
            // 用专业CAN卡测试，发送频率在1khz时已经几乎达到极限
        }
    });
    send_thread.detach();
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    return 0;
}


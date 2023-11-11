#include "bupt_can.h"
#include <unistd.h>
#include <iostream>
#include <chrono>
#include <thread>

int main()
{
    Can can("can0");
    can.set_recv_filter(0x205);
    can_frame frame;
    while (1)
    {
        frame = can.rece_can();
        std::cout << "id: 0x" << std::hex << frame.can_id << std::endl;
        std::cout << "dlc: " << std::dec << (int)frame.can_dlc << std::endl;
        std::cout << "data: ";
        for (int i = 0;i < frame.can_dlc;i++)
        {
            std::cout << std::hex << (int)frame.data[i] << " ";
        }
        std::cout << std::endl;
    }
    return 0;
}


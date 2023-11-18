# BUPT_CAN_Node

## 1. 项目简介

一个CAN收发管理节点，基于SocketCAN开发

## 2. 项目结构

```
- BUPT_CAN_Node
    - inc
        - bupt_can.h # CAN收发管理节点头文件 
    - src
        - bupt_can.cpp # CAN收发管理节点源文件  
    - example
        - socket_can_test.cpp # SocketCAN测试程序
    - CMakeLists.txt # CMake配置文件
    - setup_can.sh # SocketCAN配置脚本
    - setup_vcan.sh # 虚拟CAN配置脚本
    - LICENSE # 项目许可证
    - README.md # 项目说明文件
```

## 3.构建方法

本工程依赖pthread和SocketCAN库，你需要安装这些库才能构建本工程。
使用C++17以上的标准进行编译。

```
mkdir build
cd build
cmake ..
make
```

这将会构建出一个名为bupt_can_node的可执行文件和名为libbupt_can.a的静态库。
你可以使用静态库来构建你自己的程序。

## 4.运行例程

请在正确设置can设备后运行程序，否则程序将无法正常运行。

如果你不知道如何设置can设备，请参考[SocketCAN](https://www.kernel.org/doc/Documentation/networking/can.txt)的官方文档。

对于一般的CAN卡，你可以尝试使用setup_can.sh脚本来设置can设备或者setup_vcan.sh来获得一个虚拟can口。

脚本将会将can0设备设置为1000kbps的速率，启用回环模式。

注意，你需要安装can-utils和net-tools来使用脚本。

```
cd build
./bupt_can
```

程序将以1ms的间隔发送ID为0x1FF的消息，SocketCAN的回环模式被默认启用，故程序将会收到自己发送的消息。屏幕上将不断刷新出收发的消息。

## 5.Can类的使用方法

**需要注意的是，0x7FC是本程序的保留ID，用于结束接收线程，如无必要请勿占用该ID。**

对象接口如下：

```cpp
enum CAN_ID_TYPE{
    CAN_ID_STD,
    CAN_ID_EXT,
    CAN_ID_ERR,
    CAN_ID_RTR
};

explicit Can(const std::string &can_name); // constructor
~Can(); // destructor
/**
 * @brief Set the can id type
 * @param id_type The can id type
 * @param id The can id
 * @return The can id with type
*/
uint32_t set_id_type(const CAN_ID_TYPE &id_type, const uint32_t &id);
/**
 * @brief It will star to receive and send can frame
 * @param none
*/
void can_start();
/**
 * @brief Register a callback function to a can frame id
 * @param id The can frame id
 * @param callback The callback function
*/
void register_msg(const uint32_t &id, const CAN_ID_TYPE &id_type, const std::function<void(const std::shared_ptr<can_frame>&)> callback);
/**
 * @brief Send a can frame
 * @param id The can frame id
 * @param dlc The can frame dlc
 * @param data The can frame data
*/
void send_can(const uint32_t &id, const CAN_ID_TYPE &id_type, const int &dlc, const std::array<uint8_t,8> &data);
/**
 * @brief Send a can frame
 * @param frame The can frame
*/
void send_can(const can_frame &frame);
/**
 * @brief Send a can frame and wait for respond
 * @param id The can frame id
 * @param dlc The can frame dlc
 * @return true if success, false if failed
*/
bool send_can_with_respond(const uint32_t &id, const CAN_ID_TYPE &id_type, const int &dlc, const std::array<uint8_t,8> &data);
/**
 * @brief Send a can frame and wait for respond
 * @param frame The can frame
 * @return true if success, false if failed
*/
bool send_can_with_respond(const can_frame &frame);
/**
 * @brief Set the recv filter
 * @param none
 * @return 0 if success, -1 if failed
*/
int set_recv_filter();
```

你可以参考例程获得更多信息。

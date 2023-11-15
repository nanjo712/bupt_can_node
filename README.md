# BUPT_CAN_Node

## 1. 项目简介

一个CAN收发管理节点，基于SocketCAN开发

## 2. 项目结构

```
- BUPT_CAN_Node
    - config
        - bupt_can.yaml # CAN收发管理节点配置文件
    - inc
        - bupt_can.h # CAN收发管理节点头文件
    - src
        - bupt_can.cpp # CAN收发管理节点源文件
        - socket_can_test.cpp # SocketCAN测试文件
    - CMakeLists.txt # CMake配置文件
    - setup_can.sh # SocketCAN配置脚本
    - LICENSE # 项目许可证
    - README.md # 项目说明文件
```

## 3.构建方法

```
mkdir build
cd build
cmake ..
make
```

## 4.运行方法

```
cd build
./can_node
```


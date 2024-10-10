#pragma once
#include <iostream>
#include <string>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

class T_Socket
{
protected:
    std::string ip;
    int port;
public:
    T_Socket(const std::string& ip, int port) :ip(ip), port(port) {};

    virtual void U3Dconnect() {
       // std::cout << "Connecting to " << ip << ":" << port << std::endl;
        // 连接到指定的IP和端口
        // 在派生类中实现具体的连接逻辑
    }

    virtual void sendData(const std::string& data) {
        std::cout << "Sending data: " << data << std::endl;
        // 发送数据到连接的IP和端口
        // 在派生类中实现具体的发送逻辑
    }

    virtual void receiveData() {
        // 接收来自连接的IP和端口的数据
        // 在派生类中实现具体的接收逻辑
    }

};


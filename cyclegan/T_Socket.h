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
        // ���ӵ�ָ����IP�Ͷ˿�
        // ����������ʵ�־���������߼�
    }

    virtual void sendData(const std::string& data) {
        std::cout << "Sending data: " << data << std::endl;
        // �������ݵ����ӵ�IP�Ͷ˿�
        // ����������ʵ�־���ķ����߼�
    }

    virtual void receiveData() {
        // �����������ӵ�IP�Ͷ˿ڵ�����
        // ����������ʵ�־���Ľ����߼�
    }

};


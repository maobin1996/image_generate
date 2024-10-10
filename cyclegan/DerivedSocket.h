#pragma once
#include "T_Socket.h"
#include <opencv2/opencv.hpp>
#include <mutex>

class DerivedSocket : public T_Socket {
public:
    DerivedSocket(const std::string& ip, int port) : T_Socket(ip, port) {}

    void U3Dconnect() override;

    void sendData(const std::string& data) override;

    void receiveData() override;

   // cv::Mat getData();
public:
   // std::string image_msg;
    cv::Mat m_image_mat;
    std::vector<char> buffer;
    std::mutex mtx;
    SOCKET sock;
    SOCKET clientSock;
   // sockaddr_in clientSockAddr;
private:
    ~DerivedSocket();
};

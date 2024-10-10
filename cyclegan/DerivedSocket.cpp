#include "DerivedSocket.h"
#include <iostream>
#include <string>
#include <vector>
#include <winsock2.h>
#include <ctime>
#include <opencv2/opencv.hpp>
#include <chrono>
#pragma comment(lib, "ws2_32.lib")
// 初始化Winsock  server

//cv::Mat image_mat(940, 2544, CV_8UC3, cv::Scalar(0));

void DerivedSocket::U3Dconnect()
{
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed with error: " << result << std::endl;
    }

    this->sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Failed to create socket." << std::endl;
        WSACleanup();
    }

    //std::cout << ip << port<<std::endl;
    // 连接到服务器
    sockaddr_in serverSockAddr;
    serverSockAddr.sin_family = AF_INET;
    serverSockAddr.sin_addr.s_addr = INADDR_ANY;
    serverSockAddr.sin_port = htons(port);

    //if (connect(sock, (sockaddr*)&serverSockAddr, sizeof(serverSockAddr)) == -1) {
    //    std::cerr << "Failed to connect to server." << std::endl;
    //}

    // 监听连接请求
    if (bind(sock, (sockaddr*)&serverSockAddr, sizeof(serverSockAddr)) == SOCKET_ERROR) {
        std::cerr << "Failed to bind socket." << std::endl;
        closesocket(sock);
        WSACleanup();
    }

    // 监听连接请求
    if (listen(sock, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Failed to listen on socket." << std::endl;
        closesocket(sock);
        WSACleanup();
    }
}

void DerivedSocket::sendData(const std::string& data)
{
    while (true)
    {
        // 接受连接请求
        sockaddr_in clientSockAddr;
        int clientSockAddrSize = sizeof(clientSockAddr);
        SOCKET clientSock = accept(this->sock, (sockaddr*)&clientSockAddr, &clientSockAddrSize);
        if (clientSock == INVALID_SOCKET) {
            closesocket(this->sock);
            WSACleanup();
        }

        // 循环接收和发送数据
        //char buffer[4096];
        while (true)
        {
            int bytesReceived = send(clientSock, data.c_str(), data.size(), 0);
            if (bytesReceived == SOCKET_ERROR) {
                std::cerr << "Failed to send data." << std::endl;
                closesocket(clientSock);
                closesocket(sock);
                WSACleanup();
            }
        }
        // 关闭客户端套接字
        closesocket(clientSock);
    }
}

void DerivedSocket::receiveData()
{
    // 接收数据
    while (true)
    {
        // 接受连接请求
        sockaddr_in clientSockAddr;
        int clientSockAddrSize = sizeof(clientSockAddr);
        SOCKET clientSock = accept(this->sock, (sockaddr*)&clientSockAddr, &clientSockAddrSize);
        if (clientSock == INVALID_SOCKET) {
            std::cerr << "Failed to accept client connection." << std::endl;
            closesocket(this->sock);
            WSACleanup();
        }

        // 循环接收和发送数据
        //char buffer[1024] = {0};
        std::vector<char> buffer(1024 * 512 * 3);
       // std::deque<char> fifo;
        // 接收图像数据
        //std::vector<char> img_data;
        // 重置图像数据接收状态
        
        while (true)
        {
            int bytes = recv(clientSock, buffer.data(), buffer.size(), 0);

            //std::cout << "bytes = " << bytes << std::endl;
            if (bytes == SOCKET_ERROR) {
                std::cerr << "Failed to receive data." << std::endl;
                closesocket(clientSock);
                closesocket(this->sock);
                WSACleanup();
                break;
            }
            cv::Mat frame = cv::imdecode(buffer, cv::IMREAD_COLOR);
            std::cout << frame.rows << frame.rows << std::endl;

            std::lock_guard<std::mutex> lock(mtx);
            this->m_image_mat = frame;
            // std::lock_guard<std::mutex> lock(DerivedSocket_image_recvive->mtx);
             cv::imshow("roi2", frame);
             cv::waitKey(1);
            // 以时间来命名保存图像
          //  toimage(image_mat, frame);

        }


        // 关闭客户端套接字
        closesocket(clientSock);
    }
    //std::cout << "ssss" <<this->image_msg<<std::endl;
};



DerivedSocket::~DerivedSocket() {
    // 关闭套接字
    closesocket(sock);
    WSACleanup();
}
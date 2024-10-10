#include "NvInfer.h"
#include "cuda_runtime_api.h"
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>
#include <chrono>
#include "NvInferPlugin.h"
#include "opencv2/opencv.hpp"
#include <time.h> 
#include <opencv2/opencv.hpp>
#include <string.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include "DerivedSocket.h"
#include <thread>
#include "Coordinate_transformation.h"
#include <sstream>
#include <winsock2.h>
#include <WS2tcpip.h>
//#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")
#pragma comment(lib, "ws2_32.lib")
using namespace std::chrono;
using namespace std;
using namespace cv;
#define interval 1  //修改这里以改变多久保存一帧
#define MAX_BUFFER_SIZE 65000
// 定义Engine文件路径
const std::string enginePath = "c.trt";
//const std::string enginePath = "best.trt";
int ribbon_C[256][3] =
{
    { 0 , 1 , 2 },{ 0 , 2 , 4 },{ 0 , 4 , 6 },{ 0 , 5 , 8 },{ 0 , 6 , 10 },{ 0 , 7 , 12 },{ 0 , 9 , 14 },{ 0 , 10 , 15 },{ 0 , 11 , 17 },{ 0 , 12 , 19 },
    { 0 , 14 , 21 },{ 0 , 15 , 23 },{ 0 , 16 , 25 },{ 0 , 17 , 27 },{ 0 , 18 , 29 },{ 0 , 20 , 31 },{ 0 , 21 , 33 },{ 0 , 22 , 35 },{ 0 , 23 , 37 },
    { 0 , 25 , 39 },{ 0 , 26 , 41 },{ 0 , 27 , 43 },{ 0 , 28 , 45 },{ 0 , 29 , 46 },{ 0 , 31 , 48 },{ 0 , 32 , 50 },{ 0 , 33 , 52 },{ 0 , 34 , 54 },
    { 0 , 36 , 56 },{ 0 , 37 , 58 },{ 0 , 38 , 60 },{ 0 , 39 , 62 },{ 0 , 41 , 64 },{ 0 , 42 , 66 },{ 0 , 43 , 68 },{ 0 , 44 , 70 },{ 0 , 45 , 72 },
    { 0 , 47 , 74 },{ 0 , 48 , 76 },{ 0 , 49 , 77 },{ 0 , 50 , 79 },{ 0 , 52 , 81 },{ 0 , 53 , 83 },{ 0 , 54 , 85 },{ 0 , 55 , 87 },{ 0 , 57 , 89 },
    { 0 , 58 , 91 },{ 0 , 59 , 93 },{ 0 , 60 , 95 },{ 0 , 61 , 97 },{ 0 , 63 , 99 },{ 0 , 64 , 101 },{ 0 , 65 , 103 },{ 0 , 66 , 105 },{ 0 , 68 , 107 },
    { 0 , 69 , 108 },{ 0 , 70 , 110 },{ 0 , 71 , 112 },{ 0 , 72 , 114 },{ 0 , 74 , 116 },{ 0 , 75 , 118 },{ 0 , 76 , 120 },{ 0 , 77 , 122 },{ 0 , 79 , 124 },
    { 0 , 80 , 126 },{ 0 , 81 , 128 },{ 0 , 82 , 130 },{ 0 , 83 , 131 },{ 0 , 85 , 133 },{ 0 , 86 , 135 },{ 0 , 87 , 137 },{ 0 , 88 , 139 },{ 0 , 89 , 141 },
    { 0 , 91 , 143 },{ 0 , 92 , 145 },{ 0 , 93 , 146 },{ 0 , 94 , 148 },{ 0 , 95 , 150 },{ 0 , 97 , 152 },{ 0 , 98 , 154 },{ 0 , 99 , 156 },{ 0 , 100 , 158 },
    { 0 , 101 , 160 },{ 0 , 103 , 161 },{ 0 , 104 , 163 },{ 0 , 105 , 165 },{ 0 , 106 , 167 },{ 0 , 107 , 169 },{ 0 , 109 , 171 },{ 0 , 110 , 173 },{ 0 , 111 , 175 },
    { 0 , 112 , 176 },{ 0 , 113 , 178 },{ 0 , 115 , 180 },{ 0 , 116 , 182 },{ 0 , 117 , 184 },{ 0 , 118 , 186 },{ 0 , 119 , 188 },{ 0 , 120 , 190 },{ 0 , 122 , 191 },
    { 0 , 123 , 193 },{ 0 , 124 , 195 },{ 0 , 125 , 197 },{ 0 , 126 , 199 },{ 0 , 128 , 201 },{ 0 , 129 , 203 },{ 0 , 130 , 205 },{ 0 , 131 , 206 },{ 0 , 132 , 208 },
    { 0 , 134 , 210 },{ 0 , 135 , 212 },{ 0 , 136 , 214 },{ 0 , 137 , 216 },{ 0 , 138 , 218 },{ 0 , 140 , 220 },{ 0 , 141 , 221 },{ 0 , 142 , 223 },{ 0 , 143 , 225 },
    { 0 , 144 , 227 },{ 0 , 146 , 229 },{ 0 , 147 , 231 },{ 0 , 148 , 233 },{ 0 , 149 , 235 },{ 0 , 150 , 236 },{ 0 , 152 , 238 },{ 0 , 153 , 240 },{ 0 , 154 , 242 },
    { 6 , 157 , 242 },{ 6 , 157 , 242 },{ 12 , 159 , 243 },{ 19 , 162 , 243 },{ 25 , 165 , 244 },{ 31 , 167 , 244 },{ 37 , 170 , 244 },{ 43 , 173 , 245 },{ 49 , 176 , 245 },
    { 56 , 178 , 246 },{ 62 , 181 , 246 },{ 68 , 184 , 246 },{ 74 , 186 , 247 },{ 80 , 189 , 247 },{ 86 , 192 , 248 },{ 93 , 194 , 248 },{ 99 , 197 , 248 },{ 105 , 200 , 249 },{ 111 , 203 , 249 },{ 117 , 205 , 249 },{ 124 , 208 , 250 },{ 130 , 211 , 250 },{ 136 , 213 , 251 },{ 142 , 216 , 251 },{ 148 , 219 , 251 },{ 154 , 221 , 252 },{ 161 , 224 , 252 },{ 167 , 227 , 253 },{ 173 , 229 , 253 },{ 179 , 232 , 253 },{ 185 , 235 , 254 },{ 192 , 238 , 254 },{ 198 , 240 , 255 },{ 204 , 243 , 255 },{ 205 , 243 , 255 },{ 205 , 243 , 255 },{ 206 , 243 , 255 },{ 206 , 244 , 255 },{ 207 , 244 , 255 },{ 208 , 244 , 255 },{ 208 , 244 , 255 },{ 209 , 244 , 255 },{ 209 , 244 , 255 },{ 210 , 244 , 255 },{ 210 , 244 , 255 },{ 211 , 245 , 255 },{ 212 , 245 , 255 },{ 212 , 245 , 255 },{ 213 , 245 , 255 },{ 213 , 245 , 255 },{ 214 , 245 , 255 },{ 215 , 245 , 255 },{ 215 , 245 , 255 },{ 216 , 246 , 255 },{ 216 , 246 , 255 },{ 217 , 246 , 255 },{ 218 , 246 , 255 },{ 218 , 246 , 255 },{ 219 , 246 , 255 },{ 219 , 246 , 255 },{ 220 , 247 , 255 },{ 221 , 247 , 255 },{ 221 , 247 , 255 },{ 222 , 247 , 255 },{ 222 , 247 , 255 },
    { 223 , 247 , 255 },{ 223 , 247 , 255 },{ 224 , 247 , 255 },{ 224 , 248 , 255 },{ 225 , 248 , 255 },{ 225 , 248 , 255 },{ 226 , 248 , 255 },{ 226 , 248 , 255 },{ 227 , 248 , 255 },{ 227 , 248 , 255 },{ 228 , 248 , 255 },{ 228 , 249 , 255 },{ 229 , 249 , 255 },{ 230 , 249 , 255 },{ 230 , 249 , 255 },{ 231 , 249 , 255 },{ 231 , 249 , 255 },{ 232 , 249 , 255 },{ 232 , 249 , 255 },{ 233 , 250 , 255 },{ 233 , 250 , 255 },{ 234 , 250 , 255 },{ 234 , 250 , 255 },{ 235 , 250 , 255 },{ 235 , 250 , 255 },{ 236 , 250 , 255 },{ 236 , 250 , 255 },{ 237 , 251 , 255 },{ 237 , 251 , 255 },{ 238 , 251 , 255 },{ 238 , 251 , 255 },{ 239 , 251 , 255 },{ 239 , 251 , 255 },{ 240 , 251 , 255 },{ 240 , 251 , 255 },{ 241 , 252 , 255 },{ 241 , 252 , 255 },{ 242 , 252 , 255 },{ 243 , 252 , 255 },{ 243 , 252 , 255 },{ 244 , 252 , 255 },{ 244 , 252 , 255 },{ 245 , 252 , 255 },{ 245 , 253 , 255 },{ 246 , 253 , 255 },{ 246 , 253 , 255 },{ 247 , 253 , 255 },{ 247 , 253 , 255 },{ 248 , 253 , 255 },{ 248 , 253 , 255 },{ 249 , 253 , 255 },{ 249 , 254 , 255 },{ 250 , 254 , 255 },{ 250 , 254 , 255 },{ 251 , 254 , 255 },{ 251 , 254 , 255 },{ 252 , 254 , 255 },{ 252 , 254 , 255 },{ 253 , 254 , 255 },{ 253 , 255 , 255 },{ 254 , 255 , 255 },{ 254 , 255 , 255 },{ 255 , 255 , 255 },{ 255 , 255 , 255 },
};

class Logger : public nvinfer1::ILogger
{
public:
    void log(Severity severity, const char* msg) noexcept override
    {
        // 根据需要自定义日志输出逻辑
        switch (severity)
        {
        case Severity::kINTERNAL_ERROR:
            std::cerr << "INTERNAL_ERROR: " << msg << std::endl;
            break;
        case Severity::kERROR:
            std::cerr << "ERROR: " << msg << std::endl;
            break;
        case Severity::kWARNING:
            std::cerr << "WARNING: " << msg << std::endl;
            break;
        case Severity::kINFO:
            std::cout << "INFO: " << msg << std::endl;
            break;
        default:
            break;
        }
    }
};

using namespace nvinfer1;

static Logger gLogger;

cv::Mat ribbon(cv::Mat image) {
    //applyColorMap(gray_image, color_map, COLORMAP_OCEAN);
    Mat jet_map = Mat::zeros(256, 1, CV_8UC3);
    for (int i = 0; i < 256; i++)
    {
        jet_map.at<Vec3b>(i, 0)[0] = ribbon_C[i][0];
        jet_map.at<Vec3b>(i, 0)[1] = ribbon_C[i][1];
        jet_map.at<Vec3b>(i, 0)[2] = ribbon_C[i][2];
    }
    Mat color_image;
    LUT(image, jet_map, color_image);
    return color_image;
}

void draw_scale99(cv::Mat& image,int scale) {
    line(image, Point(511, 520), Point(49, 296), Scalar(255, 255, 255), 1, LINE_AA);
    line(image, Point(511, 520), Point(973, 296), Scalar(255, 255, 255), 1, LINE_AA);
    line(image, Point(511, 520), Point(512, 10), Scalar(255, 255, 255), 1, LINE_AA);
    line(image, Point(511, 520), Point(256, 79), Scalar(255, 255, 255), 1, LINE_AA);
    line(image, Point(511, 520), Point(768, 79), Scalar(255, 255, 255), 1, LINE_AA);

    ellipse(image, Point(511, 520), Size(102, 102), 0, -26, -154, Scalar(255, 255, 255), 1, LINE_AA);
    ellipse(image, Point(511, 520), Size(204, 204), 0, -26, -154, Scalar(255, 255, 255), 1, LINE_AA);
    ellipse(image, Point(511, 520), Size(306, 306), 0, -26, -154, Scalar(255, 255, 255), 1, LINE_AA);
    ellipse(image, Point(511, 521), Size(409, 409), 0, -26, -154, Scalar(255, 255, 255), 1, LINE_AA);
    ellipse(image, Point(511, 521), Size(513, 513), 0, -26, -154, Scalar(255, 255, 255), 1, LINE_AA);

    stringstream ss_0 ;
    ss_0 << scale;
    string str_0 = ss_0.str();
    putText(image, str_0 + ".0m", Point(12, 315), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);
    putText(image, str_0 + ".0m", Point(970, 315), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);

    stringstream ss_1;
    ss_1 << ((static_cast<double>(scale))*0.8);
    string str_1 = ss_1.str();
    //std::cout << "size = " << str_1.size() << std::endl;
    if (str_1.size() <= 2)
    {
        str_1 = str_1 + ".0";
    }
    putText(image, str_1 + "m", Point(105, 360), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);
    putText(image, str_1 + "m", Point(870, 360), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);

    stringstream ss_2;
    ss_2 << (static_cast<double>(scale)) * 0.6;
    string str_2 = ss_2.str();
    if (str_2.size() <= 2)
    {
        str_2 = str_2 +".0";
    }
    putText(image, str_2 + "m", Point(199, 408), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);
    putText(image, str_2 + "m", Point(776, 408), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);

    stringstream ss_3;
    ss_3 << (static_cast<double>(scale)) * 0.4;
    string str_3 = ss_3.str();
    if (str_3.size() <= 2)
    {
        str_3 = str_3 +".0";
    }
    putText(image, str_3 + "m", Point(290, 450), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);
    putText(image, str_3 + "m", Point(687, 450), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);

    stringstream ss_4;
    ss_4 << (static_cast<float>(scale)) * 0.2 ;
    string str_4 = ss_4.str();
    if (str_4.size() <= 2)
    {
        str_4 = str_4 +".0";
    }
    putText(image, str_4 + "m", Point(381, 492), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);
    putText(image, str_4 + "m", Point(600, 492), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);
}

void draw_scale10(cv::Mat& image, int scale) {
    line(image, Point(511, 520), Point(49, 296), Scalar(255, 255, 255), 1, LINE_AA);
    line(image, Point(511, 520), Point(973, 296), Scalar(255, 255, 255), 1, LINE_AA);
    line(image, Point(511, 520), Point(512, 10), Scalar(255, 255, 255), 1, LINE_AA);
    line(image, Point(511, 520), Point(256, 79), Scalar(255, 255, 255), 1, LINE_AA);
    line(image, Point(511, 520), Point(768, 79), Scalar(255, 255, 255), 1, LINE_AA);

    ellipse(image, Point(511, 520), Size(102, 102), 0, -26, -154, Scalar(255, 255, 255), 1, LINE_AA);
    ellipse(image, Point(511, 520), Size(204, 204), 0, -26, -154, Scalar(255, 255, 255), 1, LINE_AA);
    ellipse(image, Point(511, 520), Size(306, 306), 0, -26, -154, Scalar(255, 255, 255), 1, LINE_AA);
    ellipse(image, Point(511, 521), Size(409, 409), 0, -26, -154, Scalar(255, 255, 255), 1, LINE_AA);
    ellipse(image, Point(511, 521), Size(513, 513), 0, -26, -154, Scalar(255, 255, 255), 1, LINE_AA);

    stringstream ss_0;
    ss_0 << scale;
    string str_0 = ss_0.str();
    putText(image, str_0 + ".0m", Point(12, 315), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);
    putText(image, str_0 + ".0m", Point(970, 315), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);

    stringstream ss_1;
    ss_1 << (static_cast<float>(scale)) * 0.8;
    string str_1 = ss_1.str();
    putText(image, str_1 + "m", Point(105, 360), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);
    putText(image, str_1 + "m", Point(873, 360), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);

    stringstream ss_2;
    ss_2 << (static_cast<float>(scale)) * 0.6;
    string str_2 = ss_2.str();
    putText(image, str_2 + "m", Point(199, 408), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);
    putText(image, str_2 + "m", Point(776, 408), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);

    stringstream ss_3;
    ss_3 << (static_cast<float>(scale)) * 0.4;
    string str_3 = ss_3.str();
    putText(image, str_3 + "m", Point(290, 450), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);
    putText(image, str_3 + "m", Point(687, 450), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);

    stringstream ss_4;
    ss_4 << (static_cast<float>(scale)) * 0.2;
    string str_4 = ss_4.str();
    putText(image, str_4 + "m", Point(381, 492), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);
    putText(image, str_4 + "m", Point(600, 492), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);
}

void draw_scale100(cv::Mat& image)
{
    line(image, Point(511, 520), Point(49, 296), Scalar(255, 255, 255), 1, LINE_AA);
    line(image, Point(511, 520), Point(973, 296), Scalar(255, 255, 255), 1, LINE_AA);
    line(image, Point(511, 520), Point(512, 10), Scalar(255, 255, 255), 1, LINE_AA);
    line(image, Point(511, 520), Point(256, 79), Scalar(255, 255, 255), 1, LINE_AA);
    line(image, Point(511, 520), Point(768, 79), Scalar(255, 255, 255), 1, LINE_AA);

    ellipse(image, Point(511, 520), Size(102, 102), 0, -26, -154, Scalar(255, 255, 255), 1, LINE_AA);
    ellipse(image, Point(511, 520), Size(204, 204), 0, -26, -154, Scalar(255, 255, 255), 1, LINE_AA);
    ellipse(image, Point(511, 520), Size(306, 306), 0, -26, -154, Scalar(255, 255, 255), 1, LINE_AA);
    ellipse(image, Point(511, 521), Size(409, 409), 0, -26, -154, Scalar(255, 255, 255), 1, LINE_AA);
    ellipse(image, Point(511, 521), Size(513, 513), 0, -26, -154, Scalar(255, 255, 255), 1, LINE_AA);

    putText(image, "100.0m", Point(7, 318), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);
    putText(image, "100.0m", Point(962, 318), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);

    putText(image, "80.0m", Point(100, 363), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);
    putText(image, "80.0m", Point(878, 362), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);

    putText(image, "60.0m", Point(194, 411), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);
    putText(image, "60.0m", Point(780, 411), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);

    putText(image, "40.0m", Point(284, 453), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);
    putText(image, "40.0m", Point(685, 453), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);

    putText(image, "20.0m", Point(382, 495), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);
    putText(image, "20.0m", Point(600, 495), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);
}

void adjustBrightnessContrast(Mat& image, int brightness, int contrast) {
    // 调整亮度和对比度
    image.convertTo(image, -1, contrast / 100.0 + 1, brightness - 50);
}

int main(int argc, char** argv)
{
    Coordinate_transformation Coordinate_transformation_er;
    // socket环境
    WSADATA  wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cout << "WSAStartup error:" << GetLastError() << endl;
        return false;
    }

    // socket对象
        // 创建UDP接收端socket
    SOCKET recvSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (recvSocket == INVALID_SOCKET) {
        cerr << "Failed to create receive socket" << endl;
        WSACleanup();
        return 1;
    }

    //std::cout << ip << port<<std::endl;
    // 连接到服务器
        // 设置接收端地址
    sockaddr_in recvAddr;
    recvAddr.sin_family = AF_INET;
    recvAddr.sin_port = htons(8085);
    recvAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (::bind(recvSocket, (sockaddr*)&recvAddr, sizeof(recvAddr)) == SOCKET_ERROR) {
        cerr << "Failed to bind receive socket" << endl;
        closesocket(recvSocket);
        WSACleanup();
        return 1;
    }


    // nvinfer1::ILogger* gLogger; 
    initLibNvInferPlugins(&gLogger, "");
    // 创建TensorRT的运行时对象
    IRuntime* runtime = createInferRuntime(gLogger);

    // 从文件中反序列化Engine对象
    std::ifstream engineFile(enginePath, std::ios::binary);
    if (!engineFile)
    {
        std::cerr << "无法打开Engine文件进行读取。" << std::endl;
        return 1;
    }

    engineFile.seekg(0, std::ios::end);
    const size_t fileSize = engineFile.tellg();
    engineFile.seekg(0, std::ios::beg);
    std::vector<char> engineData(fileSize);
    engineFile.read(engineData.data(), fileSize);
    engineFile.close();
    auto stop = high_resolution_clock::now();
  //  auto duration = duration_cast<microseconds>(stop - start);  微秒 1ms = 1000us
  //  auto duration = duration_cast<milliseconds>(stop - start);
  //  cout << "time is " << duration.count() << " ms" << endl;
    // 反序列化Engine对象
    IPluginFactory* pluginFactory = nullptr;  // 如果有自定义插件，可以传递一个插件工厂对象
    ICudaEngine* engine = runtime->deserializeCudaEngine(engineData.data(), fileSize, pluginFactory);
    if (!engine)
    {
        std::cerr << "无法反序列化Engine对象。" << std::endl;
        return 1;
    }

    // 创建TensorRT的执行上下文对象
    IExecutionContext* context = engine->createExecutionContext();

    // 分配输入和输出内存
    const int batchSize = 1;

    const int inputSize = 256 * 256; // 输入张量的大小
    const int outputSize = 256 * 256; // 输出张量的大小
    //const int inputSize = 512 * 512; // 输入张量的大小
    //const int outputSize = 512 * 512; // 输出张量的大小

    void* deviceInput;
    void* deviceOutput;

    cudaMalloc(&deviceInput, batchSize * inputSize * sizeof(float));
    cudaMalloc(&deviceOutput, batchSize * outputSize * sizeof(float));
    std::vector<void*> bindings = { deviceInput,deviceOutput };

    // 创建CUDA流
    cudaStream_t stream;
    cudaStreamCreate(&stream);
    // 将输入数据复制到GPU内存中
    std::vector<float> input(batchSize * inputSize);

    // 计数器，用于生成文件名
    int counter = 1;
    int imgIndex = 0;
	//cv::Rect rect(1272, 0, 940, 940);
    cv::Rect rect(512, 0, 512, 512);


    // 初始化压缩参数
    std::vector<int> compression_params;
    compression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
    compression_params.push_back(35);  // 设置JPEG压缩质量，范围0-100

    // 接收数据

    cv::Mat receivedImage;
    cv::Mat croppedImage;
    while (true)
    {
        //char c_buffer[2] = { 0 };
        //int bytes_0 = recv(clientSock, c_buffer, 2, 0);
        //std::string receivedData(c_buffer, 2);
        //std::cout << "receivedData = " << receivedData << std::endl;
        //int bytes = recv(clientSock, buffer.data(), buffer.size(), 0);
        int scale = 100;
        int imgSize = 0;
        uchar buffer[MAX_BUFFER_SIZE];
        int addrLen = sizeof(recvAddr);
        int bytes = recvfrom(recvSocket, (char*)&buffer, MAX_BUFFER_SIZE, 0, (sockaddr*)&recvAddr, &addrLen);
        std::cout << "bytes = " << bytes << std::endl;
        if (bytes > sizeof(int)) {
            memcpy(&scale, buffer , sizeof(int));
            memcpy(&imgSize, buffer + sizeof(int), sizeof(int));
            vector<uchar> imageData(buffer + sizeof(int) + sizeof(int), buffer + sizeof(int) + sizeof(int) + imgSize);
            // 提取图像大小
           // memcpy(&imgSize, buffer, sizeof(int));

            // 提取图像数据
            //vector<uchar> imageData(buffer + sizeof(int), buffer + sizeof(int) + imgSize);

            // 解码图像数据1
            receivedImage = cv::imdecode(imageData, cv::IMREAD_COLOR);
            if (scale == 100)
            {
                croppedImage = receivedImage;
                //croppedImage = frame(cv::Rect(0, 512 - newHeight, newWidth, newHeight));
            }

            else
            {
                int newHeight = scale * 512 / 100;
                // 计算裁剪后的宽度
                int newWidth = receivedImage.cols;
                // 创建一个新的Mat对象来存储裁剪后的图片
                croppedImage = receivedImage(cv::Rect(0, 512 - newHeight, newWidth, newHeight));
            }
        }
        else
        {
            continue;
        }

        // 在这里对接收到的图像进行处理，例如显示或保存到文件
        //cv::Mat frame = cv::imdecode(buffer, cv::IMREAD_COLOR);
        // cv::Mat frame = cv::imread("C:\\mao\\code\\tensorrt\\cyclegan\\test00000.png");
        // 设置裁剪的高度
        //cv::Mat croppedImage;
        //if (receivedData == "AB")
        //{
        //    int newHeight = 170;
        //    // 计算裁剪后的宽度
        //    int newWidth = frame.cols;
        //    // 创建一个新的Mat对象来存储裁剪后的图片
        //    croppedImage = frame(cv::Rect(0, 512 - newHeight, newWidth, newHeight));
        //}
        //else if (receivedData == "AC")
        //{
        //    int newHeight = 340;
        //    // 计算裁剪后的宽度
        //    int newWidth = frame.cols;
        //    // 创建一个新的Mat对象来存储裁剪后的图片
        //    croppedImage = frame(cv::Rect(0, 512 - newHeight, newWidth, newHeight));
        //}
        //else
        //{
        //    croppedImage = frame;
        //}

        cv::Mat frame2;
        cv::resize(croppedImage, frame2, cv::Size(1024, 512));

        //cv::blur(frame, frame, cv::Size(21, 21));
        //cv::GaussianBlur(frame, frame, cv::Size(21, 25), 0);

        cv::cvtColor(frame2, frame2, cv::COLOR_BGR2GRAY);
        cv::Mat out_image(512, 512, CV_8UC1, cv::Scalar(0, 0, 0));
        cv::flip(frame2, frame2, 0);
        // cv::flip(frame, frame, 1);
           
        //cv::resize(roi, roi, cv::Size(1024, 512));
        //            auto start = high_resolution_clock::now();
        Coordinate_transformation_er.preprocess(frame2, out_image);

        // cv::cvtColor(out_image, out_image, cv::COLOR_BGR2GRAY);
        // 生成文件名

        cv::Mat image = 255 - out_image;
        cv::resize(image, image, cv::Size(256, 256));
        cv::Mat normalized_image;
        cv::normalize(image, normalized_image, 0, 255, cv::NORM_MINMAX, CV_8U);

        // 将图像标准化（均值为 0，标准差为 1）
        cv::Mat standardized_image;
        cv::Mat float_image;
        image.convertTo(float_image, CV_32F);  // 将图像转换为浮点数格式
        cv::normalize(float_image, standardized_image, 0, 1, cv::NORM_MINMAX, CV_32F);


        // adjustBrightnessContrast(image, 5, 150);
        // 将图片数据复制到输入张量
        //cv::Mat resizedImage;
        //cv::resize(image, resizedImage, cv::Size(512, 512));
        //cv::resize(standardized_image, resizedImage, cv::Size(256, 256));
        cv::Mat floatImage;
        // resizedImage.convertTo(floatImage, CV_32F, 1.0 / 255.0);
        std::vector<cv::Mat> inputChannels;

        cv::split(standardized_image, inputChannels);

        std::cout << inputChannels.size() << std::endl;
        std::memcpy(input.data(), inputChannels[0].data, inputSize * sizeof(float));

        cudaMemcpyAsync(deviceInput, input.data(), batchSize * inputSize * sizeof(float), cudaMemcpyHostToDevice, stream);

        // 执行推理
        context->enqueue(batchSize, bindings.data(), stream, nullptr);

        // 将输出数据从GPU内存复制回主机内存
        std::vector<float> output(batchSize * outputSize);
        cudaMemcpyAsync(output.data(), deviceOutput, batchSize * outputSize * sizeof(float), cudaMemcpyDeviceToHost, stream);

        // 等待推理完成
        cudaStreamSynchronize(stream);

        // 处理输出结果
        // std::cout << output.data() << std::endl;
        for (float& value : output) {
            value *= -255;
            value = 1 - value;
        }

        std::vector<std::vector<int>> matrix(256, std::vector<int>(256));

        // 将一维向量转换为二维向量
        for (int i = 0; i < 256; i++) {
            for (int j = 0; j < 256; j++) {
                matrix[i][j] = output[i * 256 + j];
            }
        }

        // 创建Mat对象并将二维向量复制到该对象中
        cv::Mat img(256, 256, CV_8UC1);
        for (int i = 0; i < 256; i++) {
            for (int j = 0; j < 256; j++) {
                img.at<uchar>(i, j) = matrix[i][j];
            }
        }

        adjustBrightnessContrast(img, 35,200);
        //cv::medianBlur(img, img, 3);

        cv::resize(img, img, cv::Size(512, 512));
        // cv::blur(img, img, cv::Size(9, 9));
        // cv::GaussianBlur(img, img, cv::Size(9, 9), 0);
        cv::Mat xyimage2(512, 1024, CV_8UC1, cv::Scalar(0));
        //   cv::imwrite("test0000.png", xyimage2);//////////
            // 将像元值小于10的像元加100
        // cv::Mat result_image = image.clone();
        Coordinate_transformation_er.polar2xy(xyimage2, img);
        cv::Mat image_test = cv::imread("template.png", 0);
        // 图像相乘
        Mat result;
        cv::multiply(xyimage2, image_test, result);
        cv::flip(result, result, -1);
        cv::flip(result, result, 1);
        std::vector<Mat>src;
        src.emplace_back(result);
        src.emplace_back(result);
        src.emplace_back(result);
        static cv::Mat dst;
        cv::merge(src, dst);
        Mat result_0, out_resize_image;
        result_0 = ribbon(dst);
        cv::Mat image_temp = cv::Mat::zeros(cv::Size(1024, 10), CV_8UC3);
        cv::vconcat(image_temp, result_0, out_resize_image);
        cv::vconcat(out_resize_image, image_temp, out_resize_image);
        if (scale == 100)
        {
            draw_scale100(out_resize_image);
        }
        else
        {
            draw_scale99(out_resize_image, scale);
        }

        //imshow("Received Image2", out_resize_image);
        //cv::waitKey(1);  // 等待一段时间，以便显示图像

        vector<uchar> encodedImage;
        imencode(".jpg", out_resize_image, encodedImage, compression_params);
        int imageSize = 0;
        imageSize = encodedImage.size();
        std::cout << "imageSize = " << imageSize << std::endl;
        // 组合图像大小和图像数据到一个数据包 
        uchar buffer_s[MAX_BUFFER_SIZE];
        memcpy(buffer_s, &imageSize, sizeof(int));
        memcpy(buffer_s + sizeof(int), encodedImage.data(), imageSize);
        int image_size_Sent = sendto(recvSocket, (char*)buffer_s, sizeof(int) + imageSize, 0, (struct sockaddr*)&recvAddr, sizeof(recvAddr));
        if (image_size_Sent < 0) {
            cerr << "Error: Unable to send image size" << endl;
            break;
        }


    }


    // 释放资源
    context->destroy();
    engine->destroy();
    runtime->destroy();
    return 0;
}



#ifdef false


int main(int argc, char** argv)
{
    Coordinate_transformation Coordinate_transformation_er;
    cv::Mat image_test = cv::imread("C:\\Users\\mao\\Desktop\\55.png");
    cv::resize(image_test, image_test, cv::Size(1024, 512)); 
    cv::Mat out_image(512, 512, CV_8UC1);
    auto start = high_resolution_clock::now();
    Coordinate_transformation_er.preprocess(image_test, out_image);


 //   cv::GaussianBlur(out_image, out_image, cv::Size(9, 9),0);
    cv::medianBlur(out_image, out_image, 3);
    // nvinfer1::ILogger* gLogger; 
    initLibNvInferPlugins(&gLogger, "");
    // 创建TensorRT的运行时对象
    IRuntime* runtime = createInferRuntime(gLogger);

    // 从文件中反序列化Engine对象
    std::ifstream engineFile(enginePath, std::ios::binary);
    if (!engineFile)
    {
        std::cerr << "无法打开Engine文件进行读取。" << std::endl;
        return 1;
    }
    engineFile.seekg(0, std::ios::end);
    const size_t fileSize = engineFile.tellg();
    engineFile.seekg(0, std::ios::beg);
    std::vector<char> engineData(fileSize);
    engineFile.read(engineData.data(), fileSize);
    engineFile.close();
    auto stop = high_resolution_clock::now();
    //  auto duration = duration_cast<microseconds>(stop - start);  微秒 1ms = 1000us
    auto duration = duration_cast<milliseconds>(stop - start);
    cout << "time is " << duration.count() << " ms" << endl;
    // 反序列化Engine对象
    IPluginFactory* pluginFactory = nullptr;  // 如果有自定义插件，可以传递一个插件工厂对象
    ICudaEngine* engine = runtime->deserializeCudaEngine(engineData.data(), fileSize, pluginFactory);
    if (!engine)
    {
        std::cerr << "无法反序列化Engine对象。" << std::endl;
        return 1;
    }

    // 创建TensorRT的执行上下文对象
    IExecutionContext* context = engine->createExecutionContext();

    // 分配输入和输出内存
    const int batchSize = 1;
    const int inputSize = 512 * 512; // 输入张量的大小
    const int outputSize = 512 * 512; // 输出张量的大小

    void* deviceInput;
    void* deviceOutput;

    cudaMalloc(&deviceInput, batchSize * inputSize * sizeof(float));
    cudaMalloc(&deviceOutput, batchSize * outputSize * sizeof(float));
    std::vector<void*> bindings = { deviceInput,deviceOutput };

    // 创建CUDA流
    cudaStream_t stream;
    cudaStreamCreate(&stream);
    // 将输入数据复制到GPU内存中
    std::vector<float> input(batchSize * inputSize); 

    // 读取图片作为输入
  //  cv::Mat image = cv::imread("C:\\mao\\code\\python\\GAN\\image3_test.png", 0);

    cv::Mat image = out_image;
    // 将图片数据复制到输入张量
    cv::Mat resizedImage;
    cv::resize(image, resizedImage, cv::Size(512, 512));
    cv::Mat floatImage;
    resizedImage.convertTo(floatImage, CV_32F, 1.0 / 255.0);
    std::vector<cv::Mat> inputChannels;
    
    cv::split(floatImage, inputChannels);

    std::cout << inputChannels.size() << std::endl;
    std::memcpy(input.data(), inputChannels[0].data, inputSize * sizeof(float));

    cudaMemcpyAsync(deviceInput, input.data(),batchSize * inputSize * sizeof(float), cudaMemcpyHostToDevice, stream);

    // 执行推理
    context->enqueue(batchSize, bindings.data(), stream, nullptr);

    // 将输出数据从GPU内存复制回主机内存
    std::vector<float> output(batchSize * outputSize);
    cudaMemcpyAsync(output.data(), deviceOutput, batchSize * outputSize * sizeof(float), cudaMemcpyDeviceToHost, stream);

    // 等待推理完成
    cudaStreamSynchronize(stream);

    // 处理输出结果
    std::cout << output.data() << std::endl;
    for (float& value : output) {
        value *= -255;
        value = 1 - value;
    }

    std::vector<std::vector<int>> matrix(512, std::vector<int>(512));

    // 将一维向量转换为二维向量
    for (int i = 0; i < 512; i++) {
        for (int j = 0; j < 512; j++) {
            matrix[i][j] = output[i * 512 + j];
        }
    }

    // 创建Mat对象并将二维向量复制到该对象中
    cv::Mat img(512, 512, CV_8UC1);
    for (int i = 0; i < 512; i++) {
        for (int j = 0; j < 512; j++) {
            img.at<uchar>(i, j) = matrix[i][j];
        }
    }
    cv::imwrite("test112.png", img);
    cv::medianBlur(img, img, 3);
   // cv::blur(img, img, cv::Size(9, 9));

    cv::Mat xyimage2(512, 1024, CV_8UC1, cv::Scalar(81));
    cv::imwrite("test0000.png", xyimage2);
    Coordinate_transformation_er.polar2xy(xyimage2, img);
    
   //cv::GaussianBlur(img, img, cv::Size(9, 9), 0);
    cv::imwrite("test113.png", xyimage2);
    // 释放资源
    context->destroy();
    engine->destroy();
    runtime->destroy();

    return 0;
}


#endif // false
#include "udp_receive.h"
#include "yolov8-seg.h"
#include <string>
#include <opencv2/opencv.hpp>
#include "detect.h"
#include <iostream>
#include <cmath>
#include <windows.h> // Windows API  
#include <filesystem> // C++17 
// #define LOG_FILE "zmq_log.txt"
// yolo detect
const std::vector<std::string> CLASS_NAMES = {
    "object", "object", "car", "motorcycle", "airplane", "bus",
    "train", "truck", "boat", "traffic light", "fire hydrant",
    "stop sign", "parking meter", "bench", "bird", "cat",
    "dog", "horse", "sheep", "cow", "elephant",
    "bear", "zebra", "giraffe", "backpack", "umbrella",
    "handbag", "tie", "suitcase", "frisbee", "skis",
    "snowboard", "sports ball", "kite", "baseball bat", "baseball glove",
    "skateboard", "surfboard", "tennis racket", "bottle", "wine glass",
    "cup", "fork", "knife", "spoon", "bowl",
    "banana", "apple", "sandwich", "orange", "broccoli",
    "carrot", "hot dog", "pizza", "donut", "cake",
    "chair", "couch", "potted plant", "bed", "dining table",
    "toilet", "tv", "laptop", "mouse", "remote",
    "keyboard", "cell phone", "microwave", "oven",
    "toaster", "sink", "refrigerator", "book", "clock", "vase",
    "scissors", "teddy bear", "hair drier", "toothbrush" };

const std::vector<std::vector<unsigned int>> COLORS = {
    { 0, 114, 189 }, { 217, 83, 25 }, { 237, 177, 32 },
    { 126, 47, 142 }, { 119, 172, 48 }, { 77, 190, 238 },
    { 162, 20, 47 }, { 76, 76, 76 }, { 153, 153, 153 },
    { 255, 0, 0 }, { 255, 128, 0 }, { 191, 191, 0 },
    { 0, 255, 0 }, { 0, 0, 255 }, { 170, 0, 255 },
    { 85, 85, 0 }, { 85, 170, 0 }, { 85, 255, 0 },
    { 170, 85, 0 }, { 170, 170, 0 }, { 170, 255, 0 },
    { 255, 85, 0 }, { 255, 170, 0 }, { 255, 255, 0 },
    { 0, 85, 128 }, { 0, 170, 128 }, { 0, 255, 128 },
    { 85, 0, 128 }, { 85, 85, 128 }, { 85, 170, 128 },
    { 85, 255, 128 }, { 170, 0, 128 }, { 170, 85, 128 },
    { 170, 170, 128 }, { 170, 255, 128 }, { 255, 0, 128 },
    { 255, 85, 128 }, { 255, 170, 128 }, { 255, 255, 128 },
    { 0, 85, 255 }, { 0, 170, 255 }, { 0, 255, 255 },
    { 85, 0, 255 }, { 85, 85, 255 }, { 85, 170, 255 },
    { 85, 255, 255 }, { 170, 0, 255 }, { 170, 85, 255 },
    { 170, 170, 255 }, { 170, 255, 255 }, { 255, 0, 255 },
    { 255, 85, 255 }, { 255, 170, 255 }, { 85, 0, 0 },
    { 128, 0, 0 }, { 170, 0, 0 }, { 212, 0, 0 },
    { 255, 0, 0 }, { 0, 43, 0 }, { 0, 85, 0 },
    { 0, 128, 0 }, { 0, 170, 0 }, { 0, 212, 0 },
    { 0, 255, 0 }, { 0, 0, 43 }, { 0, 0, 85 },
    { 0, 0, 128 }, { 0, 0, 170 }, { 0, 0, 212 },
    { 0, 0, 255 }, { 0, 0, 0 }, { 36, 36, 36 },
    { 73, 73, 73 }, { 109, 109, 109 }, { 146, 146, 146 },
    { 182, 182, 182 }, { 219, 219, 219 }, { 0, 114, 189 },
    { 80, 183, 189 }, { 128, 128, 0 }
};

const std::vector<std::vector<unsigned int>> MASK_COLORS = {
	{ 255, 255,255 }, { 255, 255, 255 }, { 255, 255, 255 },
	{ 255, 178, 29 }, { 207, 210, 49 }, { 72, 249, 10 },
	{ 146, 204, 23 }, { 61, 219, 134 }, { 26, 147, 52 },
	{ 0, 212, 187 }, { 44, 153, 168 }, { 0, 194, 255 },
	{ 52, 69, 147 }, { 100, 115, 255 }, { 0, 24, 236 },
	{ 132, 56, 255 }, { 82, 0, 133 }, { 203, 56, 255 },
	{ 255, 149, 200 }, { 255, 55, 199 }
};

int ribbon_C[256][3] =
{
    { 0 , 1 , 2 },{ 0 , 2 , 4 },{ 0 , 4 , 6 },{ 0 , 5 , 8 },{ 0 , 6 , 10 },{ 0 , 0 , 0 },{ 0 , 9 , 14 },{ 0 , 10 , 15 },{ 0 , 11 , 17 },{ 0 , 12 , 19 },
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
    { 56 , 178 , 246 },{ 62 , 181 , 246 },{ 68 , 184 , 246 },{ 74 , 186 , 247 },{ 80 , 189 , 247 },{ 86 , 192 , 248 },{ 93 , 194 , 248 },{ 99 , 197 , 248 },{ 105 , 200 , 249 },
    { 111 , 203 , 249 },{ 117 , 205 , 249 },{ 124 , 208 , 250 },{ 130 , 211 , 250 },{ 136 , 213 , 251 },{ 142 , 216 , 251 },{ 148 , 219 , 251 },{ 154 , 221 , 252 },
    { 161 , 224 , 252 },{ 167 , 227 , 253 },{ 173 , 229 , 253 },{ 179 , 232 , 253 },{ 185 , 235 , 254 },{ 192 , 238 , 254 },{ 198 , 240 , 255 },{ 204 , 243 , 255 },
    { 205 , 243 , 255 },{ 205 , 243 , 255 },{ 206 , 243 , 255 },{ 206 , 244 , 255 },{ 207 , 244 , 255 },{ 208 , 244 , 255 },{ 208 , 244 , 255 },{ 209 , 244 , 255 },
    { 209 , 244 , 255 },{ 210 , 244 , 255 },{ 210 , 244 , 255 },{ 211 , 245 , 255 },{ 212 , 245 , 255 },{ 212 , 245 , 255 },{ 213 , 245 , 255 },{ 213 , 245 , 255 },
    { 214 , 245 , 255 },{ 215 , 245 , 255 },{ 215 , 245 , 255 },{ 216 , 246 , 255 },{ 216 , 246 , 255 },{ 217 , 246 , 255 },{ 218 , 246 , 255 },{ 218 , 246 , 255 },
    { 219 , 246 , 255 },{ 219 , 246 , 255 },{ 220 , 247 , 255 },{ 221 , 247 , 255 },{ 221 , 247 , 255 },{ 222 , 247 , 255 },{ 222 , 247 , 255 },
    { 223 , 247 , 255 },{ 223 , 247 , 255 },{ 224 , 247 , 255 },{ 224 , 248 , 255 },{ 225 , 248 , 255 },{ 225 , 248 , 255 },{ 226 , 248 , 255 },{ 226 , 248 , 255 },
    { 227 , 248 , 255 },{ 227 , 248 , 255 },{ 228 , 248 , 255 },{ 228 , 249 , 255 },{ 229 , 249 , 255 },{ 230 , 249 , 255 },{ 230 , 249 , 255 },{ 231 , 249 , 255 },
    { 231 , 249 , 255 },{ 232 , 249 , 255 },{ 232 , 249 , 255 },{ 233 , 250 , 255 },{ 233 , 250 , 255 },{ 234 , 250 , 255 },{ 234 , 250 , 255 },{ 235 , 250 , 255 },
    { 235 , 250 , 255 },{ 236 , 250 , 255 },{ 236 , 250 , 255 },{ 237 , 251 , 255 },{ 237 , 251 , 255 },{ 238 , 251 , 255 },{ 238 , 251 , 255 },{ 239 , 251 , 255 },
    { 239 , 251 , 255 },{ 240 , 251 , 255 },{ 240 , 251 , 255 },{ 241 , 252 , 255 },{ 241 , 252 , 255 },{ 242 , 252 , 255 },{ 243 , 252 , 255 },{ 243 , 252 , 255 },
    { 244 , 252 , 255 },{ 244 , 252 , 255 },{ 245 , 252 , 255 },{ 245 , 253 , 255 },{ 246 , 253 , 255 },{ 246 , 253 , 255 },{ 247 , 253 , 255 },{ 247 , 253 , 255 },
    { 248 , 253 , 255 },{ 248 , 253 , 255 },{ 249 , 253 , 255 },{ 249 , 254 , 255 },{ 250 , 254 , 255 },{ 250 , 254 , 255 },{ 251 , 254 , 255 },{ 251 , 254 , 255 },
    { 252 , 254 , 255 },{ 252 , 254 , 255 },{ 253 , 254 , 255 },{ 253 , 255 , 255 },{ 254 , 255 , 255 },{ 254 , 255 , 255 },{ 255 , 255 , 255 },{ 255 , 255 , 255 },
};

std::string getExecutableDirectory() {  
    char buffer[MAX_PATH];  
    GetModuleFileNameA(NULL, buffer, MAX_PATH);  
    std::string exePath(buffer);  
    std::filesystem::path exeDir = std::filesystem::path(exePath).parent_path();  
    return exeDir.string();  
}  

udp_receive::udp_receive()
{
    std::string tcpmsg;
    inifile::IniFile FsonarFile;
    FsonarFile.Load(CommonGetExePath() + "configs/FsonarFile.ini");
    FsonarFile.GetStringValue("PORT", "PORT", &tcpmsg);
    if (tcpmsg.size() == 0) {
        HERROR("read configs failed {:s}", "FsonarFile.ini");
        std::cout << "read configs failed\n";
    }
    int initport = stoi(tcpmsg);
    std::cout << "initport: " << initport << std::endl;
    //udp_flag = false;
    cudaSetDevice(0);

    char path[MAX_PATH];
    GetModuleFileName(NULL, path, MAX_PATH);

    std::string exeDir = std::string(path);
    std::string exePath = exeDir.substr(0, exeDir.find_last_of("\\"));
    
    // 加载配置文件 TO检测
    std::string configFileName = "sonard_best.engine";
    std::string configFilePath = exePath + "\\\\" + configFileName;
    std::cout << "Config file path: " << configFilePath << std::endl;
    this->yolov8Ptr = new YOLOv8(configFilePath);
    this->yolov8Ptr->make_pipe(true);

    // 加载配置文件 TO分割
    std::string configFileName_seg = "best.engine";
    std::string configFilePath_seg = exePath + "\\\\" + configFileName_seg;
    std::cout << "Config file path seg: " << configFilePath_seg << std::endl;
    this->ms_yolov8Ptr = new YOLOv8_seg(configFilePath_seg);
    this->ms_yolov8Ptr->make_pipe(true);
    // 加载配置文件 TO风格迁移
    std::string configFileName_cyc = "c.trt";
    std::string configFilePath_cyc = exePath + "\\\\" + configFileName_cyc;
    std::cout << "Config file path cyc: " << configFilePath_cyc << std::endl;
    this->cyclerPtr = new cycle(configFilePath_cyc);

    // 加载配置文件 
    // std::string exeDir_image = getExecutableDirectory();  
    // std::cout << "exeDir_image Directory: " << exeDir_image << std::endl;
    // std::string imagePath = exeDir_image + "\\template.png";  
    // this->image_test = cv::imread(imagePath,0); 

    // std::cout << "Config file path template: " << configFilePath_template << std::endl;
    std::string configFileName_template = "template.png";
    std::string configFilePath_template = exePath + "\\\\" + configFileName_template;
    std::cout << "Config file path template: " << configFilePath_template << std::endl;
    this->image_test = cv::imread(configFilePath_template, 0);

    // cv::imwrite("147.png",image_test); 
    
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed." << std::endl;
    }

    this->udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (this->udpSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create socket." << std::endl;
        WSACleanup();
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(initport); 
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (::bind(this->udpSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed." << std::endl;
        closesocket(this->udpSocket);
        WSACleanup();
    }
}

void udp_receive::adjustBrightnessContrast(cv::Mat& image, int brightness, int contrast) {
    // 调整亮度和对比度
    image.convertTo(image, -1, contrast / 100.0 + 1, brightness - 50);
}

void udp_receive::draw_scale(cv::Mat& image)
{
	line(image, cv::Point(511, 520), cv::Point(49, 296), cv::Scalar(255, 255, 255), 1, cv::LINE_AA);
	line(image, cv::Point(511, 520), cv::Point(973, 296), cv::Scalar(255, 255, 255), 1, cv::LINE_AA);
	line(image, cv::Point(511, 520), cv::Point(512, 10), cv::Scalar(255, 255, 255), 1, cv::LINE_AA);
	line(image, cv::Point(511, 520), cv::Point(256, 79), cv::Scalar(255, 255, 255), 1, cv::LINE_AA);
	line(image, cv::Point(511, 520), cv::Point(768, 79), cv::Scalar(255, 255, 255), 1, cv::LINE_AA);

	ellipse(image, cv::Point(511, 520), cv::Size(102, 102), 0, -26, -154, cv::Scalar(255, 255, 255), 1, cv::LINE_AA);
	ellipse(image, cv::Point(511, 520), cv::Size(204, 204), 0, -26, -154, cv::Scalar(255, 255, 255), 1, cv::LINE_AA);
	ellipse(image, cv::Point(511, 520), cv::Size(306, 306), 0, -26, -154, cv::Scalar(255, 255, 255), 1, cv::LINE_AA);
	ellipse(image, cv::Point(511, 521), cv::Size(409, 409), 0, -26, -154, cv::Scalar(255, 255, 255), 1, cv::LINE_AA);
	ellipse(image, cv::Point(511, 521), cv::Size(513, 513), 0, -26, -154, cv::Scalar(255, 255, 255), 1, cv::LINE_AA);

	putText(image, "60m", cv::Point(12, 315), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);
	putText(image, "60m", cv::Point(975, 315), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);

	putText(image, " 48m", cv::Point(105, 360), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);
	putText(image, " 48m", cv::Point(875, 360), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);

	putText(image, " 36m", cv::Point(199, 408), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);
	putText(image, " 36m", cv::Point(776, 408), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);

	putText(image, " 24m", cv::Point(288, 450), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);
	putText(image, " 24m", cv::Point(682, 450), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);

	putText(image, "12m", cv::Point(388, 492), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);
	putText(image, "12m", cv::Point(596, 492), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);
}

cv::Mat udp_receive::ribbon(cv::Mat image) {
    cv::Mat jet_map = cv::Mat::zeros(256, 1, CV_8UC3);
    for (int i = 0; i < 256; i++)
    {
        jet_map.at<cv::Vec3b>(i, 0)[0] = ribbon_C[i][0];
        jet_map.at<cv::Vec3b>(i, 0)[1] = ribbon_C[i][1];
        jet_map.at<cv::Vec3b>(i, 0)[2] = ribbon_C[i][2];
    }
    cv::Mat color_image;
    LUT(image, jet_map, color_image);
    return color_image;
}

void udp_receive::obj() {
    char buffer[65000];
    sockaddr_in clientAddr;
    int clientAddrSize = sizeof(clientAddr);
    int bytesReceived = recvfrom(udpSocket, buffer, sizeof(buffer), 0, (sockaddr*)&clientAddr, &clientAddrSize);
    std::string processDataidentifier((char *)buffer, 4);
    std::cout << processDataidentifier << std::endl;
    if (processDataidentifier != "$FSP")
    {
        std::cerr << "message is not $FSP" << std::endl;
    }
    int uuv_id;
    float uuv_x,uuv_y,uuv_z;
    memcpy(&uuv_id, buffer + 4, sizeof(int));
    memcpy(&uuv_x, buffer + 8, sizeof(float));
    memcpy(&uuv_y, buffer + 12, sizeof(float));
    memcpy(&uuv_z, buffer + 16, sizeof(float));
    std::cout << "uuv_id = " << uuv_id << std::endl;
    float uuv_rotationx, uuv_rotationy, uuv_rotationz;

    memcpy(&uuv_rotationx, buffer + 20, sizeof(float));
    memcpy(&uuv_rotationy, buffer + 24, sizeof(float));
    memcpy(&uuv_rotationz, buffer + 28, sizeof(float));

    int point_num;
    memcpy(&point_num, buffer+32, sizeof(int));
    std::cout << "point_num = " << point_num << std::endl;
    vector<Point_Pos::PointXYZ> PointXYZ_V;
    vector<Point_Pos::PointXYZA> v2;
    for (int i = 0; i < point_num; i++)
    {
        Point_Pos::PointXYZ temp;
        float temp_x;
        float temp_y;
        float temp_z;
        memcpy(&temp_x, buffer + 36 + i * 12, sizeof(float));
        memcpy(&temp_y, buffer + 40 + i * 12, sizeof(float));
        memcpy(&temp_z, buffer + 44 + i * 12, sizeof(float));
        temp.x = temp_x;
        temp.y = temp_y;
        temp.z = temp_z;
        PointXYZ_V.push_back(temp);
    }
    int imgSize = 0;
    memcpy(&imgSize, buffer +36 + 12 * point_num, sizeof(int));
    std::cout << "imgSize = " << imgSize << std::endl;
    std::vector<u_char> imageData(buffer + 36 + 12 * point_num + 4, buffer + 36 + 12 * point_num + 4 + imgSize);
    cv::Mat receivedImage = cv::imdecode(imageData, cv::IMREAD_COLOR);
    // cv::imshow("receivedImage", receivedImage);
	// cv::waitKey(1);
    //cv::imwrite("test270.png",receivedImage);
    cudaSetDevice(0);
    cv::Mat res_seg;
    cv::Size size = cv::Size{ 640, 640 };
    int topk = 100;
    int seg_h = 160;
    int seg_w = 160;
    int seg_channels = 32;
    float score_thres = 0.15f;
    float iou_thres = 0.35f;

    std::vector<seg::Object> seg_objs;

    seg_objs.clear();
    ms_yolov8Ptr->copy_from_Mat(receivedImage, size);
    ms_yolov8Ptr->infer();
    ms_yolov8Ptr->postprocess(seg_objs, score_thres, iou_thres, topk, seg_channels, seg_h, seg_w);
    int height = receivedImage.rows;
    int width = receivedImage.cols;
    cv::Mat black_image = cv::Mat::zeros(height, width, receivedImage.type());
    ms_yolov8Ptr->draw_objects(black_image, res_seg, seg_objs, CLASS_NAMES, COLORS, MASK_COLORS);
    cv::add(res_seg, receivedImage, res_seg);
    resize(res_seg, res_seg, cv::Size(1024,512));
    cv::cvtColor(res_seg, res_seg, cv::COLOR_BGR2GRAY);
    cv::Mat out_image(512, 512, CV_8UC1, cv::Scalar(0, 0, 0));
    cv::flip(res_seg, res_seg, 0);
    m_Coordinate_transformation.preprocess(res_seg, out_image);
    resize(out_image, out_image, cv::Size(256, 256));
    cv::Mat outputimage_;
	cyclerPtr->infer(out_image, outputimage_);
    cv::resize(outputimage_, outputimage_, cv::Size(512, 512));
    cv::Mat xyimage2(512, 1024, CV_8UC1, cv::Scalar(0));

    m_Coordinate_transformation.polar2xy(xyimage2, outputimage_);
    
    cv::Mat result;
    // cv::imshow("out_resize_image326", xyimage2);
	// cv::waitKey(1);

    cv::multiply(xyimage2, image_test, result);
    // cv::imshow("out_resize_image331", result);
	// cv::waitKey(1);
    // std::time_t t = std::time(0);
    // std::tm* now = std::localtime(&t);
    // char filename[100];
    // std::strftime(filename, 100, "%Y%m%d%H%M%S.png", now);
    // cv::imwrite(filename, result);

    adjustBrightnessContrast(result, 55, 120);
    cv::flip(result, result,-1);
    cv::flip(result, result, 1);
    std::vector<cv::Mat>src;
    src.emplace_back(result);
    src.emplace_back(result);
    src.emplace_back(result);
    static cv::Mat dst;
    cv::merge(src, dst);
    cv::Mat result_0, out_resize_image;
    result_0 = ribbon(dst);


    cv::Mat image_temp = cv::Mat::zeros(cv::Size(1024, 10), CV_8UC3);
    cv::vconcat(image_temp, result_0, out_resize_image);
    cv::vconcat(out_resize_image, image_temp, out_resize_image);

    // cv::imshow("out_resize_image", out_resize_image);
	// cv::waitKey(1);
    // std::time_t t = std::time(0);
    // std::tm* now = std::localtime(&t);
    // char filename[100];
    // std::strftime(filename, 100, "%Y%m%d%H%M%S.png", now);
    // cv::imwrite(filename, res_seg);

    ///////yolov8
    cv::Mat res, image;
    std::vector<det::Object> objs;
    objs.clear();
    msg_fsonar_obj.vec.clear();
    image = out_resize_image;
    yolov8Ptr->copy_from_Mat(image, size);
    yolov8Ptr->infer();
    yolov8Ptr->postprocess(objs);
    msg_fsonar_obj.uuv_id = uuv_id;

    float threshold = 0.68;
    objs.erase(std::remove_if(objs.begin(), objs.end(), [threshold](const det::Object& obj) {
        return obj.prob < threshold;
    }), objs.end());

    yolov8Ptr->draw_objects(image, res, objs, CLASS_NAMES, COLORS);
    draw_scale(out_resize_image);
    cv::imshow("Multi-beam-sonar", out_resize_image);
    cv::waitKey(1);
    // std::time_t t = std::time(0);
    // std::tm* now = std::localtime(&t);

    std::string log_file;
    if (log_bool == false){
        log_file = CommonGetExePath() + "log/app_FsonarFile/FsonarFile.log";
        HSPD::GetSingleton()->InitLog(log_file);
        HSPD::GetSingleton()->HandleLog(5);
        log_bool = true;
    }

    v2 = m_calucate_sonar_distance.filterVector(uuv_x, uuv_y, uuv_z, uuv_rotationy, 0, PointXYZ_V, v2);
    for (auto& v22:v2)
    {
        HMSG::msg_fsonar::Obj tempStruct = { 0,v22.x,v22.y,v22.z};
        HINFO("obj.size:{:d}, temp_XYZA.x:{:f}, temp_XYZA.y:{:f}, temp_XYZA.z:{:f}", v2.size(), v22.x, v22.y,v22.z);
        msg_fsonar_obj.vec.push_back(tempStruct);
    } 

    PointXYZ_V.clear();
    v2.clear();
    //std::cout << "msg_fsonar_obj_size = " << msg_fsonar_obj.vec.size() << std::endl;
    //yolov8->draw_objects(image, res, objs, CLASS_NAMES, COLORS);


    if (bytesReceived == SOCKET_ERROR) {
        std::cerr << "recvfrom failed." << std::endl;
    }
}



udp_receive::~udp_receive()
{
    closesocket(this->udpSocket);
    WSACleanup();
}
#pragma once
#include "HMSG.h"
#include "HUtils/HZeromq.h"
#include "detect.h"
#include "calucate_sonar_distance.h"
#include "Coordinate_transformation.h"
#include "yolov8-seg.h"
#include "cycle.h"
#ifdef _WIN32
#include <winsock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#elif __linux__
#include <unistd.h> 
#include <arpa/inet.h>
#endif
class udp_receive
{
public:
	udp_receive();
	void obj();
	void adjustBrightnessContrast(cv::Mat& image, int brightness, int contrast);
	void draw_scale(cv::Mat& image);
	cv::Mat ribbon(cv::Mat image);
	~udp_receive();
public:
	HMSG::msg_fsonar::Fsonar_Obj msg_fsonar_obj;
	// HMSG::msg_fsonar::Obj m_obj;
	//bool udp_flag;
	SOCKET udpSocket;
	YOLOv8* yolov8Ptr;
    YOLOv8_seg* ms_yolov8Ptr;
	cycle* cyclerPtr;
	Coordinate_transformation m_Coordinate_transformation;
	//cycle cycler("c.trt");
	calucate_sonar_distance m_calucate_sonar_distance;
	bool log_bool = false;
	cv::Mat image_test;
private: 

};


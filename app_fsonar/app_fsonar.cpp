#include "HUtils/HZeromq.h"
#include "HMSG.h"
#include <string>
#include <iostream>
#include <cmath>
#include "chrono"
#include "detect.h"
#include "udp_receive.h"
#ifdef _WIN32
#include <winsock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#elif __linux__
#include <unistd.h> // Linux 头文件
#include <arpa/inet.h>
#endif



typedef struct {
	HZMQ::HZmqPub* zmqPubA;
	HZMQ::HZmqPub* zmqPubC;
}app_data_t;
app_data_t app_data;

udp_receive m_udp_receive;



static void sig_int_handler(int signo) {
	HZMQ::Quit();
}

 
void callback_loop(void* userData){
	app_data_t* app_data_p = (app_data_t*)userData;
	m_udp_receive.obj();
	app_data_p->zmqPubA->Publish(m_udp_receive.msg_fsonar_obj);
	
	
}




int main(int argc, char *argv[]) {

    signal(SIGINT, sig_int_handler);
	HZMQ::Init();  
	app_data.zmqPubA =  HZMQ::CreatePublish("Fsonar_Obj");

    printf("Spin start\n");
    HZMQ::Spin();
    printf("Loop start\n");
    HZMQ::Loop(10,callback_loop,&app_data);
    printf("Loop end\n");
	return 0;
}
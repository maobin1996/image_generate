#include "calucate_sonar_distance.h"
#include <iostream>
#include <fstream>
#define LOG_FILE "calucate_sonar_distance.txt"
#include <string>
std::vector<Point_Pos::PointXYZA> calucate_sonar_distance::filterVector(float uuv_x, float uuv_y, float uuv_z,float rotation_y,float detect_angle,vector<Point_Pos::PointXYZ> &v1, vector<Point_Pos::PointXYZA> &v2)
{
    int minIndex = - 1;
    Point_Pos::PointXYZA temp_XYZA;
    std::ofstream logFile(LOG_FILE, std::ios::app);
    if (!logFile.is_open()) {
       std::cerr << "Unable to open log file" << std::endl;
    }
    for (auto &v11:v1)
    {
        float tempx = v11.x;
        float tempy = v11.y;
        float tempz = v11.z;
        float distance = calucate_distance(uuv_x, uuv_y, uuv_z, tempx, tempy, tempz);
        float x_max_point = uuv_x + 50 ;//(uuv_x + 50,uuv_z + 25)
        float x_min_point = uuv_x - 50 ;
        //float z_max_point = uuv_z + 25 ;
        // x,z 旋转
        float theta = -rotation_y;
        double x_new = (tempx - uuv_x) * cos(theta) - (tempz - uuv_z) * sin(theta) + uuv_x;
        double z_new = (tempz - uuv_z) * sin(theta) + (tempz - uuv_z) * cos(theta) + uuv_z;
        
        //double angle_c = std::atan2(opposite, adjacent) * 180.0 / M_PI;
        float vision_theta = std::atan2(abs(z_new - uuv_z),abs(x_new - uuv_x))* 180.0 / PI;
        // float angle_h =  calucate_angle_x(uuv_x,uuv_y, rotation_y, tempx, tempy);
        // float angle_g =  calucate_angle_z(uuv_x, uuv_y, uuv_z, tempx, tempy, tempz);
       // cout << "distance :" << distance << "vision_theta :" << vision_theta << "z_new :" << z_new <<"uuv_z" <<uuv_z<<std::endl;
        if (distance<=60 && vision_theta>25 && z_new>uuv_z )
        { 
            // logFile << " uuv_x: " << uuv_x << " uuv_y: " << uuv_y << " uuv_z: " << uuv_z <<" rotation_y: "<<rotation_y<< std::endl;
            // logFile << " tempx: " << tempx << " tempy: " << tempy << " tempz: " << tempz << std::endl;
            Point_Pos::PointXYZA temp_XYZA;
            temp_XYZA.x = tempx;
            temp_XYZA.y = tempy;
            temp_XYZA.z = tempz;
            v2.push_back(temp_XYZA);
        //     temp_XYZA.angle = angle_h;
            //logFile << " distance :" << distance << " angle_h :" << angle_h << " angle_g :" << angle_g << std::endl;
        }

        // logFile << " uuv_x :" << uuv_x << " uuv_y :" << uuv_y << " uuv_z :" << uuv_z <<" rotation_y :"<<rotation_y<< std::endl;
        // logFile << " tempx :" << tempx << " tempy :" << tempy << " tempz :" << tempz << std::endl;
        // logFile << " distance :" << distance << " angle_h :" << angle_h << " angle_g :" << angle_g << std::endl;
        // if (distance<=60 && abs(angle_h)<=60 && abs(angle_g)<=10)
        // {
        //     //Point_Pos::PointXYZA temp_XYZA;
        //     temp_XYZA.x = tempx;
        //     temp_XYZA.y = tempy;
        //     temp_XYZA.z = tempz;
        //     temp_XYZA.angle = angle_h;
        //     cout << "tempx :" << tempx << "tempy :" << tempy << "tempz :" << tempz << "angle :"<<angle_h<<endl;
        //     v2.push_back(temp_XYZA);
        // }
    }

    // for (auto &v22:v2)
    // {
    //     float min_angle,temp_angle;
    //     vector<float> temp_angle_v;
    //     temp_angle = abs(detect_angle - v22.angle);
    //     temp_angle_v.push_back(temp_angle);
    //     auto minIter = std::min_element(temp_angle_v.begin(), temp_angle_v.end());
    //     minIndex = std::distance(temp_angle_v.begin(), minIter);
    // }
    // if (minIndex != -1)
    // {
    //     std::cout << "minIndex =" << minIndex << endl;
    //     return v2.at(minIndex);
    // }
    // else
    // {
    //     return { 0,0,0,0 };
    // }
    return v2;
}

float calucate_sonar_distance::calucate_angle_x(float x1, float z1, float theta, float x2, float z2)
{

    float theta_rad = theta * PI / 180.0;

    float delta_x = x2 - x1;
    float delta_z = z2 - z1;

    float theta_b_rad = atan2(delta_x, delta_z);

    float angle_rad = theta_b_rad - theta_rad;
 
    float angle_deg = angle_rad * 180.0 / PI;

    return angle_deg;
}

float calucate_sonar_distance::calucate_distance(float x1, float y1, float z1, float x2, float y2, float z2)
{
    return  sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1) + (z2 - z1) * (z2 - z1));
}

float calucate_sonar_distance::calucate_angle_z(float x1, float y1, float z1, float x2, float y2, float z2)
{
    float height = y2 - y1;
    float distance = sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1) + (z2 - z1) * (z2 - z1));
    float angle_rad = std::asin(height/distance);
    float angle_deg = angle_rad * 180.0 / PI;
    return angle_deg;
}

#pragma once
#include <iostream>
#include <cmath>
#include <vector>
using namespace std;

const double PI = 3.14159265358979323846;


namespace Point_Pos
{
    struct PointXYZ
    {
        float x;
        float y;
        float z;
    };
    struct PointXYZA
    {
        float x;
        float y;
        float z;
        float angle;
    };
}
class calucate_sonar_distance
{
public:
    std::vector<Point_Pos::PointXYZA>  filterVector(float uuv_x, float uuv_y, float uuv_z, float rotation_y, float detect_angle,vector<Point_Pos::PointXYZ> &v1, vector<Point_Pos::PointXYZA> &v2);
    float calucate_angle_x(float x1, float y1, float theta, float x2, float y2);
    float calucate_distance(float x1, float y1, float z1, float x2, float y2, float z2);
    float calucate_angle_z(float x1, float y1, float z1, float x2, float y2, float z2);
};
#pragma once
#include <opencv2/opencv.hpp>
#include <string>
#include <iostream>
#include <math.h>
#include <chrono>
#include <time.h>
#include <cmath>
#include <tuple>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;
using namespace std::chrono;

class Coordinate_transformation
{
public:
	Coordinate_transformation();
	void xy2polar(cv::Mat& xyimage, cv::Mat& polar_image);
	void polar2xy(cv::Mat& xyimage, cv::Mat& polar_image);
	void preprocess(cv::Mat& input_image, cv::Mat& output_image);
public:
    std::vector<std::vector<int>> groups;
	std::vector<std::vector<int>> xy2polar_groups;
};



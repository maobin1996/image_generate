#include "stitch_Images.h"

using namespace cv;  
using namespace std;  
namespace fs = std::filesystem;


stitch_Images::stitch_Images()
{
    

}

int stitch_Images::getMax(Mat src) 
{
	int row = src.rows;
	int col = src.cols;
	int temp = 0;
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			temp = max((int)src.at<uchar>(i, j), temp);
		}
		if (temp == 255) return temp;
	}
	return temp;
}

Mat stitch_Images::dehaze(Mat src) {
	double eps;
	int row = src.rows;
	int col = src.cols;
	Mat M = Mat::zeros(row, col, CV_8UC1);
	Mat M_max = Mat::zeros(row, col, CV_8UC1);
	Mat M_ave = Mat::zeros(row, col, CV_8UC1);
	Mat L = Mat::zeros(row, col, CV_8UC1);
	Mat dst = Mat::zeros(row, col, CV_8UC3);
	double m_av, A;
	//get M
	double sum = 0;
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			uchar r, g, b, temp1, temp2;
			b = src.at<Vec3b>(i, j)[0];
			g = src.at<Vec3b>(i, j)[1];
			r = src.at<Vec3b>(i, j)[2];
			temp1 = min(min(r, g), b);
			temp2 = max(max(r, g), b);
			M.at<uchar>(i, j) = temp1;
			M_max.at<uchar>(i, j) = temp2;
			sum += temp1;
		}
	}
	m_av = sum / (row * col * 255);
	eps = 0.85 / m_av;
	boxFilter(M, M_ave, CV_8UC1, Size(51, 51));
	double delta = min(0.9, eps*m_av);
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			L.at<uchar>(i, j) = min((int)(delta * M_ave.at<uchar>(i, j)), (int)M.at<uchar>(i, j));
		}
	}
	A = (getMax(M_max) + getMax(M_ave)) * 0.5;
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			int temp = L.at<uchar>(i, j);
			for (int k = 0; k < 3; k++) {
				int val = A * (src.at<Vec3b>(i, j)[k] - temp) / (A - temp);
				if (val > 255) val = 255;
				if (val < 0) val = 0;
				dst.at<Vec3b>(i, j)[k] = val;
			}
		}
	}
	return dst;
}


cv::Mat stitch_Images::preprocess(cv::Mat image,int imagetype)
{
	if (imagetype ==0)
	{
		cv::resize(image,image,cv::Size(1920,1080));
		int left = 0;  
		int upper = 90;  
		int right = 1920;
		int lower = 800; 
		if (right > image.cols || lower > image.rows) {  
			std::cerr << "Crop dimensions exceed image size!" << std::endl;  
		}  
		cv::Rect cropRegion(left, upper, right - left, lower - upper); // Rect(x, y, width, height)  
		cv::Mat croppedImage = image(cropRegion);  
        cv::Mat dst = dehaze(croppedImage);
		return dst;
	}

	else if (imagetype ==1)
	{
		if (image.empty())
		{  
           std::cerr << "Error loading image!" << std::endl;  
        }  
		// Number of columns to remove from left and right  
		const int colsToRemove = 50;  
		// Ensure the image is wide enough  
		if (image.cols <= 2 * colsToRemove) {  
			std::cerr << "Image is too narrow to remove columns!" << std::endl;   
		}  
		cv::Rect cropRegion(colsToRemove, 0, image.cols - 2 * colsToRemove, image.rows);  
		cv::Mat croppedImage = image(cropRegion);  
        return croppedImage;
	}
	else
	{
		return image;
	}  

}

    // 设置回调函数
void stitch_Images::setProgressStringCallback(ProgressStringCallback callback) 
{
    m_progressStringCallback = callback;
}

std::vector<cv::Mat>stitch_Images::getImagesInDirectory(std::vector<std::string> folderPath,int imagetype) 
{  
    std::vector<cv::Mat> images;
	for (size_t i = 0; i < folderPath.size(); i++)
	{
		std::string images_string = folderPath.at(i);
		cv::Mat img = cv::imread(images_string);
		img = preprocess(img,imagetype);
		if (!img.empty()) {  
                images.push_back(img); // 和成功读取的图像  
        } 
		else {  
                std::cout << "Failed to read image: " << images_string << std::endl;  
        }  
	}

    return images;  
}  


// 尝试接拼图像  
std::string stitch_Images::stitchImages(std::vector<std::string> folderPath,std::vector<std::string>& failedimagefloder,int imagetype)
{  

    std::vector<cv::Mat> images = getImagesInDirectory(folderPath,imagetype);  
    cv::Mat pano;  
    cv::Stitcher::Mode mode = cv::Stitcher::SCANS;
    cv::Ptr<cv::Stitcher> stitcher = cv::Stitcher::create(mode); 
    std::string failedImage; // 用于存储最后一张接拼失败的图像  
    int i =images.size();

	if (m_progressStringCallback) {
		m_progressStringCallback("开始接拼");
	}
    // 从所有图像开始接拼  
    while (!images.empty()) {  

		if (m_progressStringCallback)
		{   
			string test_str;
            // 拼接所有字符串，中间加空格
			for (size_t i = 0; i < folderPath.size(); ++i) {

				std::filesystem::path absolute_path(folderPath[i]);
                std::string file_name = absolute_path.filename().string();  // 获取文件名
				test_str += file_name;  // 添加当前字符串
				if (i < folderPath.size() - 1) {
					test_str += ",";  // 在字符串之间添加空格
				}
			}
			//test_str = test_str + std::to_string(images.size());
			//m_progressStringCallback(test_str +" 为一组图片接拼,共" + std::to_string(images.size())+"张。");
			cv::Stitcher::Status status = stitcher->stitch(images, pano);    
			if (status == cv::Stitcher::OK) {  
				//m_progressStringCallback("以" + std::to_string(images.size())+"图像");
				// 接拼成功  
				//std::cout << "Stitching successful!" << std::endl;  
				std::filesystem::path currentPath = std::filesystem::current_path();
				std::string imageName = "pano.jpg";
				std::filesystem::path fullPath = currentPath / imageName;
				cv::imwrite(fullPath.string(),pano);
				m_progressStringCallback(test_str +"，图片组接拼(" + std::to_string(images.size())+")张，接拼成功!");
				//m_progressStringCallback("接拼成功！");
				return fullPath.string(); // 成功时返回空字符串  
			} 
			m_progressStringCallback(test_str +"，图片组接拼(" + std::to_string(images.size())+")张，接拼失败!");
			//m_progressStringCallback("接拼失败!");
			//std::cout<<imagestring.at(images.size() - 1)<<std::endl;
			// m_progressStringCallback("Stitching failed!");
			// 接拼失败，记录失败的图像  
			//std::cout<<imagestring.at(images.size() - 1)<<std::endl;
			failedImage = folderPath.at(images.size() - 1);

			failedimagefloder.push_back(failedImage);
			//m_progressStringCallback(failedImage);
			//std::cout << "Stitching failed, removing last image." << std::endl; 
			images.pop_back();
			folderPath.pop_back();
			if (images.size() ==1)
			{
				return "";
			}
			//m_progressStringCallback("Stitching failed, removing " + std::to_string(images.size()) + "image");
		}


		
    }  

	// if (m_progressStringCallback) {
    //     m_progressStringCallback("Work complete!");
    // }


	
    //return failedImage; // 返回最后一张接拼失败的图像索引  
}   

stitch_Images::~stitch_Images()
{
}

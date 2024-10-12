#ifndef STITCH_IMAGES
#define STITCH_IMAGES

#ifdef STITCH_IMAGES_EXPORTS
#define STITCH_IMAGES_API __declspec(dllexport)
#else
#define STITCH_IMAGES_API __declspec(dllimport)
#endif

#include <opencv2/opencv.hpp>  
#include <opencv2/stitching.hpp>  
#include <iostream>  
#include <vector>  
#include <filesystem>  
#include <functional>
using namespace cv;
using namespace std;
namespace fs = std::filesystem;  

class STITCH_IMAGES_API stitch_Images
{
private:
public:
    stitch_Images();
    using ProgressStringCallback = std::function<void(const std::string&)>;
    int getMax(cv::Mat src);
    cv::Mat dehaze(Mat src);
    cv::Mat preprocess(cv::Mat image,int imagetype);
    void setProgressStringCallback(ProgressStringCallback callback);
    std::vector<cv::Mat>getImagesInDirectory(std::vector<std::string> folderPath,int imagetype);
    std::string stitchImages(std::vector<std::string> folderPath,std::vector<std::string>& failedimagefloder,int imagetype);
    ~stitch_Images();
public:
    std::vector<std::string> imagestring;
private:
    ProgressStringCallback m_progressStringCallback;
};

#endif // MYCLASS_H
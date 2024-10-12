#include <opencv2/opencv.hpp>  
#include <opencv2/stitching.hpp>  
#include <iostream>  
#include <vector>  
#include <filesystem>  
#include <functional>
using namespace cv;
using namespace std;
namespace fs = std::filesystem;  

class stitch_Images
{
private:
public:
    stitch_Images();
    using ProgressStringCallback = std::function<void(const std::string&)>;
    int getMax(cv::Mat src);
    cv::Mat dehaze(Mat src);
    cv::Mat preprocess(cv::Mat image,int imagetype);
    void setProgressStringCallback(ProgressStringCallback callback);
    std::vector<cv::Mat>getImagesInDirectory(const std::string& folderPath,int imagetype);
    bool stitchImages(const std::string& folderPath,std::vector<std::string>& failedimagefloder,int imagetype);
    ~stitch_Images();
public:
    std::vector<std::string> imagestring;
private:
    ProgressStringCallback m_progressStringCallback;
};
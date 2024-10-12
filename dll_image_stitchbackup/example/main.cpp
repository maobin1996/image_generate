#include <iostream>
#include "stitch_Images.h"  // 包含头文件
#include <string>
#include <vector>
int main()
{
    // stitch_Images stitcher;  // 创建类的实例
    // stitcher
    stitch_Images m_stitch_Images;
    //std::string folderPath = "C:\\Users\\mao\\Desktop\\bjxn\\test02\\feature_matching\\cpp\\test02\\testimage\\";
    m_stitch_Images.setProgressStringCallback([](const std::string &msg)
    {
        std::cout << msg << std::endl;
    });
    std::vector<std::string> testfloder;
    std::vector<std::string> folderPath;
    std::string image_path1 = "C:\\Users\\mao\\Desktop\\bjxn\\test02\\feature_matching\\cpp\\test02\\4\\0100.jpg";
    //std::string image_path2 = "C:\\Users\\mao\\Desktop\\bjxn\\test02\\feature_matching\\cpp\\test02\\4\\0101.jpg";
    // std::string image_path2 = "C:\\Users\\mao\\Desktop\\bjxn\\test02\\feature_matching\\cpp\\test02\\4\\0140.jpg";
    std::string image_path3 = "C:\\Users\\mao\\Desktop\\bjxn\\test02\\feature_matching\\cpp\\test02\\3\\0010.jpg";
    std::string image_path4 = "C:\\Users\\mao\\Desktop\\bjxn\\test02\\feature_matching\\cpp\\test02\\2\\0014.jpg";
    folderPath.push_back(image_path1);
    //folderPath.push_back(image_path2);
    folderPath.push_back(image_path3);
    folderPath.push_back(image_path4);
    std::string image_name = m_stitch_Images.stitchImages(folderPath,testfloder,2);
    // 使用类的方法
    std::cout << "Stitching images..." << image_name<<std::endl;
    return 0;
}
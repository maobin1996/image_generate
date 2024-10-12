#include <iostream>
#include "stitch_Images.h"  // 包含头文件
#include <string>
int main()
{
    // stitch_Images stitcher;  // 创建类的实例
    // stitcher
    stitch_Images m_stitch_Images;
    std::string folderPath = "C:\\Users\\mao\\Desktop\\bjxn\\test02\\feature_matching\\cpp\\test02\\testimage\\";
    m_stitch_Images.setProgressStringCallback([](const std::string &msg)
    {
        std::cout << msg << std::endl;
    });
    std::vector<std::string> testfloder;
    bool flag =m_stitch_Images.stitchImages(folderPath,testfloder,2);
    // 使用类的方法
    std::cout << "Stitching images..." << std::endl;
    return 0;
}

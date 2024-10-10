#include "Coordinate_transformation.h"

Coordinate_transformation::Coordinate_transformation() {
    std::ifstream file("output.txt");
    std::string line;
    std::vector<int> tokens;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string token;
        while (std::getline(iss, token, ' ')) {
            int num = std::stoi(token);
            tokens.push_back(num);
        }
    }

    // 现在tokens向量中包含了一行中的所有单词
// 将数值4个为一组插入到vector中
    
    for (int i = 0; i < tokens.size(); i += 4) {
        std::vector<int> group;
        for (int j = i; j < i + 4 && j < tokens.size(); j++) {
            group.push_back(tokens[j]);
        }
        groups.push_back(group);
    }
}



void Coordinate_transformation::xy2polar(cv::Mat& xyimage, cv::Mat& polar_image) {
    if (xyimage.channels()==1)
    {
        for (const auto& group : groups) {
            int xyimage_height_index = group.at(0);
            int xyimage_width_index = group.at(1);
            int image_height_index = group.at(2);
            int image_width_index = group.at(3);
            polar_image.at<uchar>(image_height_index, image_width_index) = xyimage.at<uchar>(xyimage_height_index, xyimage_width_index);
        }
    }

    else
    {
        for (const auto& group : groups) {
            int xyimage_height_index = group.at(0);
            int xyimage_width_index = group.at(1);
            int image_height_index = group.at(2);
            int image_width_index = group.at(3);
            polar_image.at<cv::Vec3b>(image_height_index, image_width_index) = xyimage.at<cv::Vec3b>(xyimage_height_index, xyimage_width_index);
        }
    }

}

void Coordinate_transformation::polar2xy(cv::Mat& xyimage, cv::Mat& polar_image)
{
    if (xyimage.channels()==1)
    {
        for (const auto& group : groups) {
            int xyimage_height_index = group.at(0);
            int xyimage_width_index = group.at(1);
            int image_height_index = group.at(2);
            int image_width_index = group.at(3);
            //  ellipse_image.at<uchar>(ellipse_image_height_idex, ellipse_image_width_idex) = xyimage.at<uchar>(image_height_idex, image_width_idex);
            xyimage.at<uchar>(xyimage_height_index, xyimage_width_index) = polar_image.at<uchar>(image_height_index, image_width_index);
        }
    }

    else
    {
        for (const auto& group : groups) {
            int xyimage_height_index = group.at(0);
            int xyimage_width_index = group.at(1);
            int image_height_index = group.at(2);
            int image_width_index = group.at(3);
            //  ellipse_image.at<uchar>(ellipse_image_height_idex, ellipse_image_width_idex) = xyimage.at<uchar>(image_height_idex, image_width_idex);
            polar_image.at<cv::Vec3b>(image_height_index, image_width_index) = xyimage.at<cv::Vec3b>(xyimage_height_index, xyimage_width_index);;
        }
    }

}

void Coordinate_transformation::preprocess(cv::Mat &input_image, cv::Mat &output_image)
{
    if (output_image.channels()==1)
    {
        // 生成噪声图像
        cv::Mat image_sequence(512, 512, CV_8UC1, cv::Scalar(0));
        // cv::randu(image_sequence, cv::Scalar(0), cv::Scalar(0));
        std::vector<Mat>mv;
        split(input_image, mv);
        Mat unity_image = input_image;
        //  std::cout << roi.cols << roi.rows << std::endl;
        // 生成噪声图像
        cv::Mat noise(512, 512, CV_8UC1);
        cv::randu(noise, cv::Scalar(3), cv::Scalar(20));
        noise += image_sequence * 0.8;
        // 将噪声图像添加到序列中
        noise.copyTo(image_sequence);
        xy2polar(unity_image, output_image);
        for (int y = 0; y < output_image.rows; y++) {
            for (int x = 0; x < output_image.cols; x++) {
                if (output_image.at<uchar>(y, x) > 180) {
                    output_image.at<uchar>(y, x) -= 30;
                }
            }
        }

        cv::addWeighted(output_image, 0.3, noise, 0.7, 1, output_image);

    }
    else
    {
        cv::Mat image_sequence(512, 1024, CV_8UC3, cv::Scalar(0,0,0));
        // cv::randu(image_sequence, cv::Scalar(0), cv::Scalar(0));
        Mat unity_image = input_image;
        //  std::cout << roi.cols << roi.rows << std::endl;
        // 生成噪声图像
        cv::Mat noise(512, 1024, CV_8UC3);
        cv::randu(noise, cv::Scalar(3, 3, 3), cv::Scalar(255, 255, 255));


        noise += image_sequence * 0.8;
        // 将噪声图像添加到序列中
        noise.copyTo(image_sequence);
        cv::addWeighted(unity_image, 0.9, noise, 0.1, 1, noise);


        cv::resize(noise, noise, cv::Size(1024, 512));
        xy2polar(noise, output_image);

        cv::imshow("noise", output_image);
        cv::waitKey(1);
    }

}
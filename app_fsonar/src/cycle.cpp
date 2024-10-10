#include "cycle.h"
using namespace cv;


//void draw_scale(cv::Mat& image) {
//    line(image, Point(511, 11), Point(50, 234), Scalar(255, 255, 255), 1, LINE_AA);
//    line(image, Point(511, 11), Point(973, 233), Scalar(255, 255, 255), 1, LINE_AA);
//    line(image, Point(511, 11), Point(512, 522), Scalar(255, 255, 255), 1, LINE_AA);
//    line(image, Point(511, 11), Point(256, 453), Scalar(255, 255, 255), 1, LINE_AA);
//    line(image, Point(511, 11), Point(768, 453), Scalar(255, 255, 255), 1, LINE_AA);
//
//    ellipse(image, Point(511, 11), Size(102, 102), 0, 26, 154, Scalar(255, 255, 255), 1, LINE_AA);
//    ellipse(image, Point(511, 11), Size(204, 204), 0, 26, 154, Scalar(255, 255, 255), 1, LINE_AA);
//    ellipse(image, Point(511, 11), Size(306, 306), 0, 26, 154, Scalar(255, 255, 255), 1, LINE_AA);
//    ellipse(image, Point(511, 9), Size(409, 409), 0, 26, 154, Scalar(255, 255, 255), 1, LINE_AA);
//    ellipse(image, Point(511, 9), Size(513, 513), 0, 26, 154, Scalar(255, 255, 255), 1, LINE_AA);
//
//    putText(image, "125m", Point(5, 230), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);
//    putText(image, "125m", Point(972, 230), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);
//
//    putText(image, "100m", Point(98, 185), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);
//    putText(image, "100m", Point(878, 185), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);
//
//    putText(image, "75m", Point(199, 140), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);
//    putText(image, "75m", Point(787, 139), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);
//
//    putText(image, "50m", Point(288, 96), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);
//    putText(image, "50m", Point(695, 97), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);
//
//    putText(image, "25m", Point(383, 52), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);
//    putText(image, "25m", Point(605, 52), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);
//}

void adjustBrightnessContrast(Mat& image, int brightness, int contrast) {
    // 调整亮度和对比度
    image.convertTo(image, -1, contrast / 100.0 + 1, brightness - 50);
}



cycle::cycle(const std::string& engine_file_path)
{

	std::ifstream file(engine_file_path, std::ios::binary);
	assert(file.good());
	file.seekg(0, std::ios::end);
	auto size = file.tellg();
	file.seekg(0, std::ios::beg);
	char* trtModelStream = new char[size];
	assert(trtModelStream);
	file.read(trtModelStream, size);
	file.close();
	initLibNvInferPlugins(&this->gLogger, "");
	this->runtime = nvinfer1::createInferRuntime(this->gLogger);
	assert(this->runtime != nullptr);

	this->engine = this->runtime->deserializeCudaEngine(trtModelStream, size);
	assert(this->engine != nullptr);
	delete[] trtModelStream;
	this->context = this->engine->createExecutionContext();

	assert(this->context != nullptr);
	cudaStreamCreate(&this->stream);

}

cycle::~cycle()
{
	this->context->destroy();
	this->engine->destroy();
	this->runtime->destroy();
	cudaStreamDestroy(this->stream);
}

void cycle::infer(const cv::Mat& image,cv::Mat &outputimage)
{
	const int batchSize = 1;
	const int inputSize = 256*256; // 输入张量的大小
	const int outputSize = 256*256; // 输出张量的大小
	void* deviceInput;void* deviceOutput;
	cudaMalloc(&deviceInput, batchSize * inputSize * sizeof(float));
	cudaMalloc(&deviceOutput, batchSize * outputSize * sizeof(float));
	std::vector<void*> bindings = {deviceInput,deviceOutput};

	cudaStream_t stream;
    cudaStreamCreate(&stream); //如果需要显式释放CUDA流，可以使用cudaStreamDestroy函数来释放已创建的CUDA流
   // 将输入数据复制到GPU内存中
    std::vector<float> input(batchSize * inputSize);
    // 将图片数据复制到输入张量
    cv::Mat resizedImage;
    cv::resize(image, resizedImage, cv::Size(256, 256));
    cv::Mat floatImage;
    resizedImage.convertTo(floatImage, CV_32F, 1.0 / 255.0);
    std::vector<cv::Mat> inputChannels;
    cv::split(floatImage, inputChannels);
    std::memcpy(input.data(), inputChannels[0].data, inputSize * sizeof(float));
    cudaMemcpyAsync(deviceInput, input.data(),batchSize * inputSize * sizeof(float), cudaMemcpyHostToDevice, stream);
    // 执行推理
    this->context->enqueue(batchSize, bindings.data(), stream, nullptr);
    // 将输出数据从GPU内存复制回主机内存
    std::vector<float> output(batchSize * outputSize);
    cudaMemcpyAsync(output.data(), deviceOutput,batchSize * outputSize * sizeof(float), cudaMemcpyDeviceToHost, stream);
    // 等待推理完成
    cudaStreamSynchronize(stream);

	// 处理输出结果
    // std::cout << output.data() << std::endl;
    for (float& value : output) {
        value *= -255;
        value = 1 - value;
    }

    std::vector<std::vector<int>> matrix(256, std::vector<int>(256));

    // 将一维向量转换为二维向量
    for (int i = 0; i < 256; i++) {
        for (int j = 0; j < 256; j++) {
            matrix[i][j] = output[i * 256 + j];
        }
    }
    // 创建Mat对象并将二维向量复制到该对象中
    cv::Mat img(256, 256, CV_8UC1);
    for (int i = 0; i < 256; i++) {
        for (int j = 0; j < 256; j++) {
            img.at<uchar>(i, j) = matrix[i][j];
        }
    }
    cudaFree(deviceInput);
    cudaFree(deviceOutput);
    cudaStreamDestroy(stream);
    outputimage = img;

}



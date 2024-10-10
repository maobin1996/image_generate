#ifndef   CYCLE_H
#define  CYCLE_H
#include <fstream>
#include "common.h"
#include "NvInferPlugin.h"

using namespace seg;
class    cycle
{
public:
	explicit cycle(const std::string& engine_file_path);
	~cycle();
    void infer(const cv::Mat& image,cv::Mat &outputimage);
	// int num_bindings;
	// int num_inputs = 0;
	// int num_outputs = 0;
	// std::vector<Binding> input_bindings;
	// std::vector<Binding> output_bindings;
	// std::vector<void*> host_ptrs;
	// std::vector<void*> device_ptrs;

private:
	nvinfer1::ICudaEngine* engine = nullptr;
	nvinfer1::IRuntime* runtime = nullptr;
	nvinfer1::IExecutionContext* context = nullptr;
	cudaStream_t stream = nullptr;
	Logger gLogger{ nvinfer1::ILogger::Severity::kERROR };

};

#endif //CYCLE_H


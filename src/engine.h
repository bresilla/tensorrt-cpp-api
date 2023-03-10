#pragma once

#include <opencv2/opencv.hpp>

#include "NvInfer.h"
#include "buffers.h"

// Precision used for GPU inference
enum class Precision {
    FP32,
    FP16
};

// Options for the network
struct Options {
    // Precision to use for GPU inference. 16 bit is faster but may reduce accuracy.
    Precision precision = Precision::FP16;
    // Batch sizes to optimize for.
    std::vector<int32_t> optBatchSizes {};
    // Maximum allowable batch size
    int32_t maxBatchSize = 16;
    // Max allowable GPU memory to be used for model conversion, in bytes.
    // Applications should allow the engine builder as much workspace as they can afford;
    // at runtime, the SDK allocates no more than this and typically less.
    size_t maxWorkspaceSize = 4000000000;
    // GPU device index
    int deviceIndex = 0;
};

// Class to extend TensorRT logger
class Logger : public nvinfer1::ILogger {
    void log (Severity severity, const char* msg) noexcept override;
};

class Engine {
public:
    Engine(const Options& options);
    ~Engine();
    // Build the network
    bool build(std::string onnxModelPath);
    // Load and prepare the network for inference
    bool loadNetwork();
    // Run inference.
    bool runInference(const std::vector<cv::Mat>& inputFaceChips, std::vector<std::vector<float>>& featureVectors);

    int32_t getInputHeight() const { return m_inputH; };
    int32_t getInputWidth() const { return m_inputW; };
private:

    // Converts the engine options into a string
    std::string serializeEngineOptions(const Options& options);

    void getDeviceNames(std::vector<std::string>& deviceNames);

    bool doesFileExist(const std::string& filepath);

    std::unique_ptr<nvinfer1::ICudaEngine> m_engine = nullptr;
    std::unique_ptr<nvinfer1::IExecutionContext> m_context = nullptr;
    const Options& m_options;
    Logger m_logger;
    samplesCommon::ManagedBuffer m_inputBuff;
    samplesCommon::ManagedBuffer m_outputBuff;
    size_t m_prevBatchSize = 0;
    std::string m_engineName;
    cudaStream_t m_cudaStream = nullptr;

    int32_t m_inputH = 0;
    int32_t m_inputW = 0;
};

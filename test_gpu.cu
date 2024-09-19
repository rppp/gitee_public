#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cuda_runtime.h>

#define CHECK_CUDA(call) \
    do { \
        cudaError_t err = call; \
        if (err != cudaSuccess) { \
            std::cerr << "CUDA error in " << __FILE__ << " line " << __LINE__ \
                      << ": " << cudaGetErrorString(err) << std::endl; \
            exit(EXIT_FAILURE); \
        } \
    } while(0)

const size_t MEMORY_SIZE = 20ULL * 1024 * 1024 * 1024;  // 20GB

void testMemoryCopy(int deviceId, void* h_memory) {
    cudaSetDevice(deviceId);
    
    void *d_memory;
    
    // 分配设备内存
    CHECK_CUDA(cudaMalloc(&d_memory, MEMORY_SIZE));
    
    clock_t start, end;
    double cpu_time_used;
    
    // 主机到设备的拷贝
    start = clock();
    CHECK_CUDA(cudaMemcpy(d_memory, h_memory, MEMORY_SIZE, cudaMemcpyHostToDevice));
    end = clock();
    
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    double speed_h2d = MEMORY_SIZE / cpu_time_used / (1024.0 * 1024 * 1024);
    
    // 设备到主机的拷贝
    start = clock();
    CHECK_CUDA(cudaMemcpy(h_memory, d_memory, MEMORY_SIZE, cudaMemcpyDeviceToHost));
    end = clock();
    
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    double speed_d2h = MEMORY_SIZE / cpu_time_used / (1024.0 * 1024 * 1024);
    
    std::cout << "Device " << deviceId << ":" << std::endl;
    std::cout << "  Host to Device: " << speed_h2d << " GB/s" << std::endl;
    std::cout << "  Device to Host: " << speed_d2h << " GB/s" << std::endl;
    
    // 释放设备内存
    CHECK_CUDA(cudaFree(d_memory));
}

int main() {
    int deviceCount;
    CHECK_CUDA(cudaGetDeviceCount(&deviceCount));
    
    std::cout << "Found " << deviceCount << " CUDA devices" << std::endl;

    // 只分配一次主机内存
    void *h_memory;
    CHECK_CUDA(cudaMallocHost(&h_memory, MEMORY_SIZE));
    
    for (int i = 0; i < deviceCount && i < 4; ++i) {
        testMemoryCopy(i, h_memory);
    }
    
    // 释放主机内存
    CHECK_CUDA(cudaFreeHost(h_memory));
    
    return 0;
}

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <vector>
#include <numeric> 

using namespace std;


const size_t MEMORY_SIZE = 20ULL * 1024 * 1024 * 1024;   // 20GB
const int64_t MEMORY_SIZE2 = 60ULL * 1024 * 1024 * 1024; // 20GB

int main()
{
    // 分配内存
    std::cout << "Allocating memory..." << std::endl;
    void *memory1 = std::malloc(MEMORY_SIZE);
    void *memory2 = std::malloc(MEMORY_SIZE);

    if (memory1 == nullptr || memory2 == nullptr)
    {
        std::cerr << "Failed to allocate memory" << std::endl;
        return 1;
    }

    std::cout << "Memory allocated. Starting copy tests..." << std::endl;

    // 第一次拷贝：memory1 到 memory2
    clock_t start1 = clock();
    std::memcpy(memory1, memory2, MEMORY_SIZE);
    clock_t end1 = clock();

    double time1 = ((double)(end1 - start1)) / CLOCKS_PER_SEC;
    double speed1 = MEMORY_SIZE / time1 / (1024.0 * 1024 * 1024);

    // 第二次拷贝：memory2 到 memory1
    clock_t start2 = clock();
    std::memcpy(memory2, memory1, MEMORY_SIZE);
    clock_t end2 = clock();

    double time2 = ((double)(end2 - start2)) / CLOCKS_PER_SEC;
    double speed2 = MEMORY_SIZE / time2 / (1024.0 * 1024 * 1024);

    // 输出结果
    std::cout << "Copy 1 (memory1 to memory2):" << std::endl;
    std::cout << "  Time: " << time1 << " seconds" << std::endl;
    std::cout << "  Speed: " << speed1 << " GB/s" << std::endl;

    std::cout << "Copy 2 (memory2 to memory1):" << std::endl;
    std::cout << "  Time: " << time2 << " seconds" << std::endl;
    std::cout << "  Speed: " << speed2 << " GB/s" << std::endl;

    // std::cout << "Average speed: " << (speed1 + speed2) / 2 << " GB/s" << std::endl;

    start1 = clock();
    std::memcpy(memory1, memory2, MEMORY_SIZE);
    end1 = clock();

    start2 = clock();
    std::memcpy(memory2, memory1, MEMORY_SIZE);
    end2 = clock();

    time1 = ((double)(end1 - start1)) / CLOCKS_PER_SEC;
    speed1 = MEMORY_SIZE / time1 / (1024.0 * 1024 * 1024);

    time2 = ((double)(end2 - start2)) / CLOCKS_PER_SEC;
    speed2 = MEMORY_SIZE / time2 / (1024.0 * 1024 * 1024);

    std::cout << "Copy 1 (memory1 to memory2):" << std::endl;
    std::cout << "  Time: " << time1 << " seconds" << std::endl;
    std::cout << "  Speed: " << speed1 << " GB/s" << std::endl;

    std::cout << "Copy 2 (memory2 to memory1):" << std::endl;
    std::cout << "  Time: " << time2 << " seconds" << std::endl;
    std::cout << "  Speed: " << speed2 << " GB/s" << std::endl;

    // 释放内存
    std::free(memory1);
    std::free(memory2);
    vector<double> numbers;
    char *mem = (char *)std::malloc(MEMORY_SIZE2);
    int buf_size = 4*1024*1024;
    //char buf[4096];
    char* buf = (char *)std::malloc(buf_size);
    for (int64_t i = 0; i < MEMORY_SIZE2 - buf_size; i += buf_size)
    {
        start1 = clock();
        memcpy(buf, mem + i, buf_size);
        end1 = clock();

        time1 = ((double)(end1 - start1)) / CLOCKS_PER_SEC;
        //std::cout <<  time1*1000 << std::endl;
        numbers.push_back(time1*1000);
        //speed1 = MEMORY_SIZE / time1 / ( 1024 * 1024);
    }
    std::free(buf);
    std::free(mem);

    auto max = *std::max_element(numbers.begin(), numbers.end());

    // 最小值
    auto min = *std::min_element(numbers.begin(), numbers.end());

    // 平均值
    double average = std::accumulate(numbers.begin(), numbers.end(), 0.0) / numbers.size();

    std::cout << "最大值: " << max << std::endl;
    std::cout << "最小值: " << min << std::endl;
    std::cout << "平均值: " << average << std::endl;

    return 0;
}

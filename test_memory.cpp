#include <iostream>
#include <cstring>
#include <cstdlib>
#include <ctime>

const size_t MEMORY_SIZE = 20ULL * 1024 * 1024 * 1024;  // 20GB

int main() {
    // 分配内存
    std::cout << "Allocating memory..." << std::endl;
    void* memory1 = std::malloc(MEMORY_SIZE);
    void* memory2 = std::malloc(MEMORY_SIZE);

    if (memory1 == nullptr || memory2 == nullptr) {
        std::cerr << "Failed to allocate memory" << std::endl;
        return 1;
    }

    std::cout << "Memory allocated. Starting copy tests..." << std::endl;

    // 第一次拷贝：memory1 到 memory2
    clock_t start1 = clock();
    std::memcpy(memory1, memory2, MEMORY_SIZE);
    clock_t end1 = clock();
    
    double time1 = ((double) (end1 - start1)) / CLOCKS_PER_SEC;
    double speed1 = MEMORY_SIZE / time1 / (1024.0 * 1024 * 1024);

    // 第二次拷贝：memory2 到 memory1
    clock_t start2 = clock();
    std::memcpy(memory1, memory1, MEMORY_SIZE);
    clock_t end2 = clock();
    
    double time2 = ((double) (end2 - start2)) / CLOCKS_PER_SEC;
    double speed2 = MEMORY_SIZE / time2 / (1024.0 * 1024 * 1024);

    // 输出结果
    std::cout << "Copy 1 (memory1 to memory2):" << std::endl;
    std::cout << "  Time: " << time1 << " seconds" << std::endl;
    std::cout << "  Speed: " << speed1 << " GB/s" << std::endl;

    std::cout << "Copy 2 (memory2 to memory1):" << std::endl;
    std::cout << "  Time: " << time2 << " seconds" << std::endl;
    std::cout << "  Speed: " << speed2 << " GB/s" << std::endl;

    std::cout << "Average speed: " << (speed1 + speed2) / 2 << " GB/s" << std::endl;

    // 释放内存
    std::free(memory1);
    std::free(memory2);

    return 0;
}

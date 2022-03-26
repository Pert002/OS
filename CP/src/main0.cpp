#include <iostream>
#include <vector>
#include "freeBlocksAllocator.h"
#include "binaryAllocator.h"
#include <chrono>


int main(){

    std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
    auto allocator = FreeBlocksAllocator(1024000);
    std::vector<void *> pointers;
    char mod;
    while (std::cin >> mod) {
        if (mod == '+') {
            int size;
            std::cin >> size;
            auto ptr = allocator.allocate(size);
            if (ptr) {
                pointers.push_back(ptr);
            }
        }
        if (mod == '-') {
            int index;
            std::cin >> index;
            allocator.deallocate(pointers[index]);
        }
    }
    std::chrono::time_point<std::chrono::system_clock> finish = std::chrono::system_clock::now();

    std::chrono::duration<double> difference = finish - start;
    const double ms = difference.count() * 1000;
    std::cout << ms << " ms\n";

}
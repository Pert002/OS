#include <iostream>
#include <vector>
#include "freeBlocksAllocator.h"
#include "binaryAllocator.h"
using namespace std;

int main () {
  auto allocator = FreeBlocksAllocator(1024);
  allocator.memoryDump();
  
  auto ptr1 = allocator.allocate(200);
  allocator.memoryDump();
  
  auto ptr2 = allocator.allocate(100);
  allocator.memoryDump();
  
  auto ptr3 = allocator.allocate(300);
  allocator.memoryDump();
  
  allocator.deallocate(ptr2);
  allocator.memoryDump();
  allocator.deallocate(ptr1);
  allocator.memoryDump();
  allocator.deallocate(ptr3);
  allocator.memoryDump();
}
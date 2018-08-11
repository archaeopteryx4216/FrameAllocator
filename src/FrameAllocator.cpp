#include <FrameAllocator.hpp>

frameallocator::FrameAllocatorManager* frameallocator::FrameAllocatorManager::manager = nullptr;

// FrameAllocator methods

frameallocator::FrameAllocator::FrameAllocator() {
  memory = nullptr;
  top = nullptr;
  unusedBytes = 0;
  usedBytes = 0;
}

frameallocator::FrameAllocator::FrameAllocator(size_t size) {
  memory = new byte[size]();
  top = memory;
  base = memory;
  *(stackptr*)base = memory;
  unusedBytes = size - sizeof(stackptr);
  usedBytes = sizeof(stackptr);
}

frameallocator::FrameAllocator::~FrameAllocator() {
  delete [] memory;
  memory = nullptr;
  top = nullptr;
  base = nullptr;
  unusedBytes = 0;
  usedBytes = 0;
}

frameallocator::stackptr frameallocator::FrameAllocator::getBlock(size_t size) {
  usedBytes += (size + sizeof(stackptr));
  if (unusedBytes - usedBytes < 0) {
    errors.push("Stack full, alloc failed.");
    usedBytes -= (size + sizeof(stackptr));
    return nullptr;
  }
  usedBytes += size + sizeof(stackptr);
  *(stackptr*)top = base;
  base = top;
  top += size + sizeof(stackptr);
  return base + sizeof(stackptr);
}

void frameallocator::FrameAllocator::freeBlock() {
  size_t numBytes = top - base;
  top = base;
  base = *(stackptr*)top;
  unusedBytes += numBytes;
  usedBytes -= numBytes;
}

frameallocator::frameptr frameallocator::FrameAllocator::getFrame() {
  return base;
}

bool frameallocator::FrameAllocator::freeFrame(frameptr frame) {
  if (frame > top) {
    errors.push("freeFrame failed, frame already freed");
    return false;
  }
  size_t numBytes = top - frame;
  top = frame;
  base = *(stackptr*)top;
  usedBytes -= numBytes;
  unusedBytes += numBytes;
  return true;
}

std::string frameallocator::FrameAllocator::getError() {
  std::string ret = errors.top();
  errors.pop();
  return ret;
}

// FrameAllocatorManager methods

frameallocator::FrameAllocatorManager& frameallocator::FrameAllocatorManager::getInstance() {
  if (nullptr == manager) {
    manager = new FrameAllocatorManager();
  }
  return *manager;
}

bool frameallocator::FrameAllocatorManager::createAllocator(std::string id, size_t size) {
  auto ret = allocators.insert(std::pair<std::string, FrameAllocator*>(id, new FrameAllocator(size)));
  if (!ret.second) {
    errors.push("CreateAllocator failed, id already used.");
    return false;
  }
  return true;
}

frameallocator::FrameAllocator* frameallocator::FrameAllocatorManager::getAllocator(std::string id) {
  auto it = allocators.find(id);
  if (it == std::end(allocators)) {
    errors.push("getAllocator failed, id \"" + id + "\" not used.");
    return nullptr;
  }
  return it->second;
}

bool frameallocator::FrameAllocatorManager::cleanupAllocator(std::string id) {
  auto it = allocators.find(id);
  if (it == std::end(allocators)) {
    errors.push("cleanupAllocator failed, id \"" + id + "\" not used.");
    return false;
  }
  allocators.erase(it);
  return true;
}

void frameallocator::FrameAllocatorManager::cleanupInstance() {
  delete manager;
  manager = nullptr;
}

std::string frameallocator::FrameAllocatorManager::getError() {
  std::string ret = errors.top();
  errors.pop();
  return ret;
}

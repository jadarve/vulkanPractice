#ifndef MEMORY_MANAGER_HPP_
#define MEMORY_MANAGER_HPP_

#include <vector>
#include <string>

#include <vulkan/vulkan.hpp>

#include "Buffer.hpp"

// compute kit
namespace ck {


bool containsMemoryFlags(const vk::MemoryPropertyFlags& flags1, const vk::MemoryPropertyFlags& flags2);
void printMemoryFlags(const vk::MemoryPropertyFlags& flags, const std::string& prefix="",
    const std::string& tabs="\t\t");

class MemoryManager {

public:
    MemoryManager();
    MemoryManager(  vk::PhysicalDevice& physicalDevice,
                    vk::Device& device,
                    const vk::MemoryPropertyFlags flags,
                    const size_t size);


public:
    void free();
    size_t getSize() const;
    vk::MemoryPropertyFlags getMemoryFlags() const;

    void bindBuffer(const ck::Buffer& buffer);

private:
    vk::Device device;
    vk::DeviceMemory memory;
    vk::MemoryPropertyFlags memoryFlags;
    size_t size;
    size_t offset;
};


} // namespace ck

#endif /* MEMORY_MANAGER_HPP_ */

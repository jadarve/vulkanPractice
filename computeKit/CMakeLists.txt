
include_directories(include)
add_executable(computeKit
    src/Session.cpp
    src/MemoryManager.cpp
    src/Buffer.cpp
    src/Program.cpp
    src/NodeDescriptor.cpp
    src/Node.cpp
    src/main.cpp)

target_link_libraries(computeKit vulkan)
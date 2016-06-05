
#ifndef LOADER_H_
#define LOADER_H_

#include <string>
#include <functional>

#include <dlfcn.h>

#include <vulkan/vulkan.h>

// THINK: Should this class be a Singleton?
class VulkanLibrary {

public:

	~VulkanLibrary();

	bool load();
	void initGlobalFunctions();
	std::string getErrorMessage();

	void printInstanceExtensionProperties();
	void printInstanceLayerProperties();

	template<class T>
    std::function<T> loadFunction(const std::string& functionName) const {

        dlerror();
        void* result = dlsym(_library, functionName.c_str());
        if (!result) {
            char* error = dlerror();
            if (error) {
                throw std::logic_error("can't find symbol named \"" + functionName + "\": " + error);
            }
        }

        return reinterpret_cast<T*>(result);
    }

    template<class T>
    std::function<T> loadInstanceFunction(const std::string& functionName) const {

    	auto functionPtr = _vkGetInstanceProcAddr(nullptr, functionName.c_str());
    	if(functionPtr == nullptr) {
    		throw std::logic_error("Error loading Vulkan instance function \"" + functionName + "\":");
    	}
    	return reinterpret_cast<T*>(functionPtr);
    }

private:
	// FIXME: need to enclose _library in a shared_ptr to support
	// copy/move semantics. Otherwise the destructor will call dlclose
	// before time.
	void* _library {nullptr};

	// If instance is NULL, pName must be one of: vkEnumerateInstanceExtensionProperties,
	// vkEnumerateInstanceLayerProperties or vkCreateInstance

	// used to load instance functions
	std::function<PFN_vkVoidFunction(VkInstance, const char*)> _vkGetInstanceProcAddr;

	// THINK: should instance level functions be stored in a VkInstanceFactory class?

	// INSTANCE LEVEL FUNCTIONS
	std::function<VkResult(const VkInstanceCreateInfo*, const VkAllocationCallbacks*, VkInstance*)> _vkCreateInstance;

	// from documentation, vkDestroyInstance needs to be loaded specifically for each instance. 
	std::function<void(VkInstance instance, const VkAllocationCallbacks* pAllocator)> _vkDestroyInstance;
	
	std::function<VkResult(const char*, uint32_t*, VkExtensionProperties*)> _vkEnumerateInstanceExtensionProperties;
	std::function<VkResult(uint32_t*, VkLayerProperties*)> _vkEnumerateInstanceLayerProperties;
};


#endif
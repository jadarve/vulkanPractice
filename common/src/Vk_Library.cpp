
#include <iostream>
#include <sstream>
#include <vector>
#include <dlfcn.h>

#include "Vk_Library.hpp"

using namespace std;


// Singleton instance
std::shared_ptr<Vk_Library> Vk_Library::_instance = nullptr;


// can throw runtime_error
std::shared_ptr<Vk_Library> Vk_Library::get() {

	if(_instance == nullptr) {
		Vk_Library* aux = new Vk_Library();
		_instance = std::shared_ptr<Vk_Library>(aux);

		// I cannot use make_shared because the constructor is protected
		// _instance = std::make_shared<Vk_Library>();
	}

	return _instance;
}


// Protected constructor
Vk_Library::Vk_Library() {

	std::cout << "Vk_Library::Vk_Library(): creating instance" << std::endl;

	// FROM DOCUMENTATION: http://pubs.opengroup.org/onlinepubs/7908799/xsh/dlopen.html
	// Only a single copy of an object file is brought
	// into the address space, even if dlopen() is invoked
	// multiple times in reference to the file, and even if
	// different pathnames are used to reference the file. 
	void* libPtr = dlopen("libvulkan.so", RTLD_NOW);

	// error loading Vulkan
	if(libPtr == nullptr) {
		throw runtime_error{"Error loading Vulkan library: " + std::string{dlerror()}};
	}

	// when library is going to be deleted, it will call dlclose
	_library = std::shared_ptr<void>(libPtr,
		[](void* libPtr){dlclose(libPtr); std::cout << "Unloading Vullkan library..." << std::endl;});


	// load global function vkGetInstanceProcAddr
	_vkGetInstanceProcAddr = loadFunction<PFN_vkVoidFunction(VkInstance, const char*)>("vkGetInstanceProcAddr");

	// load instance functions
	initInstanceFunctions();
}


std::string Vk_Library::getExtensionPropertiesString() const {

	std::stringbuf buffer;
	std::ostream os{&buffer};

	uint32_t extensionCount = 0;

	if(_vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr) != VK_SUCCESS) {
		throw runtime_error{"Vk_Library::getExtensionPropertiesString(): error getting instance extension count"};
	}

	os << "Vulkan instance extension count: " << extensionCount << endl;

	if(extensionCount > 0) {

		vector<VkExtensionProperties> properties(extensionCount);
		_vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, &properties[0]);

		// iterates the instances
		for(auto p : properties) {
			os << p.extensionName << endl;
			os << "\tspecVersion: " << p.specVersion << endl;
		}
	}

	return buffer.str();
}


std::string Vk_Library::getLayerPropertiesString() const {

	std::stringbuf buffer;
	std::ostream os{&buffer};

	uint32_t layerCount = 0;
	if(_vkEnumerateInstanceLayerProperties(&layerCount, nullptr) != VK_SUCCESS) {
		throw runtime_error{std::string(__FUNCTION__) + std::string("(): error getting Vulkan instance layer count")};
	}

	os << "Vulkan instance layers count: " << layerCount << endl;

	if(layerCount > 0) {

		vector<VkLayerProperties> layers(layerCount);
		_vkEnumerateInstanceLayerProperties(&layerCount, &layers[0]);

		for(auto p : layers) {
			os << p.layerName << endl;
			os << "\tspecVersion: " << p.specVersion << endl;
			os << "\timplementationVersion: " << p.implementationVersion << endl;
			os << "\tdescription: " << p.description << endl;
		}
	}

	return buffer.str();
}

std::shared_ptr<Vk_Instance> Vk_Library::createInstance() {

	return std::make_shared<Vk_Instance>();
}

void Vk_Library::initInstanceFunctions() {

	_vkCreateInstance = loadInstanceFunction<VkResult(const VkInstanceCreateInfo*,
		const VkAllocationCallbacks*, VkInstance*) >("vkCreateInstance");

	_vkEnumerateInstanceExtensionProperties = loadInstanceFunction<VkResult(const char*,
		uint32_t*, VkExtensionProperties*)>("vkEnumerateInstanceExtensionProperties");

	_vkEnumerateInstanceLayerProperties = loadInstanceFunction<
		VkResult(uint32_t*, VkLayerProperties*)>("vkEnumerateInstanceLayerProperties");
}


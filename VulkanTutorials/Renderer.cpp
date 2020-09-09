#include "Renderer.h"

// standard template library includes
#include <iostream>
#include <cstdlib>
#include <assert.h>
#include <vector>
#include <sstream>

// Project includes
#include "Shared.h"

Renderer::Renderer()
{
	SetupDebug();
	InitInstance();
	InitDebug();
	InitDevice();
}

Renderer::~Renderer()
{
	DeInitDevice();
	DeInitDebug();
	DeInitInstance();
}

void Renderer::InitInstance()
{
	VkApplicationInfo applicationInfo{};
	applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	applicationInfo.apiVersion = VK_MAKE_VERSION(1, 0, 3);
	applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	applicationInfo.pApplicationName = "Vulkan Tutorial";


	VkInstanceCreateInfo instanceInfo{};
	instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceInfo.pApplicationInfo = &applicationInfo;
	instanceInfo.enabledLayerCount = InstanceLayers.size();
	instanceInfo.ppEnabledLayerNames = InstanceLayers.data();
	instanceInfo.enabledExtensionCount = InstanceExensions.size();
	instanceInfo.ppEnabledExtensionNames = InstanceExensions.data();
	instanceInfo.pNext = &DebugCallbackCreateInfo;

	ErrorCheck(vkCreateInstance(&instanceInfo, nullptr, &instance));

}

void Renderer::DeInitInstance()
{
	vkDestroyInstance(instance, nullptr);
	instance = nullptr;
}

void Renderer::InitDevice()
{
	uint32_t gpuCount = 0;
	vkEnumeratePhysicalDevices(instance, &gpuCount, nullptr);
	std::vector <VkPhysicalDevice> gpuVec(gpuCount);
	vkEnumeratePhysicalDevices(instance, &gpuCount, gpuVec.data());
	GPU = gpuVec[0];

	vkGetPhysicalDeviceProperties(GPU, &GPUProperties);

	uint32_t familyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(GPU, &familyCount, nullptr);
	std::vector<VkQueueFamilyProperties> familyPropertyVec(familyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(GPU, &familyCount, familyPropertyVec.data());

	bool found = false;
	for (uint32_t i = 0; i < familyCount; ++i)
	{
		if (familyPropertyVec[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			found = true;
			graphicsFamilyIndex = i;
		}
	}

	if (!found)
	{
		assert(0 && "VULKAN ERROR: Queue family supporting graphics not found");
		std::exit(-1);
	}

	uint32_t LayerCount = 0;

	// List all layers installed on system
	vkEnumerateInstanceLayerProperties(&LayerCount, nullptr);
	std::vector<VkLayerProperties> LayerPropertyVec(LayerCount);
	vkEnumerateInstanceLayerProperties(&LayerCount, LayerPropertyVec.data());
	for (auto &i : LayerPropertyVec)
	{
		std::cout << "Instance layer " << i.layerName << "\t\t" << i.description << std::endl;
	}

	uint32_t DeviceLayerCount = 0;

	// List all layers installed on system
	vkEnumerateDeviceLayerProperties(GPU, &DeviceLayerCount, nullptr);
	std::vector<VkLayerProperties> DeviceLayerPropertyVec(LayerCount);
	vkEnumerateDeviceLayerProperties(GPU, &DeviceLayerCount, LayerPropertyVec.data());
	for (auto &i : LayerPropertyVec)
	{
		std::cout << "Instance layer " << i.layerName << "\t\t" << i.description << std::endl;
	}

	float queuePriorities[]{ 1.0f };

	VkDeviceQueueCreateInfo deviceQueueCreateInfo{};
	deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	deviceQueueCreateInfo.queueFamilyIndex = graphicsFamilyIndex;
	deviceQueueCreateInfo.queueCount = 1;
	deviceQueueCreateInfo.pQueuePriorities = queuePriorities;

	VkDeviceCreateInfo deviceCreateInfo{};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.queueCreateInfoCount = 1;
	deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;

	// deviceCreateInfo.enabledLayerCount = DeviceLayers.size();
	// deviceCreateInfo.ppEnabledLayerNames = DeviceLayers.data();
	deviceCreateInfo.enabledExtensionCount = DeviceExtensions.size();
	deviceCreateInfo.ppEnabledExtensionNames = DeviceExtensions.data();

	 ErrorCheck(vkCreateDevice(GPU, &deviceCreateInfo, nullptr, &device));

	 vkGetDeviceQueue(device, graphicsFamilyIndex, 0, &queue);
}

void Renderer::DeInitDevice()
{
	vkDestroyDevice(device, nullptr);
	device = nullptr;
}

VKAPI_ATTR VkBool32 VKAPI_CALL
VulkanDebugCallback(
	VkDebugReportFlagsEXT msgFlags, // What kind of error
	VkDebugReportObjectTypeEXT objType, // What debug object is getting sent 
	uint64_t srcObj, // pointer to object
	size_t location, 
	int32_t msgCode,
	const char* layerPrefix, // what layer is called
	const char* msg,
	void* userData
	)
{
	std::ostringstream stream;
	stream << "VKDEBUG: ";
	if (msgFlags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT)
	{
		stream << "INFO: ";
	}
	if (msgFlags & VK_DEBUG_REPORT_WARNING_BIT_EXT) {
		stream << "WARNING: ";
	}
	if (msgFlags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) {
		stream << "PERFORMANCE: ";
	}
	if (msgFlags & VK_DEBUG_REPORT_ERROR_BIT_EXT) {
		stream << "ERROR: ";
	}
	if (msgFlags & VK_DEBUG_REPORT_DEBUG_BIT_EXT) {
		stream << "DEBUG: ";
	}

	stream << "@[" << layerPrefix << "]: ";
	stream << msg << std::endl;
	std::cout << stream.str();

	return false;
}

void Renderer::SetupDebug()
{

	DebugCallbackCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
	DebugCallbackCreateInfo.pfnCallback = VulkanDebugCallback;
	DebugCallbackCreateInfo.flags = VK_DEBUG_REPORT_INFORMATION_BIT_EXT |
		VK_DEBUG_REPORT_WARNING_BIT_EXT |
		VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT |
		VK_DEBUG_REPORT_ERROR_BIT_EXT |
		VK_DEBUG_REPORT_DEBUG_BIT_EXT |
		VK_DEBUG_REPORT_FLAG_BITS_MAX_ENUM_EXT |
		0;

	// Enables standard validation for instance
	 InstanceLayers.push_back("VK_LAYER_LUNARG_standard_validation");
	
	/* Enable manual validation layers
	InstanceLayers.push_back("VK_LAYER_LUNARG_threading");
	InstanceLayers.push_back("VK_LAYER_LUNARG_draw_state");
	InstanceLayers.push_back("VK_LAYER_LUNARG_image");
	InstanceLayers.push_back("VK_LAYER_LUNARG_mem_tracker");
	InstanceLayers.push_back("VK_LAYER_LUNARG_object_tracker");
	InstanceLayers.push_back("VK_LAYER_LUNARG_param_checker");
	*/
	InstanceExensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

	//DeviceLayers.push_back("VK_LAYER_LUNARG_standard_validation");
	/*0
	DeviceLayers.push_back("VK_LAYER_LUNARG_threading");
	DeviceLayers.push_back("VK_LAYER_LUNARG_draw_state");
	DeviceLayers.push_back("VK_LAYER_LUNARG_image");
	DeviceLayers.push_back("VK_LAYER_LUNARG_mem_tracker");
	DeviceLayers.push_back("VK_LAYER_LUNARG_object_tracker");
	DeviceLayers.push_back("VK_LAYER_LUNARG_param_checker");
	*/

	/**
	* If you disable some layers, it will crash
	* you must have all layers you plan to use
	* Initialized
	*/
}

// Pointer functions to debug callbacks
PFN_vkCreateDebugReportCallbackEXT		fvkCreateDebugReportCallbackEXT = nullptr;
PFN_vkDestroyDebugReportCallbackEXT		fvkDestroyDebugReportCallbackEXT = nullptr;

void Renderer::InitDebug()
{
	fvkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
	fvkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
	if (nullptr == fvkCreateDebugReportCallbackEXT || nullptr == fvkDestroyDebugReportCallbackEXT)
	{
		assert(0 && "Vulkan ERROR: Can't fetch debug function pointers.");
		std::exit(-1);
	}
	fvkCreateDebugReportCallbackEXT(instance, &DebugCallbackCreateInfo, nullptr, &DebugReport);
}

void Renderer::DeInitDebug()
{
	fvkDestroyDebugReportCallbackEXT(instance, DebugReport, nullptr);
	DebugReport = nullptr;
}

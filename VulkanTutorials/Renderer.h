#pragma once

#include <vulkan/vulkan.h>
#include <vector>

class Renderer
{
public:

	Renderer();
	~Renderer();

	VkDevice GetDevice() { return device; }
	uint32_t GetGraphicsFamilyIndex() { return graphicsFamilyIndex; }
	VkQueue GetQueue() { return queue; }

private:
	void InitInstance();
	void DeInitInstance();

	void InitDevice();
	void DeInitDevice();

	void SetupDebug();
	void InitDebug();
	void DeInitDebug();

	VkInstance instance = VK_NULL_HANDLE;
	VkDevice device = VK_NULL_HANDLE;
	VkQueue queue = VK_NULL_HANDLE;
	VkPhysicalDevice GPU = VK_NULL_HANDLE;
	VkPhysicalDeviceProperties GPUProperties{};

	uint32_t graphicsFamilyIndex = 0;

	// Enable layers
	std::vector<const char*> InstanceLayers;
	std::vector<const char*> InstanceExensions;
	//std::vector<const char*> DeviceLayers; // depricated
	std::vector<const char*> DeviceExtensions;

	VkDebugReportCallbackEXT DebugReport = VK_NULL_HANDLE;
	VkDebugReportCallbackCreateInfoEXT DebugCallbackCreateInfo = {};
};


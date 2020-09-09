// standard template library includes
#include <iostream>
#include <cstdlib>

// Vulkan Includes

// Project Includes
#include "Renderer.h"

int main() 
{
	Renderer renderer;

	auto device = renderer.GetDevice();
	auto queue = renderer.GetQueue();

	// Wait for CPU to be ready on GPU
	VkFence fence;
	VkFenceCreateInfo fenceCreateInfo{};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	vkCreateFence(device, &fenceCreateInfo, nullptr, &fence);
	

	// Wait for GPU to be ready
	VkSemaphore semaphore;
	VkSemaphoreCreateInfo SemaphoreCreateInfo;
	SemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	vkCreateSemaphore(device, &SemaphoreCreateInfo, nullptr, &semaphore);

	VkCommandPool CommandPool;
	VkCommandPoolCreateInfo PoolCreateInfo{};
	PoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	PoolCreateInfo.queueFamilyIndex = renderer.GetGraphicsFamilyIndex();
	PoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	vkCreateCommandPool(device, &PoolCreateInfo, nullptr, &CommandPool);
	
	VkCommandBuffer CommandBuffer;
	VkCommandBufferAllocateInfo CommandBufferAllocateInfo{};
	CommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	CommandBufferAllocateInfo.commandPool = CommandPool;
	CommandBufferAllocateInfo.commandBufferCount = 1;
	CommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	vkAllocateCommandBuffers(device, &CommandBufferAllocateInfo, &CommandBuffer);

	VkCommandBufferBeginInfo BeginInfo{};
	BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	vkBeginCommandBuffer(CommandBuffer, &BeginInfo);

	vkEndCommandBuffer(CommandBuffer);

	VkSubmitInfo SubmitInfo{};
	SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	SubmitInfo.commandBufferCount = 1;
	SubmitInfo.pCommandBuffers = &CommandBuffer;
	vkQueueSubmit(queue, 1, &SubmitInfo, fence);

	auto ret = vkWaitForFences(device, 1, &fence, VK_TRUE, UINT64_MAX);
	vkQueueWaitIdle(queue);

	vkDestroyCommandPool(device, CommandPool, nullptr);
	vkDestroyFence(device, fence, nullptr);

	return EXIT_SUCCESS;
}
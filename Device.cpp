#include "Device.hpp"
#include <set>
#include <iostream>
#include <array>
namespace cow 
{
	/* Constructors, Destructors and Helper Functions */
	Device::Device(Window& window, uint32_t* queues, uint32_t queueCount)
		: m_physicalDevice{ createSurface_and_findPhysicalDevice(&window) },
		m_ref_window{ window }
	{
		VkPhysicalDeviceProperties properties{};
		vkGetPhysicalDeviceProperties(m_physicalDevice, &properties);
		std::cout << "physical device: " << properties.deviceName << std::endl;
		createLogicalDevice();
		createCommandPool();
	}

	Device::~Device()
	{
		vkDestroySurfaceKHR(m_instance.get(), m_surfaceKHR, nullptr);
		
		vkDestroyCommandPool(m_device, m_commandPool, nullptr);
		vkDestroyDevice(m_device, nullptr);
	}
	VkPhysicalDevice Device::createSurface_and_findPhysicalDevice(Window* window)
	{
		VkPhysicalDevice r_device[1];
		// Create Surface
		window->createSurface(m_instance.instance, &m_surfaceKHR);

		// find physical device
		auto [deviceCount, physicalDevices] = m_instance.enumeratePhysicalDevices();

		for (size_t i = 0; i < deviceCount; i++)
		{
			if (deviceSuitable(physicalDevices[i]))
			{
				r_device[0] = physicalDevices[i];
			}
		}
		free(physicalDevices);
		if (r_device[0] == VK_NULL_HANDLE)
		{
			throw std::runtime_error("failed to find a gpu with vulkan support");
		}
		return r_device[0];
	}
	bool Device::deviceExtensionSupport(VkPhysicalDevice device)
	{
		// Device Extensions Chosen
		const std::array<const char*, 1> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(
			device,
			nullptr,
			&extensionCount,
			availableExtensions.data());
		std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

		for (const auto& extension : availableExtensions) {
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}
	SwapChainSupportDetails Device::querySwapChainSupport(VkPhysicalDevice device)
	{
		SwapChainSupportDetails details{};
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_surfaceKHR, &details.capabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surfaceKHR, &formatCount, nullptr);

		if (formatCount != 0) {
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surfaceKHR, &formatCount, details.formats.data());
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surfaceKHR, &presentModeCount, nullptr);

		if (presentModeCount != 0) {
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(
				device,
				m_surfaceKHR,
				&presentModeCount,
				details.presentModes.data());
		}
		return details;
	}
	bool Device::deviceSuitable(VkPhysicalDevice device)
	{
		// Check for queue family indices
		QueueInUse indices = findQueues(device);

		// check for extension support
		bool extSupport = deviceExtensionSupport(device);
		// check for swap chain support
		bool swapChainAdequate = false;
		if (extSupport) {
			SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
			swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
		}
		VkPhysicalDeviceFeatures supportedFeatures;
		vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

		return indices.isComplete()
			&& extSupport
			&& swapChainAdequate
			&& supportedFeatures.samplerAnisotropy;
	}
	QueueInUse Device::findQueues(VkPhysicalDevice device)
	{
		QueueInUse indices{};

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto& queueFamily : queueFamilies) {
			if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				indices.graphicsIndex = i;
			}
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surfaceKHR, &presentSupport);
			if (queueFamily.queueCount > 0 && presentSupport) {
				indices.surfaceIndex = i;
			}
			if (indices.isComplete()) {
				break;
			}
			i++;
		}
		return indices;
	}
	void Device::createLogicalDevice()
	{
		const std::array<const char*, 1> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

		QueueInUse indices = findQueues(m_physicalDevice);

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = { *indices.graphicsIndex, *indices.surfaceIndex };
		float queuePriority = 1.0f;

		for (size_t itr : uniqueQueueFamilies)
		{
			VkDeviceQueueCreateInfo qCreateInfo{};
			qCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			qCreateInfo.queueFamilyIndex = itr;
			qCreateInfo.queueCount = 1;
			qCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(qCreateInfo);
		}

		VkPhysicalDeviceFeatures deviceFeatures = {};
		deviceFeatures.samplerAnisotropy = VK_TRUE;

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		
		createInfo.pEnabledFeatures = &deviceFeatures;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
		createInfo.ppEnabledExtensionNames = deviceExtensions.data();

		if (vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device) != VK_SUCCESS) 
		{
			throw std::runtime_error("couldn't create VkDevice");
		}
		vkGetDeviceQueue(m_device, *indices.graphicsIndex, 0, &graphicsQueue);
		vkGetDeviceQueue(m_device, *indices.surfaceIndex, 0, &surfaceQueue);
	}

	/* Public Function Wrapper for VkDevice */

	// Object Creation
	VkDescriptorSetLayout Device::createDescriptorSetLayout(VkDescriptorSetLayoutCreateInfo* pCreateInfo) 
		const
	{
		VkDescriptorSetLayout r_descSetLayout;
		if (vkCreateDescriptorSetLayout(m_device, pCreateInfo, nullptr, &r_descSetLayout) != VK_SUCCESS)
			throw std::runtime_error("failed to create descriptor set layout");
		return r_descSetLayout;
	}

	VkPipelineLayout Device::createPipelineLayout(VkPipelineLayoutCreateInfo* pCreateInfo) 
		const
	{
		VkPipelineLayout r_pipelineLayout;
		if (vkCreatePipelineLayout(m_device, pCreateInfo, nullptr, &r_pipelineLayout) != VK_SUCCESS)
			throw std::runtime_error("failed to create pipeline layout");
		return r_pipelineLayout;
	}
	VkPipelineCache Device::createPipelineCache(VkPipelineCacheCreateInfo* pCreateInfo, VkPipelineCache* ppipelineCache) 
		const
	{
		VkPipelineCache r_pipelineCache;
		if (vkCreatePipelineCache(m_device, pCreateInfo, nullptr, &r_pipelineCache) != VK_SUCCESS)
			throw std::runtime_error("failed to create pipeline cache");
		return r_pipelineCache;
	}
	VkPipeline Device::createComputePipelines(VkPipelineCache pPipelineCache, uint32_t createInfoCount, VkComputePipelineCreateInfo *pCreateInfo) 
		const
	{
		VkPipeline r_pipeline;
		if (vkCreateComputePipelines(m_device, pPipelineCache, createInfoCount, pCreateInfo, nullptr, &r_pipeline) != VK_SUCCESS)
			throw std::runtime_error("failed to create pipeline");
		return r_pipeline;

	}
	VkDescriptorPool Device::createDescriptorPool(VkDescriptorPoolCreateInfo* pCreateInfo) 
		const
	{
		VkDescriptorPool r_descriptorPool;
		if (vkCreateDescriptorPool(m_device, pCreateInfo, nullptr, &r_descriptorPool) != VK_SUCCESS)
			throw std::runtime_error("failed to create descriptor pool");
		return r_descriptorPool;
	}
	VkCommandPool Device::createCommandPool(VkCommandPoolCreateInfo* pCreateInfo)
		const
	{
		VkCommandPool r_commandPool;
		VkResult x;
		x = vkCreateCommandPool(m_device, pCreateInfo, nullptr, &r_commandPool);
		if (x != VK_SUCCESS)
			throw std::runtime_error("failed to create command pool");
		return r_commandPool;
	}
	VkBuffer Device::createBuffer(VkBufferCreateInfo* pCreateInfo) 
		const
	{
		VkBuffer r_buffer;
		if (vkCreateBuffer(m_device, pCreateInfo, nullptr, &r_buffer) != VK_SUCCESS)
			throw std::runtime_error("failed to create buffer");
		return r_buffer;
	}
	VkFence Device::createFence(VkFenceCreateInfo* pCreateInfo) 
		const
	{
		VkFence r_fence;
		if (vkCreateFence(m_device, pCreateInfo, nullptr, &r_fence) != VK_SUCCESS)
			throw std::runtime_error("failed to create fence");
		return r_fence;
	}

	// Memory Allocation
	VkCommandBuffer Device::allocateCommandBuffers(VkCommandBufferAllocateInfo* pAllocInfo)
		const 
	{
		VkCommandBuffer r_commandBuffer;
		if (vkAllocateCommandBuffers(m_device, pAllocInfo, &r_commandBuffer))
			throw std::runtime_error("failed to allocate command buffers");
		return r_commandBuffer;
	}
	VkDescriptorSet Device::allocateDescriptorSets(VkDescriptorSetAllocateInfo* pAllocInfo)
		const
	{
		VkDescriptorSet r_descriptorSet;
		if (vkAllocateDescriptorSets(m_device, pAllocInfo, &r_descriptorSet))
			throw std::runtime_error("failed to allocate descriptor sets");
		return r_descriptorSet;
	}
	VkDeviceMemory Device::allocateMemory(VkMemoryAllocateInfo* pAllocInfo)
		const
	{
		VkDeviceMemory r_deviceMemory;
		if (vkAllocateMemory(m_device, pAllocInfo, nullptr, &r_deviceMemory))
			throw std::runtime_error("failed to allocate device memory");
		return r_deviceMemory;
	}
	void Device::createCommandPool()
	{
		QueueInUse queues = findQueues(m_physicalDevice);

		VkCommandPoolCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		createInfo.queueFamilyIndex = *queues.graphicsIndex;
		createInfo.flags =
			VK_COMMAND_POOL_CREATE_TRANSIENT_BIT
			| VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

		//m_commandPool = device.createCommandPool(&createInfo);
		vkCreateCommandPool(m_device, &createInfo, nullptr, &m_commandPool);
	}
	VkFormat Device::findSupportedFormat(uint32_t candidateCount, VkFormat* candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
	{
		for (size_t i = 0; i < candidateCount; i++)
		{
			VkFormatProperties props{};
			vkGetPhysicalDeviceFormatProperties(m_physicalDevice, candidates[i], &props);

			if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
			{
				return candidates[i];
			}
			if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) 
			{
				return candidates[i];
			}
		}
		throw std::runtime_error("couldn't find supported format!");
	}
	uint32_t Device::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &memProperties);
		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			if ((typeFilter & (1 << i)) &&
				(memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}

		throw std::runtime_error("failed to find suitable memory type!");
	}
	VkCommandBuffer Device::beginSingleTimeCommands() {
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = m_commandPool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(m_device, &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		return commandBuffer;
	}

	void Device::endSingleTimeCommands(VkCommandBuffer commandBuffer) {
		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(graphicsQueue);

		vkFreeCommandBuffers(m_device, m_commandPool, 1, &commandBuffer);
	}
}

#pragma once
#include "Instance.hpp"
#include "Window.hpp"
#include <vector>
#include <optional>
#include <set>
#include <array>
#include <unordered_set>
namespace cow
{
	struct QueueInUse
	{
		std::optional<uint32_t> graphicsIndex;
		std::optional<uint32_t> surfaceIndex;
		std::optional<uint32_t> computeIndex;
		bool isComplete() 
		{ 
			return graphicsIndex.has_value() 
				&& surfaceIndex.has_value()
				&& computeIndex.has_value();
		}
	};

	struct SwapChainSupportDetails 
	{
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};
	class Device
	{
	public:
		Device(Window& window, uint32_t* queues, uint32_t queueCount);
		~Device();

		// no copy or move constructors
		Device(const Device&) = delete;
		Device& operator=(const Device&) = delete;
		Device(Device&&) = delete;
		Device& operator=(Device&&) = delete;

		// -=-=-=-=-=-=- Getters -=-=-=-=-=-=-
		VkSurfaceKHR	 getSurfaceKHR()		{ return m_surfaceKHR;		}
		VkPhysicalDevice getPhysicalDevice()	{ return m_physicalDevice;	}
		VkDevice		 getDevice()			{ return m_device;			}
		VkCommandPool	 getCommandPool()		{ return m_commandPool;		}
		VkQueue			 getGraphicsQueue()		{ return graphicsQueue;		}
		VkQueue			 getSurfaceQueue()		{ return surfaceQueue;		}
		VkQueue			 getComputeQueue()		{ return computeQueue;		}
		// -=-=-=-=-=-=- Public Functions -=-=-=-=-=-=-
		
		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
		VkCommandBuffer beginSingleTimeCommands();
		void endSingleTimeCommands(VkCommandBuffer commandBuffer);
	
		void createImage2D(
			uint32_t width,
			uint32_t height,
			VkFormat format,
			VkImageUsageFlags usage,
			VkImageTiling tiling,
			VkImageLayout layout,
			VkImage* pImage);
		void allocateImageMemory(VkImage pImage, VkDeviceMemory* pImageMemory);
		VkCommandBuffer* allocateCommandBuffers(VkCommandBufferLevel level, uint32_t cmdBufferCount);
		void allocateCommandBuffers(VkCommandBufferLevel level, uint32_t cmdBufferCount, VkCommandBuffer* pCmdBuffers);
		inline VkResult bindImageMemory(VkImage image, VkDeviceMemory imageMemory)
			const
		{
			return vkBindImageMemory(m_device, image, imageMemory, 0);
		}

		Instance m_instance;
	private:
		// -=-=-=-=-=-=- Private Member Constructor Functions -=-=-=-=-=-=-
		void hasGflwRequiredInstanceExtensions();
		VkPhysicalDevice createSurface_and_findPhysicalDevice(Window *window);
		void createLogicalDevice();
		void createCommandPool();

		bool deviceSuitable(VkPhysicalDevice device);
		bool deviceExtensionSupport(VkPhysicalDevice device);

		QueueInUse findQueues(VkPhysicalDevice device);
		SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
		// -=-=-=-=-=-=- Private Friend Functions -=-=-=-=-=-=-
		VkFormat findSupportedFormat(uint32_t candidateCount, VkFormat* candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
		// -=-=-=-=-=-=- Private Member Variables -=-=-=-=-=-=-
		VkSurfaceKHR m_surfaceKHR;
		VkPhysicalDevice m_physicalDevice;
		VkDevice m_device;
		VkCommandPool m_commandPool;

		VkQueue graphicsQueue;
		VkQueue surfaceQueue;
		VkQueue computeQueue;

		Window &m_ref_window;

		friend class GraphicsCommands;
		friend class Swapchain;
	};

}

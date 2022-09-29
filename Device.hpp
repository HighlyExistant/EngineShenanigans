#pragma once
#include "Instance.hpp"
#include "Window.hpp"
#include <vector>
#include <optional>
#include <tuple>
namespace cow
{
	struct QueueInUse
	{
		std::optional<uint32_t> graphicsIndex;
		std::optional<uint32_t> surfaceIndex;
		bool isComplete() 
		{ 
			return graphicsIndex.has_value() 
			&& surfaceIndex.has_value(); 
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
		
		// -=-=-=-=-=-=- Public Functions -=-=-=-=-=-=-
		
		// * Object Creation

		VkDescriptorSetLayout createDescriptorSetLayout(VkDescriptorSetLayoutCreateInfo* pCreateInfo) const;
		VkPipelineLayout createPipelineLayout(VkPipelineLayoutCreateInfo* pCreateInfo) const;
		VkPipelineCache createPipelineCache(VkPipelineCacheCreateInfo* pCreateInfo, VkPipelineCache* ppipelineCache) const;
		VkPipeline createComputePipelines(VkPipelineCache pPipelineCache, uint32_t CreateInfoCount, VkComputePipelineCreateInfo* pCreateInfo) const;
		
		VkDescriptorPool createDescriptorPool(VkDescriptorPoolCreateInfo* pCreateInfo) const;
		VkCommandPool createCommandPool(VkCommandPoolCreateInfo* pCreateInfo) const;
		
		VkBuffer createBuffer(VkBufferCreateInfo* pCreateInfo) const;
		VkFence createFence(VkFenceCreateInfo* pCreateInfo) const;

		// * Object Destruction
		inline void destroyDescriptorSetLayout(VkDescriptorSetLayout descriptorSetLayout) 
			const { vkDestroyDescriptorSetLayout(m_device, descriptorSetLayout, nullptr); }
		inline void destroyPipelineLayout(VkPipelineLayout pipelineLayout) 
			const  { vkDestroyPipelineLayout(m_device, pipelineLayout, nullptr); }
		inline void destroyPipelineCache(VkPipelineCache pipelineCache) 
			const { vkDestroyPipelineCache(m_device, pipelineCache, nullptr); }
		inline void destroyPipeline(VkPipeline pipeline) 
			const { vkDestroyPipeline(m_device, pipeline, nullptr); }
		
		inline void destroyDescriptorPool(VkDescriptorPool descriptorPool) 
			const { vkDestroyDescriptorPool(m_device, descriptorPool, nullptr); }
		inline void destroyCommandPool(VkCommandPool commandPool) 
			const { vkDestroyCommandPool(m_device, commandPool, nullptr); }
		
		inline void destroyBuffer(VkBuffer buffer) 
			const { vkDestroyBuffer(m_device, buffer, nullptr); }
		inline void destroyFence(VkFence fence) 
			const { vkDestroyFence(m_device, fence, nullptr); }
	
		// * Memory Allocation
		VkCommandBuffer allocateCommandBuffers(VkCommandBufferAllocateInfo* pAllocInfo) 
			const;
		VkDescriptorSet allocateDescriptorSets(VkDescriptorSetAllocateInfo* pAllocInfo) 
			const;
		VkDeviceMemory allocateMemory(VkMemoryAllocateInfo* pAllocInfo) 
			const;

		// * Memory Deallocation
		inline void freeMemory(VkDeviceMemory& memory) 
			const { vkFreeMemory(m_device, memory, nullptr); }
		inline VkResult freeDescriptorSets(VkDescriptorPool pool,uint32_t descriptorSetCount, VkDescriptorSet *pDescriptorSet) 
			const {
			return vkFreeDescriptorSets(m_device, pool, descriptorSetCount, pDescriptorSet);
		}
		inline void freeCommandBuffers(VkCommandPool commandPool, uint32_t commandBufferCount, VkCommandBuffer* commandBuffer) 
			const { vkFreeCommandBuffers(m_device, commandPool, commandBufferCount, commandBuffer); }

	private:
		// -=-=-=-=-=-=- Private Member Constructor Functions -=-=-=-=-=-=-
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
		Instance m_instance;
		VkSurfaceKHR m_surfaceKHR;
		VkPhysicalDevice m_physicalDevice;
		VkDevice m_device;
		VkCommandPool m_commandPool;

		VkQueue graphicsQueue;
		VkQueue surfaceQueue;

		Window &m_ref_window;

		friend class GraphicsCommands;
		friend class Swapchain;
	};

}

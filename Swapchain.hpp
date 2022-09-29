#pragma once
#include "Device.hpp"
#include <memory>
#include <array>
namespace cow
{
	struct ImageResources
	{
		VkImage image;
		VkDeviceMemory memory;
		VkImageView view;
	};
	class Swapchain
	{
	public:
		static constexpr int MAX_FRAMES = 2;

		Swapchain(const Swapchain&) = delete;
		Swapchain& operator=(const Swapchain&) = delete;

		Swapchain(Device &device, VkExtent2D extent);
		Swapchain(Device& device, VkExtent2D extent, std::shared_ptr<Swapchain> old);
		~Swapchain();

		// -=-=-=-=-=-=- Getters -=-=-=-=-=-=-
		inline VkSwapchainKHR getSwapchainKHR() const { return m_swapChainKHR; }
	
		inline VkFormat getFormat() const { return m_scFormat; }
		inline VkExtent2D getExtent2D() const { return m_scExtent; }
		inline VkRenderPass getRenderPass() const { return m_renderPass; }
		inline VkFramebuffer getFrameBuffer(int index) { return m_pFramebuffers[index]; }


		inline size_t getCurrentFrame() const { return m_currentFrame; }

		bool compareSwapFormats(const Swapchain& swapChain) const
		{
			return swapChain.m_scFormat == this->m_scFormat;
		}

		// -=-=-=-=-=-=- Public Functions -=-=-=-=-=-=-
		VkResult nextImage(uint32_t* imageIndex);
		VkResult submit(const VkCommandBuffer* buffers, uint32_t* imageIndex);
	private:
		void createSwapchain();
		VkSurfaceFormatKHR	chooseSurfaceFormatKHR(VkSurfaceFormatKHR *pFormats, uint32_t formatCount);
		VkPresentModeKHR	choosePresentModeKHR(VkPresentModeKHR *pPresentModes, uint32_t presentModeCount);
		VkExtent2D			chooseExtent2D(const VkSurfaceCapabilitiesKHR& capabilities);
	
		void createRenderingObjects();
		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
		void createSyncObjects();
		
		VkExtent2D m_windowExtent;

		VkFormat m_scFormat;
		VkExtent2D m_scExtent;
		// -=-=-=-=-=-=- Swap Chain Member Variables -=-=-=-=-=-=-
		size_t m_currentFrame = 0;

		// * Swapchain
		VkSwapchainKHR m_swapChainKHR;

		// * Image Resources
		VkImage* m_pscImage;
		VkImageView* m_pscImageViews;
		uint32_t m_scImageViewCount;

		// * Render Pass
		VkRenderPass m_renderPass;

		// * Depth Resources
		ImageResources* m_depthResources;
		uint32_t m_depthResourceCount;
		
		// * Frame Buffer Resources
		VkFramebuffer *m_pFramebuffers;
		uint32_t m_framebufferCount;

		// * Synching Resources
		VkFence *m_pInFlightFences;
		//uint32_t inFlighFencesCount;

		VkFence *m_pImagesInFlight;
		//uint32_t imagesInFlightCount;

		VkSemaphore *m_pRenderFinishedSemaphore;
		//uint32_t renderFinishedSemaphoreCount;

		VkSemaphore *m_pImageAvailableSemaphore;
		//uint32_t imageAvailableSemaphoreCount;

		// Referenced Values
		std::shared_ptr<Swapchain> m_oldSwapChain;
		Device& m_ref_device;

		friend class GraphicsCommands;
	};
}

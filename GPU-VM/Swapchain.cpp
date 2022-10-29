#include "Swapchain.hpp"
#include <array>
#include <iostream>
namespace cow 
{
	Swapchain::Swapchain(Device& device, VkExtent2D extent)
		: m_windowExtent{ extent }, m_ref_device{ device }
	{
		createSwapchain();
		std::cout << "Created Swapchain\n";
		createRenderingObjects();
		std::cout << "Created Rendering Objects\n";
		createSyncObjects();
		std::cout << "Created Syncing Objects\n";
	}
	Swapchain::Swapchain(Device& device, VkExtent2D extent, std::shared_ptr<Swapchain> old)
		: m_ref_device{ device }, m_windowExtent{ extent }, m_oldSwapChain{ old }
	{
		createSwapchain();
		std::cout << "Created Swapchain\n";
		
		createRenderingObjects();
		std::cout << "Created Rendering Objects\n";
		
		createSyncObjects();
		std::cout << "Created Syncing Objects\n";

		m_oldSwapChain = nullptr;
	}
	Swapchain::~Swapchain()
	{
		// Destroy Image Views
		for (size_t i = 0; i < m_scImageViewCount; i++)
		{
			vkDestroyImageView(m_ref_device.getDevice(), m_pscImageViews[i], nullptr);
			/* Depth Resources */
			vkDestroyImageView(m_ref_device.getDevice(), m_depthResources[i].view, nullptr);
			vkDestroyImage(m_ref_device.getDevice(), m_depthResources[i].image, nullptr);
			vkFreeMemory(m_ref_device.getDevice(), m_depthResources[i].memory, nullptr);
			/* Frame Buffers */
			vkDestroyFramebuffer(m_ref_device.getDevice(), m_pFramebuffers[i], nullptr);
		}
		// Semaphores and Fences | Sync Objects 
		for (size_t i = 0; i < MAX_FRAMES; i++)
		{
			vkDestroySemaphore(m_ref_device.getDevice(), m_pRenderFinishedSemaphore[i], nullptr);
			vkDestroySemaphore(m_ref_device.getDevice(), m_pImageAvailableSemaphore[i], nullptr);
			vkDestroyFence(m_ref_device.getDevice(), m_pInFlightFences[i], nullptr);
		}
		// Image Object Pointers 
		free(m_pscImageViews);
		free(m_pscImage);
		free(m_depthResources);
		free(m_pFramebuffers);
		// Sync Object Pointers
		free(m_pImageAvailableSemaphore);
		free(m_pRenderFinishedSemaphore);
		free(m_pInFlightFences);
		free(m_pImagesInFlight);
		// Destroy Swapchain
		if (m_swapChainKHR != nullptr) 
		{
			vkDestroySwapchainKHR(m_ref_device.getDevice(), m_swapChainKHR, nullptr);
		}
		// Destroy Swapchain
		vkDestroyRenderPass(m_ref_device.getDevice(), m_renderPass, nullptr);
		// Destroy Framebuffer
	}
	void Swapchain::createSwapchain()
	{
		SwapChainSupportDetails swapChainSupport = m_ref_device.querySwapChainSupport(m_ref_device.m_physicalDevice);

		VkSurfaceFormatKHR surfaceFormat = chooseSurfaceFormatKHR(
			swapChainSupport.formats.data(),
			static_cast<uint32_t>(swapChainSupport.formats.size())
		);
		VkPresentModeKHR presentMode = choosePresentModeKHR(
			swapChainSupport.presentModes.data(),
			static_cast<uint32_t>(swapChainSupport.presentModes.size())
		);
		VkExtent2D windowExtent = chooseExtent2D(
			swapChainSupport.capabilities
		);
		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
		if (swapChainSupport.capabilities.maxImageCount > 0 &&
			imageCount > swapChainSupport.capabilities.maxImageCount) {
			imageCount = swapChainSupport.capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
		createInfo.surface = m_ref_device.m_surfaceKHR;
		createInfo.minImageCount = imageCount;

		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = windowExtent;	// Area that Vulkan can draw on the window

		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		QueueInUse indices = m_ref_device.findQueues(m_ref_device.m_physicalDevice);
		uint32_t queueFamilyIndices[] = { *indices.graphicsIndex, *indices.surfaceIndex };

		if (*indices.graphicsIndex != *indices.surfaceIndex)
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = nullptr;
		}
		createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;

		createInfo.oldSwapchain = m_oldSwapChain == nullptr ? VK_NULL_HANDLE : m_oldSwapChain->m_swapChainKHR;

		if (vkCreateSwapchainKHR(m_ref_device.m_device, &createInfo, nullptr, &m_swapChainKHR) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create swap chain!");
		}

		vkGetSwapchainImagesKHR(m_ref_device.m_device, m_swapChainKHR, &m_scImageViewCount, nullptr);
		m_pscImage = (VkImage*)calloc(m_scImageViewCount, sizeof(VkImage));
		m_pscImageViews = (VkImageView*)calloc(m_scImageViewCount, sizeof(VkImageView));
		vkGetSwapchainImagesKHR(m_ref_device.m_device, m_swapChainKHR, &m_scImageViewCount, m_pscImage);

		m_scFormat = surfaceFormat.format;
		m_scExtent = windowExtent;
	}
	VkSurfaceFormatKHR	Swapchain::chooseSurfaceFormatKHR(VkSurfaceFormatKHR* pFormats, uint32_t formatCount)
	{
		/*
			Finding support for the SRGB Color Space for
			Blue		255
			Green		255
			Red			255
			Alpha		255
			support when coloring pixels to the screen
		*/
		for (size_t i = 0; i < formatCount; i++)
		{
			if (pFormats[i].format == VK_FORMAT_B8G8R8A8_SRGB
				&& pFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				return pFormats[i];
			}
		}
		return pFormats[0];	// Try to avoid this from happening
	}
	VkPresentModeKHR Swapchain::choosePresentModeKHR(VkPresentModeKHR* pPresentModes, uint32_t presentModeCount)
	{
		/*
			Mailbox is the preffered present mode for high resolution
			high performance rendering but takes up a lot more energy
			and is not garunteed to be available in all GPU's
		*/
		for (size_t i = 0; i < presentModeCount; i++)
		{
			if (pPresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				// std::cout << "Present mode: Mailbox" << std::endl;
				return pPresentModes[i];
			}
		}
		/*
			This present mode does not wait for the image to be fully
			drawn before presenting which causes high performance but
			also high chance of screen tearing.
		*/
		// for (size_t i = 0; i < presentModeCount; i++)
		// {
		// 	if (pPresentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR)
		// 	{
		// 		m_ref_device.m_instance.logger.Log("immidiate present mode", COW_ERR_TYPE::FOUND);
		// 		return pPresentModes[i];
		// 	}
		// }

		/*
			This is the default present mode in Vulkan and the
			most likely to be supported. Its more well suited for
			low performance systems since it doesn't use too much
			memory or energy but results in less performance.
		*/
		return VK_PRESENT_MODE_FIFO_KHR;
	}
	VkExtent2D Swapchain::chooseExtent2D(const VkSurfaceCapabilitiesKHR& capabilities)
	{
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
			return capabilities.currentExtent;
		}

		VkExtent2D actualExtent = m_windowExtent;

		actualExtent.width = std::max(
			capabilities.minImageExtent.width,
			std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(
			capabilities.minImageExtent.height,
			std::min(capabilities.maxImageExtent.height, actualExtent.height));
		
		return actualExtent;
	}

#pragma warning( push )
#pragma warning( disable : 4042 6011 )
	void Swapchain::createRenderingObjects()
	{
		
		for (size_t i = 0; i < m_scImageViewCount; i++)
		{
			// Image View Creation
			VkImageViewCreateInfo viewInfo{};
			viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.image = m_pscImage[i];
			viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewInfo.format = m_scFormat;
			viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			viewInfo.subresourceRange.baseMipLevel = 0;
			viewInfo.subresourceRange.levelCount = 1;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount = 1;

			if (vkCreateImageView(
				m_ref_device.getDevice(),
				&viewInfo,
				nullptr,
				&m_pscImageViews[i]) != VK_SUCCESS
				)
			{
				throw std::runtime_error("failed to create texture image view!");
			}
		}
		// Render Pass Creation
		VkFormat candidates[] = { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT };
		VkFormat depthFormat = m_ref_device.findSupportedFormat(3, candidates, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
		m_depthFormat = depthFormat;
		VkAttachmentDescription depthAttachment{};
		depthAttachment.format = m_ref_device.findSupportedFormat(3, candidates, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
		depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		
		VkAttachmentReference depthAttachmentRef{};
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentDescription colorAttachment = {};
		colorAttachment.format = getImageFormat();
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		
		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		subpass.pDepthStencilAttachment = &depthAttachmentRef;

		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependency.srcStageMask =
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.dstSubpass = 0;
		dependency.dstStageMask =
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.dstAccessMask =
			VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;
		if (vkCreateRenderPass(m_ref_device.getDevice(), &renderPassInfo, nullptr, &m_renderPass) != VK_SUCCESS) 
		{
			throw std::runtime_error("failed to create render pass!");
		}
		// Depth Resource
		m_depthResources = (ImageResources*)calloc(m_scImageViewCount, sizeof(ImageResources));
		
		for (size_t i = 0; i < m_scImageViewCount; i++)
		{
			VkImageCreateInfo imageInfo{};
			imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageInfo.imageType = VK_IMAGE_TYPE_2D;
			imageInfo.extent.width = m_scExtent.width;
			imageInfo.extent.height = m_scExtent.height;
			imageInfo.extent.depth = 1;
			imageInfo.mipLevels = 1;
			imageInfo.arrayLayers = 1;
			imageInfo.format = depthFormat;
			imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
			imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			imageInfo.flags = 0;
			/* Constructine Image with Info */
			if (vkCreateImage(m_ref_device.getDevice(), &imageInfo, nullptr, &m_depthResources[i].image) != VK_SUCCESS) 
			{
				throw std::runtime_error("failed to create image!");
			}
			VkMemoryRequirements memRequirements;
			vkGetImageMemoryRequirements(m_ref_device.getDevice(), m_depthResources[i].image, &memRequirements);

			VkMemoryAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocInfo.allocationSize = memRequirements.size;
			allocInfo.memoryTypeIndex = m_ref_device.findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
			if (vkAllocateMemory(m_ref_device.m_device, &allocInfo, nullptr, &m_depthResources[i].memory) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to allocate image memory!");
			}
			if (vkBindImageMemory(m_ref_device.m_device, m_depthResources[i].image, m_depthResources[i].memory, 0) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to bind image memory!");
			}
			/* Constructing Image Info */
			VkImageViewCreateInfo viewInfo{};
			viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.image = m_depthResources[i].image;
			viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewInfo.format = depthFormat;
			viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			viewInfo.subresourceRange.baseMipLevel = 0;
			viewInfo.subresourceRange.levelCount = 1;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount = 1;

			if (vkCreateImageView(m_ref_device.getDevice(), &viewInfo, nullptr, &m_depthResources[i].view) != VK_SUCCESS) 
			{
				throw std::runtime_error("failed to create texture image view!");
			}
		}
		// Frame Buffer Creation

		m_pFramebuffers = (VkFramebuffer*)calloc(m_scImageViewCount, sizeof(VkFramebuffer));

		for (size_t i = 0; i < m_scImageViewCount; i++)
		{
			std::array<VkImageView, 2> attachments = { m_pscImageViews[i], m_depthResources[i].view};
			VkExtent2D swapChainExtent = m_scExtent;
			VkFramebufferCreateInfo framebufferInfo = {};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = m_renderPass;
			framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebufferInfo.pAttachments = attachments.data();
			framebufferInfo.width = swapChainExtent.width;
			framebufferInfo.height = swapChainExtent.height;
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer(
				m_ref_device.getDevice(),
				&framebufferInfo,
				nullptr,
				&m_pFramebuffers[i]) != VK_SUCCESS) {
				throw std::runtime_error("failed to create framebuffer!");
			}
		}
	}
	void Swapchain::createSyncObjects()
	{
		m_pImageAvailableSemaphore = (VkSemaphore*)calloc(MAX_FRAMES, sizeof(VkSemaphore));
		m_pRenderFinishedSemaphore = (VkSemaphore*)calloc(MAX_FRAMES, sizeof(VkSemaphore));
		m_pInFlightFences = (VkFence*)calloc(MAX_FRAMES, sizeof(VkFence));
		m_pImagesInFlight = (VkFence*)calloc(m_scImageViewCount, sizeof(VkFence));
	
		VkSemaphoreCreateInfo semaphoreInfo = {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo = {};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < MAX_FRAMES; i++)
		{
			if (
				vkCreateSemaphore(m_ref_device.getDevice(), &semaphoreInfo, nullptr, &m_pImageAvailableSemaphore[i]) != VK_SUCCESS 
				|| vkCreateSemaphore(m_ref_device.getDevice(), &semaphoreInfo, nullptr, &m_pRenderFinishedSemaphore[i]) != VK_SUCCESS 
				|| vkCreateFence(m_ref_device.getDevice(), &fenceInfo, nullptr, &m_pInFlightFences[i]) != VK_SUCCESS
				) 
			{
				throw std::runtime_error("failed to create synchronization objects for a frame!");
			}
		}
	}
#pragma warning( pop )

	VkResult Swapchain::nextImage(uint32_t* imageIndex)
	{
		vkWaitForFences(
			m_ref_device.getDevice(),
			1,
			&m_pInFlightFences[m_currentFrame],
			VK_TRUE,
			std::numeric_limits<uint64_t>::max());

		VkResult result = vkAcquireNextImageKHR(
			m_ref_device.getDevice(),
			m_swapChainKHR,
			std::numeric_limits<uint64_t>::max(),
			m_pImageAvailableSemaphore[m_currentFrame],
			VK_NULL_HANDLE,
			imageIndex);
		return result;
	}


	VkResult Swapchain::submit(const VkCommandBuffer* buffers, uint32_t* imageIndex)
	{
		// Queue for Image Presentation
		if (m_pImagesInFlight[*imageIndex] != VK_NULL_HANDLE)
			vkWaitForFences(m_ref_device.getDevice(), 1, &m_pImagesInFlight[*imageIndex], VK_TRUE, UINT64_MAX);
		
		m_pImagesInFlight[*imageIndex] = m_pInFlightFences[m_currentFrame];

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { m_pImageAvailableSemaphore[m_currentFrame] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = buffers;

		VkSemaphore signalSemaphores[] = { m_pRenderFinishedSemaphore[m_currentFrame] };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;
		vkResetFences(m_ref_device.getDevice(), 1, &m_pInFlightFences[m_currentFrame]);
		if (vkQueueSubmit(m_ref_device.graphicsQueue, 1, &submitInfo, m_pInFlightFences[m_currentFrame]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to submit draw command buffer!");
		}
		// Finish
		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = { m_swapChainKHR };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;

		presentInfo.pImageIndices = imageIndex;

		auto result = vkQueuePresentKHR(m_ref_device.surfaceQueue, &presentInfo);

		m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES;

		return result;
	}
}
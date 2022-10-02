#include "GraphicsCommands.hpp"

namespace cow 
{

	GraphicsCommands::GraphicsCommands(Device& device)
		: m_ref_device{ device }
	{
		recreateSwapchain();
		//swapchain = std::make_unique<Swapchain>(device, device.m_ref_window.getExtend2D());
		createCommandBuffers();
	}

	GraphicsCommands::~GraphicsCommands()
	{
		vkFreeCommandBuffers(m_ref_device.m_device, m_ref_device.m_commandPool, Swapchain::MAX_FRAMES, m_commandBuffers);
	}
	void GraphicsCommands::createCommandBuffers()
	{
		m_commandBuffers = (VkCommandBuffer*)calloc(Swapchain::MAX_FRAMES, sizeof(VkCommandBuffer));

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = m_ref_device.getCommandPool();
		allocInfo.commandBufferCount = Swapchain::MAX_FRAMES;
		vkAllocateCommandBuffers(m_ref_device.getDevice(), &allocInfo, m_commandBuffers);
		//m_commandBuffers = m_ref_device.allocateCommandBuffers(&allocInfo);
	}
	bool GraphicsCommands::checkRecreation(VkResult result)
	{
		/*
		* VK_ERROR_OUT_OF_DATE_KHR: This means that the surface that
		* is in use is unusable by the swapchain and we need to recreate it
		* this usually happens when you resize the window
		*/
		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			recreateSwapchain();
			return true;
		}
		/*
		* VK_SUCCESS is the desired outcome but VK_SUBOPTIMAL_KHR
		* can still use the current surface to draw
		* so it will be treated as a success
		*/
		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("failed to aquire swap chain image");
		}
	}
	VkCommandBuffer GraphicsCommands::begin()
	{
		VkResult result = swapchain->nextImage(&m_currentImageIndex);
		checkRecreation(result);

		auto commandBuffer = m_commandBuffers[swapchain->getCurrentFrame()];
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to begin recording command buffer");
		}
		return commandBuffer;
	}
	void GraphicsCommands::beginRenderPass(VkCommandBuffer commandBuffer, VkClearColorValue color)
	{
		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = swapchain->getRenderPass();
		renderPassInfo.framebuffer = swapchain->getFrameBuffer(m_currentImageIndex);

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = swapchain->getExtent2D();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = color;
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		// Dynamically setting viewport and scissor
		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(swapchain->getExtent2D().width);
		viewport.height = static_cast<float>(swapchain->getExtent2D().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0,0}, swapchain->getExtent2D() };
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	}

	void GraphicsCommands::recreateSwapchain()
	{
		auto extent = m_ref_device.m_ref_window.getExtend2D();
		while (extent.width == 0 || extent.height == 0)
		{
			extent = m_ref_device.m_ref_window.getExtend2D();
			glfwWaitEvents();
		}
		vkDeviceWaitIdle(m_ref_device.getDevice());

		if (swapchain == nullptr)
		{
			swapchain = std::make_unique<cow::Swapchain>(m_ref_device, extent);
		}
		else
		{
			std::shared_ptr<Swapchain> oldSwapChain = std::move(swapchain);

			swapchain = std::make_unique<Swapchain>(m_ref_device, extent, oldSwapChain);

			if (oldSwapChain->compareSwapFormats(*swapchain.get()))
			{
				throw std::runtime_error("Swap Chain Image format has changed");
			}
		}
	}
}

// #include "Instance.hpp"
#include <iostream>
#include "Device.hpp"
#include "Swapchain.hpp"
#include "Pipelines.hpp"
#include "GraphicsCommands.hpp"
struct SimplePushConstantData
{
	
};
int main()
{
	std::vector<VkCommandBuffer> commandBuffers(cow::Swapchain::MAX_FRAMES);

	std::cout << "\n\n" << sizeof(cow::Swapchain) << "\n\n";
	cow::Window window(800, 500, "name");

	cow::Device device(window, nullptr, 0);

	// cow::Swapchain swapchain(device, {10, 10});
	cow::GraphicsCommands commands{ device };

	cow::GraphicsPipelineSimpleInfo gpsi{};

	cow::PipelineLayoutSimpleInfo simpleInfo{};
	simpleInfo.pPushConstantRanges = nullptr;
	simpleInfo.pushConstantCount = 0;
	VkPipelineLayout layout = cow::defaultPipelineLayout(device.getDevice(), &simpleInfo);
	
	gpsi.pEntry = "main";
	gpsi.pFragpath = "C:\\Users\\anton\\source\\repos\\GPU-VM\\GPU-VM\\Shaders\\simple_shader.frag.spv";
	gpsi.pVertpath = "C:\\Users\\anton\\source\\repos\\GPU-VM\\GPU-VM\\Shaders\\simple_shader.vert.spv";
	gpsi.renderPass = commands.swapchain->getRenderPass();
	gpsi.pipelineLayout = layout;
	/*VkPushConstantRange pushConstRange{};
	pushConstRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	pushConstRange.offset = 0;
	pushConstRange.size = sizeof(SimplePushConstantData);*/

	cow::GraphicsPipelineSimpleInfo::defaultGraphicsPipeline(gpsi);

	cow::GraphicsPipeline graphicsPipeline{device, &gpsi };
	// Allocate Command Buffers
	VkCommandBufferAllocateInfo cmdAllocInfo{};
	cmdAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmdAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	cmdAllocInfo.commandPool = device.getCommandPool();
	cmdAllocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

	if (vkAllocateCommandBuffers(device.getDevice(), &cmdAllocInfo, commandBuffers.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate command buffers");
	}

	while (!window.shouldClose())
	{
		glfwPollEvents();
		size_t frameIndex = commands.swapchain->getCurrentFrame();
		// Begin Frame
		
		VkCommandBuffer cmdBuffer = commands.begin();
		commands.beginRenderPass(cmdBuffer);
		graphicsPipeline.bind(cmdBuffer);
		vkCmdDraw(cmdBuffer, 3, 1, 0, 0);
		vkCmdEndRenderPass(cmdBuffer);
		if (vkEndCommandBuffer(cmdBuffer) != VK_SUCCESS) 
		{
			throw std::runtime_error("failed to end cmdBuffer");
		}
		uint32_t index = commands.getCurrentImageIndex();
		VkResult result = commands.swapchain->submit(&cmdBuffer, &index);

		if (result != VK_SUCCESS) 
		{
			throw std::runtime_error("ASFDHFHUFDUHUYIF");
		}
		vkDeviceWaitIdle(device.getDevice());
		// Begin Swap chain render pass
		// VkCommandBuffer buffer = 
	}
	vkDestroyPipelineLayout(device.getDevice(), layout, nullptr);
}

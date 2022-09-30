// #include "Instance.hpp"

#include <iostream>
#include "Device.hpp"
#include "Swapchain.hpp"
#include "Pipelines.hpp"
#include "Buffer.hpp"
#include "GraphicsCommands.hpp"
#include "Model.hpp"
struct SimplePushConstantData
{
	glm::vec2 modelvec;
};
float lerp(float a, float b, float t) 
{
	return (1 - t) * a + t * b;
}

int main()
{
	std::vector<VkCommandBuffer> commandBuffers(cow::Swapchain::MAX_FRAMES);

	std::cout << "\n\n" << sizeof(cow::Swapchain) << "\n\n";
	cow::Window window(800, 500, "name");

	cow::Device device(window, nullptr, 0);

	cow::GraphicsCommands commands{ device };

	cow::GraphicsPipelineSimpleInfo gpsi{};

	VkPushConstantRange pushConstants{};
	pushConstants.offset = 0;
	pushConstants.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	pushConstants.size = sizeof(SimplePushConstantData);
	cow::PipelineLayoutSimpleInfo simpleInfo{};
	simpleInfo.pPushConstantRanges = &pushConstants;
	simpleInfo.pushConstantCount = 1;
	
	VkPipelineLayout layout = cow::defaultPipelineLayout(device.getDevice(), &simpleInfo);
	
	gpsi.pEntry = "main";
	gpsi.pFragpath = "C:\\Users\\anton\\source\\repos\\GPU-VM\\GPU-VM\\Shaders\\simple_shader.frag.spv";
	gpsi.pVertpath = "C:\\Users\\anton\\source\\repos\\GPU-VM\\GPU-VM\\Shaders\\simple_shader.vert.spv";
	gpsi.renderPass = commands.swapchain->getRenderPass();
	gpsi.pipelineLayout = layout;
	

	cow::GraphicsPipelineSimpleInfo::defaultGraphicsPipeline(gpsi);

	cow::GraphicsPipeline<cow::Vertex2DRGB> graphicsPipeline{device, &gpsi };
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
	float offset = 0.1;
	float colorChange = 0.0;

	VkClearColorValue value{};
	value.float32[0] = 0.1;
	value.float32[1] = 0.1;
	value.float32[2] = 0.1;
	value.float32[3] = 0.1;


	std::vector<cow::Vertex2DRGB> verticesMain;
	std::vector<cow::Vertex2DRGB> verticesOther;

	constexpr float outside = -2;
	constexpr float to_the_side = 0.50;
	constexpr float shade = 0.71;
	// Main Triangle
	verticesMain.push_back({ {0.0 + to_the_side, -0.5  }, {1.0 , 0.0, 0.0 } });
	verticesMain.push_back({ {0.5 + to_the_side , 0.5  }, {0.0, 1.0 , 0.0 } });
	verticesMain.push_back({ {-0.5 + to_the_side , 0.5 }, {0.0 , 0.0 , 1.0 } });

	// Meeting with this person
	verticesOther.push_back({ {(0.0 + outside) + offset, -0.45 }, {0.0 , 0.0 , 1.0  } });
	verticesOther.push_back({ {(0.5 + outside) + offset ,0.5 }, {0.0 , 1.0 , 0.0 } });
	verticesOther.push_back({ {(-0.5 + outside) + offset , 0.5 }, {1.0 , 0.0 , 0.0 } });

	cow::Model2D modelMain{ device, verticesMain };
	cow::Model2D modelOther{ device, verticesOther };

	while (!window.shouldClose())
	{
		glfwPollEvents();
		size_t frameIndex = commands.swapchain->getCurrentFrame();
		// Begin Frame
		
		VkCommandBuffer cmdBuffer = commands.begin();
		commands.beginRenderPass(cmdBuffer, value);
		graphicsPipeline.bind(cmdBuffer);
		// Vertex Buffer Start

		
		// Vertex Buffer End
		// Binding
		SimplePushConstantData data{};
		SimplePushConstantData otherdata{};
		otherdata.modelvec = { 0.0, 0.0 };
		data.modelvec = { 0.0 + offset, 0.0 };
		offset += 0.0001;
		modelOther.pushConst(cmdBuffer,
			layout,
			VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
			&data
		);
		modelOther.bind(cmdBuffer);
		modelOther.draw(cmdBuffer);

		modelMain.pushConst(cmdBuffer,
			layout,
			VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
			&otherdata
		);
		modelMain.bind(cmdBuffer);
		modelMain.draw(cmdBuffer);
			// // Drawing
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

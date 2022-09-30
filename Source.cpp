// #include "Instance.hpp"

#include <iostream>
#include "Device.hpp"
#include "Swapchain.hpp"
#include "Pipelines.hpp"
#include "Buffer.hpp"
#include "GraphicsCommands.hpp"
struct SimplePushConstantData
{
	
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

	cow::PipelineLayoutSimpleInfo simpleInfo{};
	simpleInfo.pPushConstantRanges = nullptr;
	simpleInfo.pushConstantCount = 0;
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
	while (!window.shouldClose())
	{
		glfwPollEvents();
		size_t frameIndex = commands.swapchain->getCurrentFrame();
		// Begin Frame
		
		VkCommandBuffer cmdBuffer = commands.begin();
		commands.beginRenderPass(cmdBuffer, value);
		graphicsPipeline.bind(cmdBuffer);
		// Vertex Buffer Start
		std::vector<cow::Vertex2DRGB> vertices;
		
		//std::cout << "sizeof vertex: " << sizeof(cow::Vertex2DRGB) << "\n";
		// Child
		/*
					* 1
		
		
			* 3				* 2
		*/
		constexpr float outside = -2;
		constexpr float to_the_side = 0.50;
		constexpr float shade = 0.71;
		// Main Triangle
		vertices.push_back({ {0.0 + to_the_side, -0.5  }, {1.0 , 0.0, 0.0 } }); // 1
		vertices.push_back({ {0.5 + to_the_side , 0.5  }, {0.0, 1.0 , 0.0 } }); // 2
		vertices.push_back({ {-0.5+ to_the_side , 0.5 }, {0.0 , 0.0 , 1.0 } }); // 3

		// Meeting with this person
		vertices.push_back({ {(0.0 + outside ) + offset, -0.45 }, {0.0 , 0.0 , 1.0  } }); // 1
		vertices.push_back({ {(0.5 + outside) + offset ,0.5 }, {0.0 , 1.0 , 0.0 } }); // 2
		vertices.push_back({ {(-0.5 + outside) + offset , 0.5 }, {1.0 , 0.0 , 0.0 } }); // 3

		if (offset <= 1.5)
		offset += 0.001;
		//colorChange += 0.0002;
		
		if (offset >= 3.0) 
		{
			offset = 0.0;
		}
		/*
		vec2(0.0, -0.5),
		vec2(0.5, 0.5),
		vec2(-0.5, 0.5)
		*/
		uint32_t vertexCount = static_cast<uint32_t>(vertices.size());
		assert(vertexCount >= 3 && "Vertex Count must be atleast 3");
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;
		
		cow::Buffer stagingBuffer{
			device, 
			bufferSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};
		stagingBuffer.map();
		stagingBuffer.write(vertices.data(), bufferSize, 0);
		stagingBuffer.unmap();

		cow::Buffer vertexBuffer{
			device,
			bufferSize,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		};
		vertexBuffer.copy(stagingBuffer.get(), bufferSize);
		// Vertex Buffer End
		// Binding
		VkBuffer pVertexBuffers[] = { vertexBuffer.get() };
		VkDeviceSize offsets[] = {0};
		vkCmdBindVertexBuffers(cmdBuffer, 0, 1, pVertexBuffers, offsets);
		// Drawing
		vkCmdDraw(cmdBuffer, vertexCount, 1, 0, 0);
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

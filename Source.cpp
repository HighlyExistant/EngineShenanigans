// #include "Instance.hpp"

#include <iostream>
#include "CowHeaders.hpp"
#include "Texture.hpp"
#include "Windows.h"
using namespace cow;

struct SimplePushConstantData
{
	glm::vec2 modelvec;
};
float lerp(float a, float b, float t) 
{
	return (1 - t) * a + t * b;
}
class RenderObject 
	: public EmptyObject, 
	public Model2DComponent, 
	public PushConstantComponent<SimplePushConstantData>
{
public:
	RenderObject(Device& device, std::vector<Vertex2DRGB> vertices2d)
		: EmptyObject{ EmptyObject::create() },
		Model2DComponent{ device, vertices2d }
	{}
	RenderObject(Device& device,uint32_t size, Vertex2DRGB *vertices2d)
		: EmptyObject{ EmptyObject::create() },
		Model2DComponent{ device, size, vertices2d }
	{}
	~RenderObject() {}
};

int main()
{
	std::vector<VkCommandBuffer> commandBuffers(Swapchain::MAX_FRAMES);

	std::cout << "\n\n" << sizeof(Swapchain) << "\n\n";
	Window window(800, 500, "name");

	Device device(window, nullptr, 0);

	GraphicsCommands commands{ device };

	GraphicsPipelineSimpleInfo gpsi{};

	VkPushConstantRange pushConstants{};
	pushConstants.offset = 0;
	pushConstants.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	pushConstants.size = sizeof(SimplePushConstantData);
	PipelineLayoutSimpleInfo simpleInfo{};
	simpleInfo.pPushConstantRanges = &pushConstants;
	simpleInfo.pushConstantCount = 1;
	
	VkPipelineLayout layout = defaultPipelineLayout(device.getDevice(), &simpleInfo);
	
	gpsi.pEntry = "main";
	gpsi.pFragpath = "C:\\Users\\anton\\source\\repos\\GPU-VM\\GPU-VM\\Shaders\\simple_shader.frag.spv";
	gpsi.pVertpath = "C:\\Users\\anton\\source\\repos\\GPU-VM\\GPU-VM\\Shaders\\simple_shader.vert.spv";
	gpsi.renderPass = commands.swapchain->getRenderPass();
	gpsi.pipelineLayout = layout;
	

	GraphicsPipelineSimpleInfo::defaultGraphicsPipeline(gpsi);

	GraphicsPipeline<Vertex2DRGB> graphicsPipeline{device, &gpsi };
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

	constexpr float outside = -2;
	constexpr float to_the_side = 0.50;
	constexpr float shade = 0.71;
	/*Vertex2DRGB verticesMain[] =
	{
		{{0.0 + to_the_side, -0.5 }, {1.0 , 0.0, 0.0 }},
		{{0.5 + to_the_side , 0.5 }, {0.0, 1.0 , 0.0 }},
		{{-0.5 + to_the_side , 0.5}, {0.0 , 0.0 , 1.0}}
	};*/
	 std::vector<Vertex2DRGB> verticesMain;
	std::vector<Vertex2DRGB> verticesOther;

	 // Main Triangle
	 verticesMain.push_back({ {0.0 + to_the_side, -0.5  }, {1.0 , 0.0, 0.0 } });
	 verticesMain.push_back({ {0.5 + to_the_side , 0.5  }, {0.0, 1.0 , 0.0 } });
	 verticesMain.push_back({ {-0.5 + to_the_side , 0.5 }, {0.0 , 0.0 , 1.0 } });

	// Meeting with this person
	verticesOther.push_back({ {(0.0 + outside) + offset, -0.45	},	{0.0 , 0.0 , 1.0	} });
	verticesOther.push_back({ {(0.5 + outside) + offset ,	0.5		},	{0.0 , 1.0 , 0.0	} });
	verticesOther.push_back({ {(-0.5 + outside) + offset, 0.5		},	{1.0 , 0.0 , 0.0	} });
	 
	RenderObject modelMain{ device,verticesMain };
	RenderObject modelOther{ device, verticesOther };
	std::vector<RenderObject*> models{};
	models.push_back(&modelMain);
	models.push_back(&modelOther);
	Texture texture{ device, "C:\\Users\\anton\\Downloads\\pic_goes_hard.png" };
	texture.createTextureImageView();
	Sleep(10000);
	while (!window.shouldClose())
	{
		glfwPollEvents();
		size_t frameIndex = commands.swapchain->getCurrentFrame();
		// Begin Frame
		
		VkCommandBuffer cmdBuffer = commands.begin();
		commands.beginRenderPass(cmdBuffer, value);
		graphicsPipeline.bind(cmdBuffer);

		//Drawing
		offset += 0.0001;
		modelOther.push_constant_data.modelvec = { 0.0 + offset, 0.0 };
		modelMain.push_constant_data.modelvec = { 0.0 , 0.0 };

		for (size_t i = 0; i < models.size(); i++)
		{
			models.data()[i]->pushConstant(cmdBuffer, layout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
			models.data()[i]->bind(cmdBuffer);
			models.data()[i]->draw(cmdBuffer);
		}
		if (offset >= 2.0) 
		{
			offset = 0.0;
		}
		
		
		// Ending
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
	}
	vkDestroyPipelineLayout(device.getDevice(), layout, nullptr);
}

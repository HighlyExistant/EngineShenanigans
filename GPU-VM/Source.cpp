// #include "Instance.hpp"

#include <iostream>
#include "CowHeaders.hpp"
#include "Texture.hpp"
#include "Windows.h"
#include "Descriptors.hpp"
using namespace cow;
struct UBO
{
	glm::vec2 offset;
};

struct SimplePushConstantData
{
	glm::vec2 modelvec;
	int index;
};
class RenderObject 
	: public ModelIndexed2DComponent<Vertex2DTextured>,
	public EmptyObject,
	public PushConstantComponent<SimplePushConstantData>
{
public:
	RenderObject(Device& device, std::vector<Vertex2DTextured> vertices2d, std::vector<uint32_t> indices)
		: EmptyObject{ EmptyObject::create() },
		ModelIndexed2DComponent{device, vertices2d, indices }
	{}
	RenderObject(Device& device,uint32_t size, Vertex2DTextured*vertices2d, uint32_t indexCount, uint32_t* indices)
		: EmptyObject{ EmptyObject::create() },
		ModelIndexed2DComponent{device, size, vertices2d , indexCount, indices}
	{}
	~RenderObject() {}
};

int main()
{
	std::vector<VkCommandBuffer> commandBuffers(Swapchain::MAX_FRAMES);

	std::cout << "\n\n" << sizeof(Model2DComponent<Vertex2DTextured, Model2DType::Default>) << "\n\n";
	Window window(600, 400, "name");

	Device device(window, nullptr, 0);

	GraphicsCommands commands{ device };

	std::array<VkDescriptorSetLayoutBinding, 2> ubo_bindings{};
	// MultiPurpose Binding for whatever
	ubo_bindings[0] = { 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1,  VK_SHADER_STAGE_VERTEX_BIT, nullptr };
	// Texture Binding
	ubo_bindings[1] = { 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 3, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr }; // 3 textures
	std::array<VkDescriptorPoolSize, 2> poolSizes{};
	// MultiPurpose Binding for whatever
	poolSizes[0] = { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2 };
	// Texture Binding
	poolSizes[1] = { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2 };

	Descriptor<2, 2> descriptor{ device, ubo_bindings, poolSizes, 2 };
	Buffer descriptorBuffers[2] = 
	{
		{ 
			device,
			sizeof(UBO),
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT 
		},
		{ 
			device,
			sizeof(UBO),
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT 
		}
	};
	//descriptor.init_all(sizeof(UBO));
	Texture texture{ device, "Anna.png" };
	std::cout << "Created Texture 1\n";

	Texture texture2{ device, "olivia.png" };
	std::cout << "Created Texture 2\n";

	Texture texture3{ device, "kel.png" };
	std::cout << "Created Texture 3\n";
	for (size_t i = 0; i < Swapchain::MAX_FRAMES; i++)
	{

		// MultiPurpose Binding for whatever
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = descriptorBuffers[i].get();
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UBO);
		
		std::array<VkDescriptorImageInfo, 3> imageInfo{};
		// Texture Binding
		imageInfo[0].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo[0].imageView = texture.m_imageView;
		imageInfo[0].sampler = texture.m_textureSampler;

		imageInfo[1].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo[1].imageView = texture2.m_imageView;
		imageInfo[1].sampler = texture2.m_textureSampler;

		imageInfo[2].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo[2].imageView = texture3.m_imageView;
		imageInfo[2].sampler = texture3.m_textureSampler;

		std::array<VkWriteDescriptorSet, 2> descriptorWrite{};
		descriptorWrite[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite[0].dstSet = descriptor.getSet(i);
		descriptorWrite[0].dstBinding = 0;
		descriptorWrite[0].dstArrayElement = 0;
		descriptorWrite[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite[0].descriptorCount = 1;
		descriptorWrite[0].pBufferInfo = &bufferInfo;
		descriptorWrite[0].pImageInfo = nullptr; // Optional
		descriptorWrite[0].pTexelBufferView = nullptr; // Optional

		descriptorWrite[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite[1].dstSet = descriptor.getSet(i);
		descriptorWrite[1].dstBinding = 1;
		descriptorWrite[1].dstArrayElement = 0;
		descriptorWrite[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrite[1].descriptorCount = 3;
		descriptorWrite[1].pImageInfo = imageInfo.data();

		vkUpdateDescriptorSets(device.getDevice(), static_cast<uint32_t>(descriptorWrite.size()), descriptorWrite.data(), 0, nullptr);
		std::cout << "Updated Descriptor Sets\n";
	}

	VkPushConstantRange pushConstants{};
	pushConstants.offset = 0;
	pushConstants.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	pushConstants.size = sizeof(SimplePushConstantData);
	
	VkPipelineLayout layout;
	VkPipelineLayoutCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	createInfo.pushConstantRangeCount = 1;
	createInfo.pPushConstantRanges = &pushConstants;
	createInfo.setLayoutCount = 1;
	createInfo.pSetLayouts = descriptor.getLayout();

	if (vkCreatePipelineLayout(device.getDevice(), &createInfo, nullptr, &layout) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create a pipeline layout");
	}
	GraphicsPipelineSimpleInfo gpsi{};
	gpsi.pEntry = "main";
	gpsi.pFragpath = "simple_shader.frag.spv";
	gpsi.pVertpath = "simple_shader.vert.spv";
	gpsi.renderPass = commands.swapchain->getRenderPass();
	gpsi.pipelineLayout = layout;

	GraphicsPipelineSimpleInfo::defaultGraphicsPipeline(gpsi);

	GraphicsPipeline<Vertex2DTextured> graphicsPipeline{device, &gpsi };
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

	constexpr float outside = -1;
	constexpr float to_the_side = 0.50;
	
	std::vector<Vertex2DTextured> verticesMain;
	std::vector<Vertex2DTextured> verticesOther;
	std::vector<Vertex2DTextured> verticesKel;

	// Main Triangle
	// verticesMain.push_back({ {0.0 + to_the_side, -0.5  }, {1.0 , 0.0, 0.0 } });
	// verticesMain.push_back({ {0.5 + to_the_side , 0.5  }, {0.0, 1.0 , 0.0 } });
	// verticesMain.push_back({ {-0.5 + to_the_side , 0.5 }, {0.0 , 0.0 , 1.0 } });
	// 
	// // Meeting with this person
	// verticesOther.push_back({ {(0.0 + outside) + offset, -0.45	},	{0.0 , 0.0 , 1.0	} });
	// verticesOther.push_back({ {(0.5 + outside) + offset ,	0.5		},	{0.0 , 1.0 , 0.0	} });
	// verticesOther.push_back({ {(-0.5 + outside) + offset, 0.5		},	{1.0 , 0.0 , 0.0	} });
	// verticesMain.push_back({ {0.0 + to_the_side, -0.5  }, {1.0 , 0.0 } });
	// verticesMain.push_back({ {0.5 + to_the_side , 0.5  }, {0.0, 1.0 } });
	// verticesMain.push_back({ {-0.5 + to_the_side , 0.5 }, {0.0 , 0.0 } });
	
	// Meeting with this person
	verticesOther.push_back({ {0.3, -0.5}, {1.0 , 0.0} });
	verticesOther.push_back({ {-0.3, 0.5}, {0.0 , 1.0} });
	verticesOther.push_back({ {0.3, 0.5}, {1.0 , 1.0} });
	verticesOther.push_back({ {-0.3, -0.5}, {0.0 , 0.0} });

	verticesMain.push_back({ {0.3 , -0.5}, {1.0 , 0.0} });
	verticesMain.push_back({ {-0.3 , 0.5}, {0.0 , 1.0} });
	verticesMain.push_back({ {0.3 , 0.5}, {1.0 , 1.0} });
	verticesMain.push_back({ {-0.3 , -0.5}, {0.0 , 0.0} });

	verticesKel.push_back({ {0.3 + to_the_side, -0.5}, {1.0 , 0.0} });
	verticesKel.push_back({ {-0.3 + to_the_side, 0.5}, {0.0 , 1.0} });
	verticesKel.push_back({ {0.3 + to_the_side, 0.5}, {1.0 , 1.0} });
	verticesKel.push_back({ {-0.3 + to_the_side, -0.5}, {0.0 , 0.0} });

	std::vector<uint32_t> indices;
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(0);
	indices.push_back(3);
	indices.push_back(1);
	RenderObject modelMain{ device,verticesMain, indices };
	RenderObject modelOther{ device, verticesOther, indices };
	RenderObject modelKel{ device, verticesKel, indices };
	
	std::vector<RenderObject*> models{};
	models.push_back(&modelMain);
	models.push_back(&modelOther);
	models.push_back(&modelKel);

	UBO ubo{};
	float offset_x = 0.0;
	float offset_y = 0.0;
	while (!window.shouldClose())
	{
		glfwPollEvents();
		size_t frameIndex = commands.swapchain->getCurrentFrame();
		uint32_t imageIndex = commands.getCurrentImageIndex();
		// Begin Frame
		
		VkCommandBuffer cmdBuffer = commands.begin();
		commands.beginRenderPass(cmdBuffer, value);
		graphicsPipeline.bind(cmdBuffer);

		//Drawing
		if (window.getKeyState(GLFW_KEY_W, GLFW_PRESS) )
		{
			offset_y -= 0.001;
		}
		if (window.getKeyState(GLFW_KEY_A, GLFW_PRESS) )
		{
			offset_x -= 0.001;
		}
		if (window.getKeyState(GLFW_KEY_S, GLFW_PRESS) )
		{
			offset_y += 0.001;
		}
		if (window.getKeyState(GLFW_KEY_D, GLFW_PRESS))
		{
			offset_x += 0.001;
		}
		modelOther.push_constant_data.modelvec = { offset_x, offset_y };
		modelOther.push_constant_data.index = 1;
		modelMain.push_constant_data.index = 0;
		modelKel.push_constant_data.index = 2;
		// modelMain.push_constant_data.modelvec = { 0.0 , 0.0 };

		descriptorBuffers[frameIndex].map();
		descriptorBuffers[frameIndex].write(&ubo, sizeof(UBO), 0);
		descriptorBuffers[frameIndex].unmap();
		
		vkCmdBindDescriptorSets(cmdBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			layout, 0, 1, &descriptor.descriptorSets[frameIndex],
			0,
			nullptr);

		for (size_t i = 0; i < models.size(); i++)
		{
			models.data()[i]->pushConstant(cmdBuffer, layout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
			models.data()[i]->bind_indexed(cmdBuffer);
			models.data()[i]->draw_indexed(cmdBuffer);
		}
		
		// Ending
		vkCmdEndRenderPass(cmdBuffer);
		if (vkEndCommandBuffer(cmdBuffer) != VK_SUCCESS) 
		{
			throw std::runtime_error("failed to end cmdBuffer");
		}
		uint32_t index = commands.getCurrentImageIndex();
		VkResult result = commands.swapchain->submit(&cmdBuffer, &index);
		// commands.checkRecreation(result);
		
		vkDeviceWaitIdle(device.getDevice());
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window.wasWindowResized())
		{
			window.resetWindowResizedFlag();
			commands.recreateSwapchain();
		}
		else if (result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to present swap chain image");
		}
	}
	vkDestroyPipelineLayout(device.getDevice(), layout, nullptr);
}

#include <iostream>
#include "CowHeaders.hpp"
#include "Texture.hpp"
#include "Windows.h"
#include "Descriptors.hpp"
#include "GraphicsEngine.hpp"
#include <chrono>
#include "cowmath2d.hpp"
using namespace cow;
struct UBO
{
	glm::vec2 offset;
};

struct SimplePushConstantData
{
	glm::mat2 modelvec;
	glm::vec2 offset;
	int index;
};
class RenderObject
	: public ModelIndexed2DComponent<Vertex2DTextured>,
	public PushConstantComponent<SimplePushConstantData>
{
public:
	RenderObject(Device& device, std::vector<Vertex2DTextured> vertices2d, std::vector<uint32_t> indices)
		: ModelIndexed2DComponent{ device, vertices2d, indices }
	{}
	RenderObject(Device& device, uint32_t size, Vertex2DTextured* vertices2d, uint32_t indexCount, uint32_t* indices)
		: ModelIndexed2DComponent{ device, size, vertices2d , indexCount, indices }
	{}
	~RenderObject() {}
};
int main()
{
	std::cout << rotate({4, 4}, 30).y;
	GraphicsEngine engine;

	std::array<VkDescriptorSetLayoutBinding, 2> ubo_bindings{};
	// MultiPurpose Binding for whatever
	ubo_bindings[0] = { 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1,  VK_SHADER_STAGE_VERTEX_BIT, nullptr }; // 1 uniform buffers
	// Texture Binding
	ubo_bindings[1] = { 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 3, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr }; // 3 textures

	std::array<VkDescriptorPoolSize, 2> poolSizes{};
	// MultiPurpose Binding for whatever
	poolSizes[0] = { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2 }; // 2 frames in flight
	// Texture Binding
	poolSizes[1] = { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 6 }; // 2 frames in flight	*	number of textures

	VkDescriptorSetLayout descLayout;
	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(ubo_bindings.size());
	layoutInfo.pBindings = ubo_bindings.data();
	
	if (vkCreateDescriptorSetLayout(engine.device.getDevice(), &layoutInfo, nullptr, &descLayout) != VK_SUCCESS)
	{
		engine.device.m_instance.logger.Log("program stopped at descriptor set layout creation", COW_ERR_TYPE::FAILURE);
		throw std::runtime_error("failed to create descriptor set layout!");
	}
	Descriptor<2, 2> descriptor{ engine.device, descLayout, poolSizes, 2 };

	VkDescriptorSetLayout* descLayouts = (VkDescriptorSetLayout*)calloc(2, sizeof(VkDescriptorSetLayout));
	for (size_t i = 0; i < 2; i++)
	{
		if (descLayouts != nullptr)
		{
			descLayouts[i] = descLayout;
		}
	}
	VkDescriptorSetAllocateInfo dAllocInfo{};
	dAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	dAllocInfo.descriptorPool = descriptor.getPool();
	dAllocInfo.descriptorSetCount = 2;
	dAllocInfo.pSetLayouts = descLayouts;

	DescriptorSets sets{engine.device, 2, &dAllocInfo };
	
	free(descLayouts);
	Buffer descriptorBuffers[2] =
	{
		{
			engine.device,
			sizeof(UBO),
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		},
		{
			engine.device,
			sizeof(UBO),
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		}
	};
	Texture textures[3] = 
	{ 
		{ engine.device, "Anna.png" }, 
		{ engine.device, "sigma.png" }, 
		{ engine.device, "kel.png" } 
	};

	for (size_t i = 0; i < Swapchain::MAX_FRAMES; i++)
	{
		// MultiPurpose Binding for whatever
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = descriptorBuffers[i].get();
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UBO);

		std::array<VkDescriptorImageInfo, 3> imageInfo{};
		// Texture Binding
		for (size_t i = 0; i < imageInfo.size(); i++)
		{
			textures[i].fillImageInfo(&imageInfo[i], VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		}

		std::array<VkWriteDescriptorSet, 2> descriptorWrite{};
		descriptorWrite[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite[0].dstSet = sets.getSet(i);
		descriptorWrite[0].dstBinding = 0;
		descriptorWrite[0].dstArrayElement = 0;
		descriptorWrite[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite[0].descriptorCount = 1;
		descriptorWrite[0].pBufferInfo = &bufferInfo;
		descriptorWrite[0].pImageInfo = nullptr; // Optional
		descriptorWrite[0].pTexelBufferView = nullptr; // Optional

		descriptorWrite[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite[1].dstSet = sets.getSet(i);
		descriptorWrite[1].dstBinding = 1;
		descriptorWrite[1].dstArrayElement = 0;
		descriptorWrite[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrite[1].descriptorCount = 3;
		descriptorWrite[1].pImageInfo = imageInfo.data();

		vkUpdateDescriptorSets(engine.device.getDevice(), static_cast<uint32_t>(descriptorWrite.size()), descriptorWrite.data(), 0, nullptr);
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
	createInfo.pSetLayouts = &descLayout;

	if (vkCreatePipelineLayout(engine.device.getDevice(), &createInfo, nullptr, &layout) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create a pipeline layout");
	}
	GraphicsPipelineSimpleInfo gpsi{};
	gpsi.pEntry = "main";
	gpsi.pFragpath = "simple_shader.frag.spv";
	gpsi.pVertpath = "simple_shader.vert.spv";
	gpsi.renderPass = engine.getRenderPass();
	gpsi.pipelineLayout = layout;

	GraphicsPipelineSimpleInfo::defaultGraphicsPipeline(gpsi);
	GraphicsPipeline<Vertex2DTextured> graphicsPipeline{ engine.device, &gpsi };
	
	float offset = 0.1;
	float colorChange = 0.0;

	VkClearColorValue value{};
	value.float32[0] = 0.1;
	value.float32[1] = 0.1;
	value.float32[2] = 0.1;
	value.float32[3] = 0.1;

	constexpr float outside = -1;
	constexpr float to_the_side = 0.10;
	
	std::vector<Vertex2DTextured> verticesMain;
	std::vector<Vertex2DTextured> dynamicVertices;

	// Meeting with this person

	verticesMain.push_back({ {0.3 , -0.5}, {1.0 , 0.0} });
	verticesMain.push_back({ {-0.3 , 0.5}, {0.0 , 1.0} });
	verticesMain.push_back({ {0.3 , 0.5}, {1.0 , 1.0} });
	verticesMain.push_back({ {-0.3 , -0.5}, {0.0 , 0.0} });

	std::vector<uint32_t> indices;
	std::vector<uint32_t> dynamicIndices;
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(0);
	indices.push_back(3);
	indices.push_back(1);

	RenderObject modelMain{ engine.device,verticesMain, indices };
	RenderObject dynamic{ engine.device,verticesMain, indices };
	
	std::vector<RenderObject*> models{};
	models.push_back(&modelMain);
	models.push_back(&dynamic);

	UBO ubo{};
	float offset_x = 0.0;
	float offset_y = 0.0;
	auto currentTime = std::chrono::high_resolution_clock::now();
	
	bool render = true;
	bool pressed = false;
	bool pressed2 = false;

	float inc = 0;
	while (!engine.window.shouldClose())
	{
		inc += 0.005;
		glfwPollEvents();

		auto newTime = std::chrono::high_resolution_clock::now();
		float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
		currentTime = newTime;

		size_t frameIndex = engine.commands.swapchain->getCurrentFrame();
		uint32_t imageIndex = engine.commands.getCurrentImageIndex();
		// Begin Frame
		
		VkCommandBuffer cmdBuffer = engine.begin();
		graphicsPipeline.bind(cmdBuffer);

		//Drawing
		if (engine.keyPressed(GLFW_KEY_W))
		{
			offset_y -= 1.0 * frameTime;
		}
		if (engine.keyPressed(GLFW_KEY_A))
		{
			offset_x -= 1.0 * frameTime;
		}
		if (engine.keyPressed(GLFW_KEY_S))
		{
			offset_y += 1.0 * frameTime;
		}
		if (engine.keyPressed(GLFW_KEY_D))
		{
			offset_x += 1.0 * frameTime;
		}
		if (engine.window.getMouseState(GLFW_MOUSE_BUTTON_LEFT, GLFW_PRESS))
		{
			if (!pressed) 
			{
				static int adding = 0;
				dynamicVertices.push_back({ {engine.window.getCursorPos().x, engine.window.getCursorPos().y}, {engine.window.getCursorPos().x, engine.window.getCursorPos().y} });
				dynamicIndices.push_back(adding);
				adding++;
				std::cout << "Pressed\n";
			}
			pressed = true;
		}
		else
		{
			pressed = false;
		}
		if (engine.keyPressed(GLFW_KEY_3))
		{
			if (!pressed2) 
			{
			dynamic.rewriteVertices(dynamicVertices.size(), dynamicVertices.data());
			dynamic.rewriteIndices(dynamicIndices.size(), dynamicIndices.data());
			std::cout << "Pressed\n";
			}
			pressed2 = true;
		}
		else 
		{
			pressed2 = false;
		}
		if (engine.keyPressed(GLFW_KEY_0))
			render = false;
		if (engine.keyPressed(GLFW_KEY_1))
			render = true;
		if (engine.keyPressed(GLFW_KEY_X))
		{
			std::cout << "Cursor X Pos: " << engine.window.getCursorPos().x << '\n';
		}
		if (engine.keyPressed(GLFW_KEY_Y))
		{
			std::cout << "Cursor Y Pos: " << engine.window.getCursorPos().y << '\n';
		}

		Transform2DComponent comp{};
		Transform2DComponent comp2{};
		comp.rotation = inc;
		modelMain.push_data.modelvec = { comp.mat2() };
		modelMain.push_data.index = 1;
		modelMain.push_data.offset = { offset_x, offset_y };
		dynamic.push_data.index = 1;
		dynamic.push_data.modelvec = { comp2.mat2() };

		descriptorBuffers[frameIndex].map();
		descriptorBuffers[frameIndex].write(&ubo, sizeof(UBO), 0);
		descriptorBuffers[frameIndex].unmap();

		vkCmdBindDescriptorSets(cmdBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			layout, 0, 1, &sets.get()[frameIndex],
			0,
			nullptr);

		for (size_t i = 0; i < models.size(); i++)
		{
			if (render)
			{
				models[i]->pushConstant(cmdBuffer, layout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
				models[i]->bind_indexed(cmdBuffer);
				models[i]->draw_indexed(cmdBuffer);
			}
		}
		// Ending
		engine.end(cmdBuffer);
		uint32_t index = engine.commands.getCurrentImageIndex();
		VkResult result = engine.submit(&cmdBuffer, &index);
		engine.checkSwapchainRecreation(result);
	}
	vkDestroyPipelineLayout(engine.device.getDevice(), layout, nullptr);
	vkDestroyDescriptorSetLayout(engine.device.getDevice(), descLayout, nullptr);
}
#include <iostream>
#include "CowHeaders.hpp"
#include "Texture.hpp"
#include "Descriptors.hpp"
#include <chrono>
#include "ProjectTypes.h"
#include "cow_math.hpp"
// Around 3400 Lines of code
// This source file will be user stuff
using namespace cow;

int main()
{
	cow::GraphicsEngine engine;

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
	{ // transparent_anya
		{ engine.device, "Images\\olivia.png" }, 
		{ engine.device, "Images\\sigma.png" }, 
		{ engine.device, "Images\\kel.png" } 
	};
	Texture switchtexture[4] = 
	{ 
		{engine.device, "Images\\sigma.png"},
		{engine.device, "Images\\olivia.png"},
		{engine.device, "Images\\kel.png"},
		{engine.device, "Images\\transparent_anya.png"}
	};	// switch out anya with this
	VkDescriptorImageInfo imageInfoSwitch[4]{};
	for (size_t i = 0; i < 4; i++)
	{
		switchtexture[i].fillImageInfo(&imageInfoSwitch[i], VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}
	for (uint32_t i = 0; i < Swapchain::MAX_FRAMES; i++)
	{
		// MultiPurpose Binding for whatever
		std::array<VkDescriptorBufferInfo, 1> bufferInfo{};
		bufferInfo[0].buffer = descriptorBuffers[i].get();
		bufferInfo[0].offset = 0;
		bufferInfo[0].range = sizeof(UBO);

		std::array<VkDescriptorImageInfo, 3> imageInfo{};
		// Texture Binding
		for (size_t i = 0; i < imageInfo.size(); i++)
		{
			textures[i].fillImageInfo(&imageInfo[i], VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		}

		std::array<VkWriteDescriptorSet, 2> descriptorWrite{};
		sets.fillUniformBuffer<1>(&descriptorWrite[0], i, 0, 0, bufferInfo);

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
	gpsi.pFragpath = "Shaders\\SPV\\vertex2dtexturedrgba.frag.spv";
	gpsi.pVertpath = "Shaders\\SPV\\vertex2dtexturedrgba.vert.spv";
	gpsi.renderPass = engine.getRenderPass();
	gpsi.pipelineLayout = layout;

	GraphicsPipelineSimpleInfo::defaultGraphicsPipeline(gpsi);
	gpsi.colorBlendAttachment.blendEnable = VK_TRUE;
	gpsi.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	gpsi.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	
	GraphicsPipeline<Vertex2DTexturedRGBA> graphicsPipeline{ engine.device, &gpsi, 0 };
	gpsi.pFragpath = "Shaders\\SPV\\vertex2drgba.frag.spv";
	gpsi.pVertpath = "Shaders\\SPV\\vertex2drgba.vert.spv";

	GraphicsPipeline<Vertex2DRGBA> UIPipeline{ engine.device, &gpsi, 0 };
	
	std::vector<Vertex2DTexturedRGBA> verticesMain;
	Vertex2DRGBA uiMainVerices[4];
	Vertex2DRGBA::Rect(1.0f, 1.0f, uiMainVerices);
	Vertex2DRGBA::GlobalOpacity(4, uiMainVerices, 0.0f);
	Vertex2DTexturedRGBA vertexRect[4]; 
	Vertex2DTexturedRGBA::Rect(-0.2f, -0.2f, vertexRect);
	// Meeting with this person

	std::vector<uint32_t> indices;
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(0);
	indices.push_back(3);
	indices.push_back(1);

	Player<Vertex2DTexturedRGBA> player1 = { engine, 4, vertexRect, 6, indices.data()};
	RenderObject<Vertex2DRGBA> uiMain{ engine.device, 4, uiMainVerices,6, indices.data() };
	
	RenderGroup<Vertex2DTexturedRGBA> models{ &graphicsPipeline };
	RenderGroup<Vertex2DRGBA> uimodels{ &UIPipeline };
	models.push(&player1);
	uimodels.push(&uiMain);

	UBO ubo{};
	auto currentTime = std::chrono::high_resolution_clock::now();
	float inc = 0;

	bool pressed = false;
	glm::vec2 pos(0.0f);
	int index = 0;
	float clampedFrameTime = 0.f;
	while (!engine.window.shouldClose())
	{
		glfwPollEvents();
		// if (engine.keyPressed(GLFW_KEY_Q) && !pressed)
		{
			for (uint32_t i = 0; i < Swapchain::MAX_FRAMES; i++)
			{
				VkWriteDescriptorSet writer{};

				writer.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				writer.dstSet = sets.getSet(i);
				writer.dstBinding = 1;
				writer.dstArrayElement = 0;
				writer.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				writer.descriptorCount = 1;
				writer.pImageInfo = &imageInfoSwitch[index];
				vkUpdateDescriptorSets(engine.device.getDevice(), 1, &writer, 0, nullptr);
			}
		}
		// else if (!engine.keyPressed(GLFW_KEY_Q) && pressed)
		{
			pressed = false;
		}
		auto newTime = std::chrono::high_resolution_clock::now();
		float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
		clampedFrameTime += frameTime;
		currentTime = newTime;
		if (clampedFrameTime > 1.0 / 1.0)	// framrate 24
		{
			clampedFrameTime = 0.f;
			index++;
		}
		if (index > 3)
		{
			index = 0;
		}
		size_t frameIndex = engine.commands.swapchain->getCurrentFrame();
		uint32_t imageIndex = engine.commands.getCurrentImageIndex();
		// Begin Frame
		
		VkCommandBuffer cmdBuffer = engine.begin(); // Beginning command buffer and render pass
		
		//Drawing
		Transform2DComponent comp{};
		player1.moveController(&pos, 1.0f * frameTime);

		player1.push_data.modelvec = { comp.mat2() };
		player1.push_data.offset = { pos };
		uiMain.push_data.modelvec = { comp.mat2() };
		descriptorBuffers[frameIndex].fast_write(&ubo, sizeof(UBO), 0);

		vkCmdBindDescriptorSets(cmdBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			layout, 0, 1, &sets.get()[frameIndex],
			0,
			nullptr);
		
		models.draw(cmdBuffer, layout);

		uiMain.push_data.offset = { glm::cos(inc) , inc };
		inc += 1.0f * frameTime;

		uimodels.draw(cmdBuffer, layout);
		// Ending
		engine.end(cmdBuffer); // ending command buffer and render pass
		uint32_t index = engine.commands.getCurrentImageIndex();
		VkResult result = engine.submit(&cmdBuffer, &index);
		engine.checkSwapchainRecreation(result);
	}
	vkDestroyPipelineLayout(engine.device.getDevice(), layout, nullptr);
	vkDestroyDescriptorSetLayout(engine.device.getDevice(), descLayout, nullptr);
};

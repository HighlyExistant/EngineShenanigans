#include <iostream>
#include "CowHeaders.hpp"
#include "Texture.hpp"
#include "Windows.h"
#include "Descriptors.hpp"
#include "GraphicsEngine.hpp"
#include <chrono>
#include "cow_types.hpp"
// Around 3400 Lines of code

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
template<typename T>
class RenderObject
	: public Model2DIndexedComponent<T>,
	public PushConstantComponent<SimplePushConstantData>
{
public:
	RenderObject(Device& device, std::vector<T> vertices2d, std::vector<uint32_t> indices)
		: Model2DIndexedComponent<T>{ device, vertices2d, indices }
	{}
	RenderObject(Device& device, uint32_t size, T* vertices2d, uint32_t indexCount, uint32_t* indices)
		: Model2DIndexedComponent<T>{ device, size, vertices2d , indexCount, indices }
	{}
	~RenderObject() {}
	AABB boundBox;
};

int main()
{
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
		{ engine.device, "transparent_anya.png" }, 
		{ engine.device, "sigma.png" }, 
		{ engine.device, "kel.png" } 
	};

	for (size_t i = 0; i < Swapchain::MAX_FRAMES; i++)
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
	gpsi.pFragpath = "simple_shader.frag.spv";
	gpsi.pVertpath = "simple_shader.vert.spv";
	gpsi.renderPass = engine.getRenderPass();
	gpsi.pipelineLayout = layout;

	GraphicsPipelineSimpleInfo::defaultGraphicsPipeline(gpsi);
	gpsi.colorBlendAttachment.blendEnable = VK_TRUE;
	gpsi.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	gpsi.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	
	GraphicsPipeline<Vertex2DTextured> graphicsPipeline{ engine.device, &gpsi, 0 };
	
	gpsi.pFragpath = "Shaders\\vertex2drgba.frag.spv";
	gpsi.pVertpath = "Shaders\\vertex2drgba.vert.spv";

	GraphicsPipeline<Vertex2DRGBA> UIPipeline{ engine.device, &gpsi, 0 };

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
	std::vector<Vertex2DRGBA> uiMainVerices;
	std::vector<Vertex2DRGBA> uiOtherVerices;

	// Meeting with this person

	verticesMain.push_back({ {0.3 , -0.5}, {1.0 , 0.0} });
	verticesMain.push_back({ {-0.3 , 0.5}, {0.0 , 1.0} });
	verticesMain.push_back({ {0.3 , 0.5}, {1.0 , 1.0} });
	verticesMain.push_back({ {-0.3 , -0.5}, {0.0 , 0.0} });

	uiMainVerices.push_back({ {1.0 , -1.0	}, {1.0 , 0.0, 0.0, 0.0} });
	uiMainVerices.push_back({ {-1.0 , 1.0	}, {0.0 , 1.0, 0.0, 1.0} });
	uiMainVerices.push_back({ {1.0 , 1.0	}, {1.0 , 1.0, 0.0, 1.0} });
	uiMainVerices.push_back({ {-1.0 , -1.0	}, {1.0 , 1.0, 1.0, 1.0} });
	
	uiOtherVerices.push_back({ {1.1 , -1.0	}, {1.0 , 1.0, 1.0, 0.5} });
	uiOtherVerices.push_back({ {-1.3 , 1.6	}, {1.0 , 1.0, 1.0, 0.5} });
	uiOtherVerices.push_back({ {1.4 , 1.5	}, {1.0 , 1.0, 1.0, 0.5} });
	uiOtherVerices.push_back({ {-5.0 , -0.0	}, {1.0 , 1.0, 1.0, 0.5} });

	std::vector<uint32_t> indices;
	std::vector<uint32_t> dynamicIndices;
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(0);
	indices.push_back(3);
	indices.push_back(1);

	RenderObject<Vertex2DTextured> modelMain{ engine.device,verticesMain, indices };
	RenderObject<Vertex2DRGBA> uiMain{ engine.device,uiMainVerices, indices };
	RenderObject<Vertex2DRGBA> uiOther{ engine.device,uiOtherVerices, indices };
	
	std::vector<RenderObject<Vertex2DTextured>*> models{};
	std::vector<RenderObject<Vertex2DRGBA>*> uimodels{};
	models.push_back(&modelMain);
	uimodels.push_back(&uiMain);
	uimodels.push_back(&uiOther);

	UBO ubo{};
	auto currentTime = std::chrono::high_resolution_clock::now();
	float inc = 0;
	while (!engine.window.shouldClose())
	{
		glfwPollEvents();

		auto newTime = std::chrono::high_resolution_clock::now();
		float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
		currentTime = newTime;

		size_t frameIndex = engine.commands.swapchain->getCurrentFrame();
		uint32_t imageIndex = engine.commands.getCurrentImageIndex();
		// Begin Frame
		
		VkCommandBuffer cmdBuffer = engine.begin(); // Beginning command buffer and render pass
		
		//Drawing
		Transform2DComponent comp{};
		Transform2DComponent comp2{};

		modelMain.push_data.modelvec = { comp.mat2() };
		uiMain.push_data.modelvec = { comp.mat2() };
		uiOther.push_data.modelvec = { comp.mat2() };
		descriptorBuffers[frameIndex].map();
		descriptorBuffers[frameIndex].write(&ubo, sizeof(UBO), 0);
		descriptorBuffers[frameIndex].unmap();

		vkCmdBindDescriptorSets(cmdBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			layout, 0, 1, &sets.get()[frameIndex],
			0,
			nullptr);
		
		graphicsPipeline.bind(cmdBuffer);

		for (size_t i = 0; i < models.size(); i++)
		{
			models[i]->pushConstant(cmdBuffer, layout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
			models[i]->bind_indexed(cmdBuffer);
			models[i]->draw_indexed(cmdBuffer);
		}

		UIPipeline.bind(cmdBuffer); 
		uiMain.push_data.offset = { glm::cos(inc) , inc };
		uiOther.push_data.offset = { glm::cos(inc) , inc };
		inc += 1.0 * frameTime;

		engine.window.opacity(inc * 0.1);

		std::cout << inc << '\n';
		for (size_t i = 0; i < uimodels.size(); i++)
		{
			uimodels[i]->pushConstant(cmdBuffer, layout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
			uimodels[i]->bind_indexed(cmdBuffer);
			uimodels[i]->draw_indexed(cmdBuffer);
		}
		// Ending
		engine.end(cmdBuffer); // ending command buffer and render pass
		uint32_t index = engine.commands.getCurrentImageIndex();
		VkResult result = engine.submit(&cmdBuffer, &index);
		engine.checkSwapchainRecreation(result);
	}
	vkDestroyPipelineLayout(engine.device.getDevice(), layout, nullptr);
	vkDestroyDescriptorSetLayout(engine.device.getDevice(), descLayout, nullptr);
}
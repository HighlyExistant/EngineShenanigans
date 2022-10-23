#pragma once
#include "cow_types.hpp"
#include "Device.hpp"

namespace cow 
{
	template<typename T>
	class GraphicsPipeline
	{
	public:
		GraphicsPipeline(Device &device, GraphicsPipelineSimpleInfo* pCreateInfo, uint32_t flags)
			: m_ref_device{ device }
		{
			m_vertexShader = createShaderModule(device.getDevice(), pCreateInfo->pVertpath);
			std::cout << "Created ShaderModule 1\n";
			m_fragmentShader = createShaderModule(device.getDevice(), pCreateInfo->pFragpath);
			std::cout << "Created ShaderModules\n";
			VkPipelineShaderStageCreateInfo shaderStages[2]{};
			shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
			shaderStages[0].module = m_vertexShader;
			shaderStages[0].pName = pCreateInfo->pEntry;
			shaderStages[0].flags = 0;
			shaderStages[0].pNext = nullptr;
			shaderStages[0].pSpecializationInfo = nullptr;

			shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
			shaderStages[1].module = m_fragmentShader;
			shaderStages[1].pName = pCreateInfo->pEntry;
			shaderStages[1].flags = 0;
			shaderStages[1].pNext = nullptr;
			shaderStages[1].pSpecializationInfo = nullptr;

			/*
			* pVertexInputState Information to see what information
			* gets passed into the vertex shader
			*/
			auto bindDesc = T::bindingDesc();
			auto attrDesc = T::attributeDesc();
			
			VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
			vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attrDesc.size());
			vertexInputInfo.vertexBindingDescriptionCount = 1;
			vertexInputInfo.pVertexAttributeDescriptions = attrDesc.data();
			vertexInputInfo.pVertexBindingDescriptions = &bindDesc;

			/*
			* Input Assembly
			*/
			VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
			inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;
			inputAssemblyInfo.flags = 0;
			inputAssemblyInfo.pNext = nullptr;
			/*
			* ViewportState info
			*/
			VkPipelineViewportStateCreateInfo viewportInfo{};
			viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			viewportInfo.viewportCount = 1;
			viewportInfo.pViewports = nullptr;
			viewportInfo.scissorCount = 1;
			viewportInfo.pScissors = nullptr;
			/*
			* Rasterization Info
			*/
			VkPipelineRasterizationStateCreateInfo rasterizationInfo{};
			rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			rasterizationInfo.depthClampEnable = VK_FALSE;
			rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
			rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
			rasterizationInfo.lineWidth = 1.0f;
			rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
			rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
			rasterizationInfo.depthBiasEnable = VK_FALSE;
			rasterizationInfo.depthBiasConstantFactor = 0.0f;  // Optional
			rasterizationInfo.depthBiasClamp = 0.0f;           // Optional
			rasterizationInfo.depthBiasSlopeFactor = 0.0f;     // Optional
			/*
			* Graphics Pipeline Creation Info
			*/
			VkGraphicsPipelineCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = flags;
			createInfo.stageCount = 2; // ej. fragment and vertex stage count
			createInfo.pStages = shaderStages;
			createInfo.pVertexInputState = &vertexInputInfo;
			createInfo.pInputAssemblyState = &inputAssemblyInfo;
			createInfo.pTessellationState = nullptr;
			createInfo.pViewportState = &viewportInfo;
			createInfo.pRasterizationState = &rasterizationInfo;
			createInfo.pMultisampleState = &pCreateInfo->multisampleInfo;
			createInfo.pDepthStencilState = &pCreateInfo->depthStencilInfo;
			createInfo.pColorBlendState = &pCreateInfo->colorBlendInfo;
			createInfo.pDynamicState = &pCreateInfo->dynamicStateInfo;
			createInfo.layout = pCreateInfo->pipelineLayout;
			createInfo.renderPass = pCreateInfo->renderPass;
			createInfo.subpass = pCreateInfo->subpass;
			createInfo.basePipelineHandle = nullptr;
			createInfo.basePipelineIndex = -1;

			if (vkCreateGraphicsPipelines(m_ref_device.getDevice(),
				VK_NULL_HANDLE,
				1,
				&createInfo,
				nullptr,
				&m_pipeline
			) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create graphics pipeline");
			}
		}
		
		~GraphicsPipeline()
		{
			vkDestroyShaderModule(m_ref_device.getDevice(), m_vertexShader, nullptr);
			vkDestroyShaderModule(m_ref_device.getDevice(), m_fragmentShader, nullptr);
			vkDestroyPipeline(m_ref_device.getDevice(), m_pipeline, nullptr);
		}

		inline void bind(VkCommandBuffer cmdBuffer) { vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline); }
	
		inline VkPipeline get() { return m_pipeline; }
	private:
		VkShaderModule m_vertexShader;
		VkShaderModule m_fragmentShader;
		VkPipeline m_pipeline;
		Device &m_ref_device;
	};

	class ComputePipeline
	{
	public:
		ComputePipeline(Device& device, VkPipelineLayout layout, const char* filepath, const char* entry);
		~ComputePipeline();
		inline void bind(VkCommandBuffer cmdBuffer) { vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline); }

	private:
		VkPipeline pipeline;
		VkShaderModule m_computeShader;
		Device& m_ref_device;
	};

	ComputePipeline::ComputePipeline(Device &device,VkPipelineLayout layout, const char *filepath, const char* entry)
		: m_ref_device{ device }
	{
		m_computeShader = createShaderModule(device.getDevice(), filepath);
		VkPipelineShaderStageCreateInfo shaderInfo{};
		shaderInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
		shaderInfo.module = m_computeShader;
		shaderInfo.pName = entry;

		VkComputePipelineCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		createInfo.layout = layout;
		createInfo.stage = shaderInfo;

		vkCreateComputePipelines(device.getDevice(), nullptr, 1, &createInfo, nullptr, &pipeline);
	}

	ComputePipeline::~ComputePipeline()
	{
		vkDestroyShaderModule(m_ref_device.getDevice(), m_computeShader, nullptr);
		vkDestroyPipeline(m_ref_device.getDevice(), pipeline, nullptr);
	}
}

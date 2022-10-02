#include "Helper.hpp"
#include <iostream>

namespace cow
{
	// -- --
	VkVertexInputBindingDescription Vertex2D::bindingDesc()
	{
		VkVertexInputBindingDescription r_bindingDesc;
		r_bindingDesc.binding = 0;
		r_bindingDesc.stride = sizeof(Vertex2D);
		r_bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return r_bindingDesc;
	}
	std::vector<VkVertexInputAttributeDescription> Vertex2D::attributeDesc()
	{
		std::vector<VkVertexInputAttributeDescription> r_attrDescs{};
		// The following values will be loaded into the vertex shader
		r_attrDescs.push_back({ 0,0,VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex2D, position) });
		//r_attrDescs.push_back({ 1,0,VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex2DSRGB, color) });
		return r_attrDescs;
	}
	// -- --
	VkVertexInputBindingDescription Vertex2DRGB::bindingDesc()
	{
		VkVertexInputBindingDescription r_bindingDesc;
		r_bindingDesc.binding = 0;
		r_bindingDesc.stride = sizeof(Vertex2DRGB);
		r_bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return r_bindingDesc;
	}
	std::vector<VkVertexInputAttributeDescription> Vertex2DRGB::attributeDesc() 
	{
		std::vector<VkVertexInputAttributeDescription> r_attrDescs{};
		// The following values will be loaded into the vertex shader
		r_attrDescs.push_back({ 0,0,VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex2DRGB, position) });
		r_attrDescs.push_back({ 1,0,VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex2DRGB, color) });
		std::cout << "position: " << offsetof(Vertex2DRGB, position) << '\n';
		std::cout << "color: " << offsetof(Vertex2DRGB, color) << '\n';
		return r_attrDescs;
	}
	// -- --
	VkVertexInputBindingDescription Vertex2DTextured::bindingDesc()
	{
		VkVertexInputBindingDescription r_bindingDesc{};
		r_bindingDesc.binding = 0;
		r_bindingDesc.stride = sizeof(Vertex2DTextured);
		r_bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return r_bindingDesc;
	}
	std::vector<VkVertexInputAttributeDescription> Vertex2DTextured::attributeDesc()
	{
		std::vector<VkVertexInputAttributeDescription> r_attrDescs{};
		r_attrDescs.push_back({ 0,0,VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex2DTextured, position) });
		r_attrDescs.push_back({ 1,0,VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex2DTextured, texture) });
		return r_attrDescs;
	}
#pragma warning( push )
#pragma warning( disable : 4267 6387 6001)
	// -=-=-=-=-=-=-=- Other Functions -=-=-=-=-=-=-=-

	std::tuple<uint32_t, char*> readFile(const char* filepath)
	{
		FILE* file;
		size_t size;
		char* bytes;
		fopen_s(&file, filepath, "rb");
		if (file != nullptr)
		{
			fseek(file, 0, SEEK_END);
			size = ftell(file);
			rewind(file);
			bytes = (char*)malloc(size);
			fread_s(bytes, size, 1, size, file);
		}
		fclose(file);
		return { size, bytes };
	}
#pragma warning( pop )

	// -=-=-=-=-=-=-=- Basic Wrappers -=-=-=-=-=-=-=-
	/*
	* Simple wrapper around vkCreateShaderModule
	 */
	VkShaderModule createShaderModule(VkDevice device, const char* filepath)
	{
		VkShaderModule r_shaderModule;
		auto [size, code] = readFile(filepath);
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = size;
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code);

		if (vkCreateShaderModule(device, &createInfo, nullptr, &r_shaderModule) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create shader module");
		}
		return r_shaderModule;
	}
	/*
	* Simple wrapper around vkCreatePipelineLayout
	 */
	
	void GraphicsPipelineSimpleInfo::defaultGraphicsPipeline(GraphicsPipelineSimpleInfo &pInfo)
	{
		pInfo.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		pInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
		pInfo.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		pInfo.multisampleInfo.minSampleShading = 1.0f;           
		pInfo.multisampleInfo.pSampleMask = nullptr;             
		pInfo.multisampleInfo.alphaToCoverageEnable = VK_FALSE;  
		pInfo.multisampleInfo.alphaToOneEnable = VK_FALSE;       
		
		pInfo.colorBlendAttachment.colorWriteMask =
			VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
			VK_COLOR_COMPONENT_A_BIT;
		pInfo.colorBlendAttachment.blendEnable = VK_FALSE;
		pInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
		pInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
		pInfo.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;              // Optional
		pInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
		pInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
		pInfo.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;              // Optional

		pInfo.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		pInfo.colorBlendInfo.logicOpEnable = VK_FALSE;
		pInfo.colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;  // Optional
		pInfo.colorBlendInfo.attachmentCount = 1;
		pInfo.colorBlendInfo.pAttachments = &pInfo.colorBlendAttachment;
		pInfo.colorBlendInfo.blendConstants[0] = 0.0f;  // Optional
		pInfo.colorBlendInfo.blendConstants[1] = 0.0f;  // Optional
		pInfo.colorBlendInfo.blendConstants[2] = 0.0f;  // Optional
		pInfo.colorBlendInfo.blendConstants[3] = 0.0f;  // Optional

		pInfo.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		pInfo.depthStencilInfo.depthTestEnable = VK_TRUE;
		pInfo.depthStencilInfo.depthWriteEnable = VK_TRUE;
		pInfo.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
		pInfo.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
		pInfo.depthStencilInfo.minDepthBounds = 0.0f;  // Optional
		pInfo.depthStencilInfo.maxDepthBounds = 1.0f;  // Optional
		pInfo.depthStencilInfo.stencilTestEnable = VK_FALSE;
		pInfo.depthStencilInfo.front = {};  // Optional
		pInfo.depthStencilInfo.back = {};   // Optional

		pInfo.dynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
		pInfo.dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		pInfo.dynamicStateInfo.pDynamicStates = pInfo.dynamicStateEnables.data();
		pInfo.dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(pInfo.dynamicStateEnables.size());
		pInfo.dynamicStateInfo.flags = 0;
	}
}

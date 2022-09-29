#pragma once
#include "cowdef.hpp"
#include <tuple>
#include <cstdio>
#include <stdexcept>
#include <glm/glm.hpp>
#include <vector>
namespace cow
{
	enum class GraphicsPipelineType
	{
		SRGB2D
	};
	// -=-=-=-=-=-=-=- Personal Structs -=-=-=-=-=-=-=-
	// * Constructor Info
	struct PipelineLayoutSimpleInfo
	{
		uint32_t pushConstantCount;
		VkPushConstantRange* pPushConstantRanges;
		uint32_t setLayoutCount;
		VkDescriptorSetLayout* pSetLayouts;
	};
	struct GraphicsPipelineSimpleInfo
	{
		const char* pVertpath;
		const char* pFragpath;
		const char* pEntry;

		VkPipelineMultisampleStateCreateInfo multisampleInfo;
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo colorBlendInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
		std::vector<VkDynamicState> dynamicStateEnables;
		VkPipelineDynamicStateCreateInfo dynamicStateInfo;

		VkPipelineLayout pipelineLayout;
		VkRenderPass renderPass;
		uint32_t subpass;

		static void defaultGraphicsPipeline(GraphicsPipelineSimpleInfo& pInfo);
	};
	// * Other
	struct Vertex2DSRGB
	{
		glm::vec2 position;
		alignas(16) glm::vec3 color;

		static VkVertexInputBindingDescription bindingDescription();
		static std::vector<VkVertexInputAttributeDescription> attributeDescriptions();
	};
#pragma warning( push )
#pragma warning( disable : 4267 6387 6001)
	// -=-=-=-=-=-=-=- Other Functions -=-=-=-=-=-=-=-

	std::tuple<uint32_t, char*> readFile(const char* filepath);
#pragma warning( pop )
	// -=-=-=-=-=-=-=- Basic Wrappers -=-=-=-=-=-=-=-
	/*
	* Simple wrapper around vkCreateShaderModule 
	 */
	VkShaderModule createShaderModule(VkDevice device, const char* filepath);
	/*
	* Simple wrapper around vkCreatePipelineLayout
	 */
	VkPipelineLayout defaultPipelineLayout(VkDevice device, PipelineLayoutSimpleInfo* pSimpleInfo);
}

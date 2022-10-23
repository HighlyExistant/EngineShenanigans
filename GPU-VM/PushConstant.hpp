#pragma once
#include "cowdef.hpp"
namespace cow 
{
#pragma warning( push )
#pragma warning( disable : 4042 6011 26495 )
	template<class T>
	struct PushConstantComponent
	{
		T push_data;
		void pushConstant(VkCommandBuffer cmdBuffer, VkPipelineLayout layout, VkShaderStageFlags flags)
		{
			vkCmdPushConstants(
				cmdBuffer,
				layout,
				flags,
				0,
				sizeof(T),
				&push_data
			);
		}
		template<typename N>
		void pushConstantExternal(VkCommandBuffer cmdBuffer, VkPipelineLayout layout, VkShaderStageFlags flags, uint32_t offset,  N *pPushConstant)
		{
			vkCmdPushConstants(
				cmdBuffer,
				layout,
				flags,
				offset,
				sizeof(N),
				pPushConstant
			);
		}
	};
#pragma warning ( pop )
}

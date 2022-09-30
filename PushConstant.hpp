#pragma once
#include "cowdef.hpp"
namespace cow 
{
#pragma warning( push )
#pragma warning( disable : 4042 6011 26495 )
	template<class T>
	class PushConstantComponent
	{
	public:
		PushConstantComponent() {}
		~PushConstantComponent() {}
		void pushConstant(VkCommandBuffer cmdBuffer, VkPipelineLayout layout, VkShaderStageFlags flags)
		{
			vkCmdPushConstants(
				cmdBuffer,
				layout,
				flags,
				0,
				sizeof(T),
				&push_constant_data
			);
		} 
		T push_constant_data;
	private:

	};
#pragma warning ( pop )
}

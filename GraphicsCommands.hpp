#pragma once
#include "Swapchain.hpp"
namespace cow 
{
	class GraphicsCommands
	{
	public:
		GraphicsCommands(Device& device);
		~GraphicsCommands();

		VkCommandBuffer begin();
		void beginRenderPass(VkCommandBuffer commandBuffer);
		inline uint32_t getCurrentImageIndex() { return m_currentImageIndex; }
		std::unique_ptr<Swapchain> swapchain;
	private:
		bool checkRecreation(VkResult result);
		void recreateSwapchain();
		void createCommandBuffers();

		VkCommandBuffer* m_commandBuffers;
		uint32_t m_currentImageIndex;
		/*
		* This class is friended so it also contains:
		* - Device
		* - Window
		*/

		Device& m_ref_device;
	};
}

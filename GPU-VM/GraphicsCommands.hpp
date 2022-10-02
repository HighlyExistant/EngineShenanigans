#pragma once
#include "Swapchain.hpp"
#define GLM_FORCE_RADIANS
#define GLFW_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
namespace cow 
{
	class GraphicsCommands
	{
	public:
		GraphicsCommands(Device& device);
		~GraphicsCommands();

		VkCommandBuffer begin();
		void beginRenderPass(VkCommandBuffer commandBuffer, VkClearColorValue color);
		inline uint32_t getCurrentImageIndex() { return m_currentImageIndex; }
		std::unique_ptr<Swapchain> swapchain;
		bool checkRecreation(VkResult result);
		void recreateSwapchain();
	private:
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

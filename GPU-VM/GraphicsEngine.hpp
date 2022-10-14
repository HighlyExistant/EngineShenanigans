#include "Device.hpp"
#include "Swapchain.hpp"
#include "GraphicsCommands.hpp"
namespace cow 
{
	class GraphicsEngine
	{
	public:
		GraphicsEngine();
		~GraphicsEngine();
		// Graphics Commands Functions
		VkCommandBuffer begin() 
		{
			VkClearColorValue value{};
			value.float32[0] = 0.1;
			value.float32[1] = 0.1;
			value.float32[2] = 0.1;
			value.float32[3] = 0.1;
			VkCommandBuffer cmdbuffer = commands.begin();
			commands.beginRenderPass(cmdbuffer, value);
			return cmdbuffer;
		}
		void end(VkCommandBuffer cmdBuffer)
		{
			vkCmdEndRenderPass(cmdBuffer);
			if (vkEndCommandBuffer(cmdBuffer) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to end cmdBuffer");
			}

		}
		inline VkResult submit(VkCommandBuffer* pCmdBuffer, uint32_t* pIndex) { return commands.swapchain->submit(pCmdBuffer, pIndex); }
		void checkSwapchainRecreation(VkResult result) 
		{
			waitIdle();
			if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window.wasWindowResized())
			{
				window.resetWindowResizedFlag();
				commands.recreateSwapchain();
			}
			else if (result != VK_SUCCESS)
			{
				throw std::runtime_error("failed to present swap chain image");
			}
		}
		// Window Functions
		inline bool keyPressed(int key) { return window.getKeyState(key, GLFW_PRESS); }
		
		template<class Ret, class ...Args> void keyPressedCallback(int key, Ret (*func)(Args...), Args... args)
		{
			if (keyPressed(key)) 
			{
				func(args...);
			}
		}
		// Device Functions
		inline VkResult waitIdle() { return vkDeviceWaitIdle(device.getDevice()); }
		// Getters
		inline VkRenderPass getRenderPass() { return commands.swapchain->getRenderPass(); };
		
		inline VkCommandBuffer * allocCmdBuffers(VkCommandBufferLevel level, uint32_t cmdBufferCount) 
		{
			return device.allocateCommandBuffers(level, cmdBufferCount);
		}
		inline void allocCmdBuffers(VkCommandBufferLevel level, uint32_t cmdBufferCount, VkCommandBuffer *cmdBuffers)
		{
			device.allocateCommandBuffers(level, cmdBufferCount, cmdBuffers);
		}
		
		Window window{ 800, 600, "name" };
		Device device{ window, nullptr, 0 };
		GraphicsCommands commands{ device };
	private:
		
	};

	GraphicsEngine::GraphicsEngine()
	{
	}

	GraphicsEngine::~GraphicsEngine()
	{
	}
}

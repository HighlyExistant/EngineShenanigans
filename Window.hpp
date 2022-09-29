#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <stdexcept>
namespace cow 
{
	class Window
	{
	public:
		Window(int width, int height, const char* name);
		~Window();

		inline bool shouldClose() const { return glfwWindowShouldClose(m_window); }
		inline VkExtent2D getExtend2D() const noexcept { return {static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height)}; }
		
		void createSurface(VkInstance instance, VkSurfaceKHR* surface) const;
	private:
		int m_width;
		int m_height;
		const char* m_name;
		GLFWwindow* m_window;
	};

}

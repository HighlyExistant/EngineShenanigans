#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <stdexcept>
#define GLM_FORCE_RADIANS
#define GLFW_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>


namespace cow 
{
	struct WindowFeatures
	{
		int resizable;
		int transparent;
	};
	class Window
	{
	public:
		Window(int width, int height, const char* name, WindowFeatures features);
		~Window();

		inline bool shouldClose() const { return glfwWindowShouldClose(m_window); }
		inline VkExtent2D getExtend2D() const noexcept { return {static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height)}; }
		static void frameBufferResizeCallback(GLFWwindow* window, int _width, int _height);
		void resetWindowResizedFlag() { frameBufferResized = false; }
		bool wasWindowResized() { return frameBufferResized; }

		void opacity(float opacity);
		void createSurface(VkInstance instance, VkSurfaceKHR* surface) const;
		bool getKeyState(int key, int state);
		bool getMouseState(int key, int state);
		glm::vec2 getCursorPos() 
		{
			double pos[2];
			glfwGetCursorPos(m_window, &pos[0], &pos[1]);
			
			float x = 2.0 * normalize_values(0.0, static_cast<float>(m_width), pos[0]) - 1.0;
			float y = 2.0 * normalize_values(0.0, static_cast<float>(m_height), pos[1]) - 1.0;
			return { x, y };
		}
	private:
		static float normalize_values(float min, float max, float t)
		{
			return (t - min) / (max - min);
		}
		int m_width;
		int m_height;
		bool frameBufferResized = false;
		const char* m_name;
		GLFWwindow* m_window;
	};

}

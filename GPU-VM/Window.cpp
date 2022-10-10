#include "Window.hpp"


namespace cow 
{
	Window::Window(int width, int height, const char* name)
		: m_width{ width },
		m_height{ height },
		m_name{ name }
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_FALSE);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		m_window = glfwCreateWindow(width, height, name, nullptr, nullptr);
	
		glfwSetWindowUserPointer(m_window, this);
		glfwSetFramebufferSizeCallback(m_window, frameBufferResizeCallback);

	}

	Window::~Window()
	{
		glfwDestroyWindow(m_window);
		glfwTerminate();
	}
	void Window::createSurface(VkInstance instance, VkSurfaceKHR* surface)
		const
	{
		VkResult x = glfwCreateWindowSurface(instance, m_window, nullptr, surface);
		if (x != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create window surface");
		}
	}
	bool Window::getKeyState(int key, int state)
	{
		return glfwGetKey(m_window, key) == state;
	}
	bool Window::getMouseState(int key, int state)
	{
		return glfwGetMouseButton(m_window, key) == state;
	}
	void Window::frameBufferResizeCallback(GLFWwindow* _window, int _width, int _height)
	{
		auto window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(_window));
		window->frameBufferResized = true;
		window->m_height = _height;
		window->m_width = _width;
	}
}

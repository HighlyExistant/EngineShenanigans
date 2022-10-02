#include "Instance.hpp"
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include <array>
#include <stdexcept>
namespace cow
{
	Instance::Instance()
	{
		std::array<const char*, 1> validationLayers = { "VK_LAYER_KHRONOS_validation" };

		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.apiVersion = VK_API_VERSION_1_3;
		appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 0, 0, 0);
		appInfo.pApplicationName = "vk_app";
		appInfo.engineVersion = VK_MAKE_API_VERSION(0, 0, 0, 0);
		appInfo.pEngineName = "N/A";

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;
		
		uint32_t glfwExtensionCount = 0;
		const char**  glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		createInfo.enabledExtensionCount = glfwExtensionCount;
		createInfo.ppEnabledExtensionNames = glfwExtensions;

		if (COW_VALIDATION_LAYERS)
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
		}
		else
		{
			createInfo.enabledLayerCount = 0;
			createInfo.pNext = nullptr;
		}
		if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
		{
			throw std::runtime_error("couldn't create VkInstance");
		}
	}
	Instance::~Instance()
	{
		vkDestroyInstance(instance, nullptr);
	}

	std::tuple<uint32_t, COW_FREE_RETURN VkPhysicalDevice*> Instance::enumeratePhysicalDevices()
		const
	{
		uint32_t count;
		vkEnumeratePhysicalDevices(instance, &count, nullptr);
		if (count == 0) return std::make_pair(-1, nullptr);
		VkPhysicalDevice* pDevices = (VkPhysicalDevice*)calloc(count, sizeof(VkPhysicalDevice));

		vkEnumeratePhysicalDevices(instance, &count, pDevices);
		return { count, pDevices };
	}

	std::tuple<uint32_t, COW_FREE_RETURN VkPhysicalDeviceGroupProperties*> Instance::enumeratePhysicalDeviceGroups()
		const
	{
		uint32_t count;
		vkEnumeratePhysicalDeviceGroups(instance, &count, nullptr);
		if (count == 0) return std::make_pair(-1, nullptr);
		VkPhysicalDeviceGroupProperties* pDeviceGroupProps = (VkPhysicalDeviceGroupProperties*)calloc(count, sizeof(VkPhysicalDeviceGroupProperties));

		vkEnumeratePhysicalDeviceGroups(instance, &count, pDeviceGroupProps);
		return { count, pDeviceGroupProps };
	}
	std::tuple<uint32_t, COW_FREE_RETURN VkExtensionProperties*> Instance::enumerateExtensionProperties()
	{
		uint32_t count;
		vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);
		if (count == 0) return std::make_pair(-1, nullptr);
		VkExtensionProperties* pProperties = (VkExtensionProperties*)calloc(count, sizeof(VkExtensionProperties));

		vkEnumerateInstanceExtensionProperties(nullptr, &count, pProperties);
		return { count, pProperties };
	}
}

#pragma once
#include <array>
#include "Device.hpp"
#define GLM_FORCE_RADIANS
#define GLFW_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include "Buffer.hpp"
namespace cow 
{
	template<uint32_t bindingSize, uint32_t poolSize>
	class Descriptor
	{
	public:
		Descriptor(Device &device, const std::array<VkDescriptorSetLayoutBinding, bindingSize> bindings, const std::array<VkDescriptorPoolSize, poolSize> pools, uint32_t descriptorSetCount)
			: m_ref_device{ device }, descriptorSetCount{ descriptorSetCount }
		{
			// Layout Creation
			VkDescriptorSetLayoutCreateInfo layoutInfo{};
			layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			layoutInfo.bindingCount = bindings.size();
			layoutInfo.pBindings = bindings.data();
			if (vkCreateDescriptorSetLayout(device.getDevice(), &layoutInfo, nullptr, &layout) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create descriptor set layout!");
			}
			//// Pool Creation

			VkDescriptorPoolCreateInfo poolInfo{};
			poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			poolInfo.poolSizeCount = pools.size();
			poolInfo.pPoolSizes = pools.data();
			poolInfo.maxSets = descriptorSetCount;
		
			if (vkCreateDescriptorPool(device.getDevice(), &poolInfo, nullptr, &pool) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create descriptor pool!");
			}
			// Set Creation
			VkDescriptorSetLayout* layouts = (VkDescriptorSetLayout*)calloc(descriptorSetCount, sizeof(VkDescriptorSetLayout));
			for (size_t i = 0; i < descriptorSetCount; i++)
			{
				if (layouts != nullptr) 
				{
					layouts[i] = layout;
				}
			}
			
			VkDescriptorSetAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocInfo.descriptorPool = pool;
			allocInfo.descriptorSetCount = descriptorSetCount;
			allocInfo.pSetLayouts = layouts;
			
			descriptorSets = (VkDescriptorSet*)calloc(descriptorSetCount, sizeof(VkDescriptorSet));
			if (vkAllocateDescriptorSets(device.getDevice(), &allocInfo, descriptorSets) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to allocate descriptor sets!");
			}
			free(layouts);
		}
		~Descriptor()
		{
			vkDestroyDescriptorSetLayout(m_ref_device.getDevice(), layout, nullptr);
			vkDestroyDescriptorPool(m_ref_device.getDevice(), pool, nullptr);
		}
		
		inline VkDescriptorPool getPool() { return pool; }
		inline VkDescriptorSetLayout *getLayout() { return &layout; }
		inline VkDescriptorSet getSet(uint32_t index) { return descriptorSets[index]; }
		
		VkDescriptorSet* descriptorSets;
	private:
		VkDescriptorSetLayout layout;
		VkDescriptorPool pool;
		uint32_t descriptorSetCount;

		Device& m_ref_device;

		friend class DescriptorWriter;
	};
}

#pragma once
#include <array>
#include "Device.hpp"
#define GLM_FORCE_RADIANS
#define GLFW_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include "Buffers.hpp"
namespace cow
{
	template<uint32_t bindingSize, uint32_t poolSize>
	class Descriptor
	{
	public:
		Descriptor(Device &device, VkDescriptorSetLayout layout, const std::array<VkDescriptorPoolSize, poolSize> pools, uint32_t descriptorSetCount)
			: m_ref_device{ device }
		{
			// Pool Creation

			VkDescriptorPoolCreateInfo poolInfo{};
			poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			poolInfo.poolSizeCount = static_cast<uint32_t>(pools.size());
			poolInfo.pPoolSizes = pools.data();
			poolInfo.maxSets = descriptorSetCount;

			if (vkCreateDescriptorPool(device.getDevice(), &poolInfo, nullptr, &pool) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create descriptor pool!");
			}
		}
		
		~Descriptor()
		{
			vkDestroyDescriptorPool(m_ref_device.getDevice(), pool, nullptr);
		}
		inline VkDescriptorPool getPool() { return pool; }
	private:
		VkDescriptorPool pool;
		Device& m_ref_device;

		friend class DescriptorWriter;
	};

	class DescriptorSets
	{
	public:
		DescriptorSets(Device &device,uint32_t setCount,  VkDescriptorSetAllocateInfo *pAllocInfo)
		{
			sets = (VkDescriptorSet*)calloc(setCount, sizeof(VkDescriptorSet));
			vkAllocateDescriptorSets(device.getDevice(), pAllocInfo, sets);
		}
		~DescriptorSets() 
		{
			free(sets);
		}
		template<uint32_t N>
		void fillUniformBuffer(VkWriteDescriptorSet *pWriter, uint32_t setIndex, uint32_t binding, uint32_t arrayElement, std::array<VkDescriptorBufferInfo, N> Info)
		{
			pWriter->sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			pWriter->dstSet = getSet(setIndex);
			pWriter->dstBinding = binding;
			pWriter->dstArrayElement = arrayElement;
			pWriter->descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			pWriter->descriptorCount = static_cast<uint32_t>(Info.size());
			pWriter->pBufferInfo = Info.data();
		}
		
		inline VkDescriptorSet getSet(uint32_t index) { return sets[index]; }
		inline VkDescriptorSet *get() { return sets; }
	private:
		VkDescriptorSet* sets;
	};
}

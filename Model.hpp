#pragma once

#include "GraphicsCommands.hpp"
#include "Helper.hpp"
#include "Buffer.hpp"
// Model 2D
// Model 3D
namespace cow 
{
	enum class Model2DType
	{
		Model2D,
		Model2D_Staged,
	};

	// Staged
	class Model2D
	{
	public:
		
		Model2D(Device& device, std::vector<Vertex2DRGB> vertices2d)
			: m_ref_device{ device }, vertexCount{ static_cast<uint32_t>(vertices2d.size()) }
		{
			assert(vertexCount >= 3 && "Vertex Count must be atleast 3");
			VkDeviceSize bufferSize = sizeof(vertices2d[0]) * vertexCount;

			vertexBuffer = std::make_unique<cow::Buffer>(
				device,
				bufferSize,
				VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			);
			vertexBuffer->map();
			vertexBuffer->write(vertices2d.data(), bufferSize, 0);
			vertexBuffer->unmap();
		}
		static inline Model2D createStagedModel2D(Device& device, std::vector<Vertex2DRGB> vertices2d)
		{
			return Model2D(vertices2d, device );
		}
		~Model2D() {}
		void bind(VkCommandBuffer cmdBuffer)
		{
			VkBuffer pVertexBuffers[] = { vertexBuffer->get() };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(cmdBuffer, 0, 1, pVertexBuffers, offsets);
		}
		void draw(VkCommandBuffer cmdBuffer)
		{
			vkCmdDraw(cmdBuffer, vertexCount, 1, 0, 0);
		}
		template<class T>
		void pushConst(VkCommandBuffer cmdBuffer, VkPipelineLayout layout, VkShaderStageFlags flags, T *push)
		{
			vkCmdPushConstants(
				cmdBuffer,
				layout,
				flags,
				0,
				sizeof(T),
				push
			);

		}
	private:
		Model2D(std::vector<Vertex2DRGB> vertices2d, Device& device)
			: m_ref_device{ device }, vertexCount{ static_cast<uint32_t>(vertices2d.size()) }
		{
			assert(vertexCount >= 3 && "Vertex Count must be atleast 3");
			VkDeviceSize bufferSize = sizeof(vertices2d[0]) * vertexCount;

			cow::Buffer stagingBuffer{
				device,
				bufferSize,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
			};
			stagingBuffer.map();
			stagingBuffer.write(vertices2d.data(), bufferSize, 0);
			stagingBuffer.unmap();

			vertexBuffer = std::make_unique<cow::Buffer>(
				device,
				bufferSize,
				VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				);
			vertexBuffer->copy(stagingBuffer.get(), bufferSize);
		}
		glm::vec2 translation;
		glm::vec2 scale;
		glm::vec2 rotation;
		Device& m_ref_device;

		std::unique_ptr<cow::Buffer> vertexBuffer;
		uint32_t vertexCount;
		friend class EmptyObject;
	};
}

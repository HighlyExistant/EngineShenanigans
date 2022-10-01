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
	/*
	* This class holds the vertices in a model. Do not confuse
	* this class for a transform.
	*/
	template<class T>
	class Model2DComponent
	{
	public:
		
		Model2DComponent(Device& device, std::vector<T> vertices2d)
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
		Model2DComponent(Device& device, uint32_t count, T* vertices2d)
			: m_ref_device{ device }, vertexCount{ count }
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
			vertexBuffer->write(vertices2d, bufferSize, 0);
			vertexBuffer->unmap();
		}
		static inline Model2DComponent createStagedModel2D(Device& device, std::vector<T> vertices2d)
		{
			return Model2DComponent(vertices2d, device );
		}
		~Model2DComponent() {}
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
	private:
		Model2DComponent(std::vector<T> vertices2d, Device& device)
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
		Device& m_ref_device;

		std::unique_ptr<cow::Buffer> vertexBuffer;
		uint32_t vertexCount;
		friend class EmptyObject;
	};
}

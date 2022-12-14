#pragma once

#include "GraphicsCommands.hpp"
#include "cow_types.hpp"
#include "Buffer.hpp"
namespace cow 
{
	enum class Model2DType
	{
		Default,
		Staged,
	};
	/*
	* This class holds the vertices in a model. Do not confuse
	* this class for a transform.
	*/
	// * Model2D Default
	template<class T, Model2DType modeltype>
	class Model2DComponent
	{
	public:
		Model2DComponent(Device& device, std::vector<T> vertices2d)
			: m_ref_device{ device }, vertexCount{ static_cast<uint32_t>(vertices2d.size()) }
		{
			Constructor<modeltype>(vertices2d.size(), vertices2d.data());
		}
		Model2DComponent(Device& device, uint32_t count, T* vertices2d)
			: m_ref_device{ device }, vertexCount{ count }
		{
			Constructor<modeltype>(count, vertices2d);
		}
		
		~Model2DComponent() {}

		// -=-=-=-=-=-=- Public Functions -=-=-=-=-=-=-

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

		void rewriteVertices(uint32_t count, T* vertices2d)
		{
			this->vertexCount = count;
			VkDeviceSize bufferSize = sizeof(vertices2d[0]) * vertexCount;

			vertexBuffer.reset();
			vertexBuffer = std::make_unique<cow::Buffer>(
				m_ref_device,
				bufferSize,
				VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				);
			vertexBuffer->map();
			vertexBuffer->write(vertices2d, bufferSize, 0);
			vertexBuffer->unmap();
		}
	private:
		// -=-=-=-=-=-=- Constructor Templates -=-=-=-=-=-=-
		
		/*
		* Vertex Only Constructors
		*/
		
		template<Model2DType N, typename std::enable_if < N == Model2DType::Default> ::type* = nullptr>
		inline void Constructor(uint32_t count, T* vertices2d)
		{
			assert(vertexCount >= 3 && "Vertex Count must be atleast 3");
			VkDeviceSize bufferSize = sizeof(vertices2d[0]) * vertexCount;

			vertexBuffer = std::make_unique<cow::Buffer>(
				m_ref_device,
				bufferSize,
				VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				);
			vertexBuffer->map();
			vertexBuffer->write(vertices2d, bufferSize, 0);
			vertexBuffer->unmap();
		}
		
		template<Model2DType N, typename std::enable_if < N == Model2DType::Staged> ::type* = nullptr>
		inline void Constructor(uint32_t count, T* vertices2d)
		{
			assert(vertexCount >= 3 && "Vertex Count must be atleast 3");
			VkDeviceSize bufferSize = sizeof(vertices2d[0]) * vertexCount;

			cow::Buffer stagingBuffer{
				m_ref_device,
				bufferSize,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
			};
			stagingBuffer.map();
			stagingBuffer.write(vertices2d, bufferSize, 0);
			stagingBuffer.unmap();

			vertexBuffer = std::make_unique<cow::Buffer>(
				m_ref_device,
				bufferSize,
				VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				);
			vertexBuffer->copy(stagingBuffer.get(), bufferSize);
		}

		/*
		* Index Only Constructor
		*/

	protected:
		Device& m_ref_device;

		std::unique_ptr<cow::Buffer> vertexBuffer;
		uint32_t vertexCount;
		friend class EmptyObject;
	};

	template<class T>
	class Model2DIndexedComponent
	{
	public:
		Model2DIndexedComponent(Device& device, std::vector<T> vertices2d, std::vector<uint32_t> indices)
			: vertexCount{ static_cast<uint32_t>(vertices2d.size()) },
			indexCount{ static_cast<uint32_t>(indices.size()) },
			m_ref_device{ device }
		{
			VertexConstructor(vertexCount, vertices2d.data());
			IndexedConstructor(indexCount, indices.data());
		}
		Model2DIndexedComponent(Device& device, uint32_t count, T* vertices2d, uint32_t indexCount, uint32_t* indices)
			: vertexCount{ count },
			indexCount{indexCount},
			m_ref_device{ device }
		{
			VertexConstructor(count, vertices2d);
			IndexedConstructor(indexCount, indices);
		}
		~Model2DIndexedComponent() 
		{
		
		}
		void bind_indexed(VkCommandBuffer cmdBuffer)
		{
			VkBuffer pVertexBuffers[1] = { this->vertexBuffer->get() };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(cmdBuffer, 0, 1, pVertexBuffers, offsets);
			vkCmdBindIndexBuffer(cmdBuffer, indexBuffer->get(), 0, VK_INDEX_TYPE_UINT32);
		}

		void draw_indexed(VkCommandBuffer cmdBuffer)
		{
			vkCmdDrawIndexed(cmdBuffer, indexCount, 1, 0, 0, 0);
		}

		void rewriteIndices(uint32_t indexCount, uint32_t* indices)
		{
			VkDeviceSize indexBufferSize = sizeof(indices[0]) * indexCount;
			this->indexCount = indexCount;

			indexBuffer.reset();
			indexBuffer = std::make_unique<cow::Buffer>(
				this->m_ref_device,
				indexBufferSize,
				VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				);
			indexBuffer->map();
			indexBuffer->write(indices, indexBufferSize, 0);
			indexBuffer->unmap();
		}
	private:
		std::unique_ptr<cow::Buffer> indexBuffer;
		uint32_t indexCount;
		std::unique_ptr<cow::Buffer> vertexBuffer;
		uint32_t vertexCount;
		Device &m_ref_device;

		void IndexedConstructor(uint32_t indexCount, uint32_t* indices)
		{
			VkDeviceSize indexBufferSize = sizeof(indices[0]) * indexCount;


			indexBuffer = std::make_unique<cow::Buffer>(
				this->m_ref_device,
				indexBufferSize,
				VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				);
			indexBuffer->map();
			indexBuffer->write(indices, indexBufferSize, 0);
			indexBuffer->unmap();
		}

		inline void VertexConstructor(uint32_t count, T* vertices2d)
		{
			assert(vertexCount >= 3 && "Vertex Count must be atleast 3");
			VkDeviceSize bufferSize = sizeof(vertices2d[0]) * vertexCount;

			cow::Buffer stagingBuffer{
				m_ref_device,
				bufferSize,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
			};
			stagingBuffer.map();
			stagingBuffer.write(vertices2d, bufferSize, 0);
			stagingBuffer.unmap();

			vertexBuffer = std::make_unique<cow::Buffer>(
				m_ref_device,
				bufferSize,
				VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				);
			vertexBuffer->copy(stagingBuffer.get(), bufferSize);
		}
	};

	glm::vec2 rotate(glm::vec2 vec, float rot)
	{
		return
		{
			vec.x * glm::cos(rot) - vec.y * sin(rot),
			vec.x * glm::sin(rot) + vec.y * cos(rot)
		};
	}
};

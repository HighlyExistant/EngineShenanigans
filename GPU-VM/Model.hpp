#pragma once

#include "GraphicsCommands.hpp"
#include "Helper.hpp"
#include "Buffer.hpp"
// Model 2D
// Model 3D
namespace cow 
{
	struct Empty {};

	// int N, typename std::enable_if <N >= 100> :: type* = nullptr
	enum class Model2DType
	{
		Model2D,
		Model2D_Staged,
		Model2D_Indexed,
		Model2D_Staged_Indexed
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

	private:
		// -=-=-=-=-=-=- Constructor Templates -=-=-=-=-=-=-
		
		/*
		* Vertex Only Constructors
		*/
		
		template<Model2DType N, typename std::enable_if < N == Model2DType::Model2D> ::type* = nullptr>
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
		
		
		template<Model2DType N, typename std::enable_if < N == Model2DType::Model2D_Staged> ::type* = nullptr>
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


		Device& m_ref_device;

		std::unique_ptr<cow::Buffer> vertexBuffer;
		uint32_t vertexCount;
		friend class EmptyObject;
		friend class ModelIndexed2DComponent;
	};
	class ModelIndexed2DComponent
		: protected Model2DComponent<Vertex2DTextured, Model2DType::Model2D_Staged>
	{
	public:
		ModelIndexed2DComponent(Device& device, std::vector<Vertex2DTextured> vertices2d, std::vector<uint32_t> indices)
			: Model2DComponent {  device, vertices2d }, indexCount{ static_cast<uint32_t>(indices.size())}
		{
			IndexedConstructor(indices.size(), indices.data());
		}
		ModelIndexed2DComponent(Device& device, uint32_t count, Vertex2DTextured* vertices2d, uint32_t indexCount, uint32_t* indices)
			: Model2DComponent{ device, count, vertices2d }, indexCount{indexCount}
		{
			IndexedConstructor(indexCount, indices);
		}
		~ModelIndexed2DComponent() 
		{
		
		}
		inline void IndexedConstructor(uint32_t indexCount, uint32_t* indices)
		{
			VkDeviceSize indexBufferSize = sizeof(indices[0]) * indexCount;
			uint32_t indexSize = sizeof(indices[0]);

			indexBuffer = std::make_unique<cow::Buffer>(
				m_ref_device,
				indexBufferSize,
				VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				);
			indexBuffer->map();
			indexBuffer->write(indices, indexBufferSize, 0);
			indexBuffer->unmap();
		}
		void bind_indexed(VkCommandBuffer cmdBuffer)
		{
			VkBuffer pVertexBuffers[] = { vertexBuffer->get() };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(cmdBuffer, 0, 1, pVertexBuffers, offsets);
			vkCmdBindIndexBuffer(cmdBuffer, indexBuffer->get(), 0, VK_INDEX_TYPE_UINT32);
		}

		void draw_indexed(VkCommandBuffer cmdBuffer)
		{
			vkCmdDrawIndexed(cmdBuffer, indexCount, 1, 0, 0, 0);
		}
	private:
		std::unique_ptr<cow::Buffer> indexBuffer;
		uint32_t indexCount;
	};

};

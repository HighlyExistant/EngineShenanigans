#pragma once
#include "cow_types.hpp"
#include "GraphicsEngine.hpp"
#include "Network.hpp"
#include "xml_parser.hpp"
using namespace cow;
struct UBO
{
	glm::vec2 offset;
};

struct SimplePushConstantData
{
	glm::mat2 modelvec;
	glm::vec2 offset;
	int index;
};
template<typename T>
class RenderObject
	: protected Mesh2DIndexedComponent<T, Model2DType::Staged>,
	public PushConstantComponent<SimplePushConstantData>
{
public:
	RenderObject(Device& device, std::vector<T> vertices2d, std::vector<uint32_t> indices)
		: Mesh2DIndexedComponent<T, Model2DType::Staged>{ device, vertices2d, indices }
	{}
	RenderObject(Device& device, uint32_t size, T* vertices2d, uint32_t indexCount, uint32_t* indices)
		: Mesh2DIndexedComponent<T, Model2DType::Staged>{ device, size, vertices2d , indexCount, indices }
	{}
	~RenderObject() {}

	inline void draw(VkCommandBuffer cmdBuffer)
	{

		this->bind_indexed(cmdBuffer);
		this->draw_indexed(cmdBuffer);
	}
	AABB boundBox;
};
template<typename T>
class RenderGroup
{
public:
	RenderGroup(GraphicsPipeline<T>* pPipeline)
		: m_pPipeline{ pPipeline }
	{

	}
	~RenderGroup() {}
	void draw(VkCommandBuffer cmdBuffer, VkPipelineLayout layout)
	{
		m_pPipeline->bind(cmdBuffer);
		for (size_t i = 0; i < obj.size(); i++)
		{
			obj[i]->pushConstant(cmdBuffer, layout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
			obj[i]->draw(cmdBuffer);
		}
	}
	inline void push(RenderObject<T>* object)
	{
		obj.push_back(object);
	}
	RenderObject<T>** data()
	{
		return obj.data();
	}
	size_t size() 
	{
		return obj.size();
	}
private:
	GraphicsPipeline<T>* m_pPipeline;
	std::vector<RenderObject<T>*> obj;
};
template<typename T>
class Player
	: public RenderObject<T>
{
public:
	Player(GraphicsEngine &engine, uint32_t vertexCount, T *vertices2D, uint32_t indexCount, uint32_t *indices)
		: m_ref_engine{ engine },
		RenderObject<T>{ engine.device, vertexCount, vertices2D, indexCount, indices}
	{
	
	}
	~Player() {}

	void moveController(glm::vec2 *pCoords, float roc)	//	WASD 
	{
		if (m_ref_engine.keyPressed(GLFW_KEY_W))
		{
			pCoords->y -= roc;
		}
		if (m_ref_engine.keyPressed(GLFW_KEY_A))
		{
			pCoords->x -= roc;
		}
		if (m_ref_engine.keyPressed(GLFW_KEY_S))
		{
			pCoords->y += roc;
		}
		if (m_ref_engine.keyPressed(GLFW_KEY_D))
		{
			pCoords->x += roc;
		}
	}
private:
	GraphicsEngine& m_ref_engine;
};


#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <ECS/Common.hpp>

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::ECS {
class Coordinator;
class Entity {
public:
	Entity() = delete;
	Entity(const Entity&) = delete;
	Entity(Entity&& a_Other) noexcept
		: _id(std::move(a_Other._id))
		, _coordinator(std::move(a_Other._coordinator))
	{}
	~Entity();
	operator EntityID () const { return _id; }
private:
	friend Coordinator;
	Entity(EntityID a_ID, Coordinator* a_Coordinator)
		: _id(a_ID)
		, _coordinator(a_Coordinator)
	{}
	EntityID _id;
	Coordinator* _coordinator{ nullptr };
};
}

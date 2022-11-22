#include <ECS/Entity.hpp>
#include <ECS/Coordinator.hpp>

TabGraph::ECS::Entity::~Entity()
{
	_coordinator->DestroyEntity(_id);
}

#pragma once

#include <ECS/ComponentManager.hpp>
#include <ECS/EntityManager.hpp>
#include <ECS/SystemManager.hpp>
#include <ECS/Entity.hpp>

namespace TabGraph::ECS {
using EntityPtr = std::shared_ptr<Entity>;
class Coordinator
{
public:	
	Coordinator() {
		// Create pointers to each manager
		_componentManager = std::make_unique<ComponentManager>();
		_entityManager = std::make_unique<EntityManager>();
		_systemManager = std::make_unique<SystemManager>();
	}

	// Entity methods
	auto CreateEntity() {
		return EntityPtr(new Entity(_entityManager->CreateEntity(), this));
	}
	void DestroyEntity(EntityID entity)
	{
		_entityManager->DestroyEntity(entity);
		_componentManager->EntityDestroyed(entity);
		_systemManager->EntityDestroyed(entity);
	}


	// Component methods
	template<typename T>
	void RegisterComponent()
	{
		_componentManager->RegisterComponent<T>();
	}

	template<typename T>
	void AddComponent(EntityID entity, T component)
	{
		_componentManager->AddComponent<T>(entity, component);

		auto signature = _entityManager->GetSignature(entity);
		signature.set(_componentManager->GetComponentType<T>(), true);
		_entityManager->SetSignature(entity, signature);

		_systemManager->EntitySignatureChanged(entity, signature);
	}

	template<typename T>
	void RemoveComponent(EntityID entity)
	{
		_componentManager->RemoveComponent<T>(entity);

		auto signature = _entityManager->GetSignature(entity);
		signature.set(_componentManager->GetComponentType<T>(), false);
		_entityManager->SetSignature(entity, signature);

		_systemManager->EntitySignatureChanged(entity, signature);
	}

	template<typename T>
	T& GetComponent(EntityID entity)
	{
		return _componentManager->GetComponent<T>(entity);
	}

	template<typename T>
	bool HasComponent(EntityID entity)
	{
		return _componentManager->HasComponent<T>(entity);
	}

	template<typename T>
	ComponentType GetComponentType()
	{
		return _componentManager->GetComponentType<T>();
	}


	// System methods
	template<typename T, typename...Args>
	std::shared_ptr<T> RegisterSystem(Args... a_Args)
	{
		return _systemManager->RegisterSystem<T>(a_Args...);
	}

	template<typename T>
	void SetSystemSignature(Signature signature)
	{
		_systemManager->SetSignature<T>(signature);
	}

private:
	std::unique_ptr<ComponentManager> _componentManager;
	std::unique_ptr<EntityManager> _entityManager;
	std::unique_ptr<SystemManager> _systemManager;
};
}
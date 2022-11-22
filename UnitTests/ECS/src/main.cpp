#include <ECS/Coordinator.hpp>

#include <SG/Node/Transform.hpp>
#include <SG/Node/ChildrenList.hpp>

#include <vector>

using namespace TabGraph;

auto CreateNode(ECS::Coordinator& a_ECSCoordinator) {
    auto entity = a_ECSCoordinator.CreateEntity();
    a_ECSCoordinator.AddComponent(*entity, SG::Transform());
    return entity;
}

auto CreateNodeGroup(ECS::Coordinator& a_ECSCoordinator) {
    auto entity = CreateNode(a_ECSCoordinator);
    a_ECSCoordinator.AddComponent(*entity, SG::ChildrenList());
    return entity;
}

struct CullingSystem : ECS::System {
    CullingSystem(ECS::Coordinator* a_ECSCoordinator)
        : _ecsCoordinator(a_ECSCoordinator)
    {}
    void SetSignature() {
        ECS::Signature signature;
        signature.set(_ecsCoordinator->GetComponentType<SG::Transform>());
        signature.set(_ecsCoordinator->GetComponentType<SG::ChildrenList>());
        _ecsCoordinator->SetSystemSignature<CullingSystem>(signature);
    }
    void Update() {
        for (auto const& entity : entities) {
            auto& transform = _ecsCoordinator->GetComponent<SG::Transform>(entity);
            auto& children = _ecsCoordinator->GetComponent<SG::ChildrenList>(entity);

        }
    }
private:
    ECS::Coordinator* const _ecsCoordinator{ nullptr };
};

int main() {
    ECS::Coordinator ecsCoordinator;
    ecsCoordinator.RegisterComponent<SG::Transform>();
    ecsCoordinator.RegisterComponent<SG::ChildrenList>();
    auto cullingSystem = ecsCoordinator.RegisterSystem<CullingSystem>(&ecsCoordinator);
    cullingSystem->SetSignature();
    auto entity = CreateNodeGroup(ecsCoordinator);
    {
        std::vector<ECS::EntityPtr> entities;
        ECS::EntityPtr lastEntity;
        for (auto i = 0u; i < 10; ++i) {
            auto newEntity = CreateNodeGroup(ecsCoordinator);
            if (lastEntity) {
                ecsCoordinator.GetComponent<SG::ChildrenList>(*lastEntity).Insert(newEntity);
            }
            ecsCoordinator.GetComponent<SG::Transform>(*newEntity).position.x = i;
            entities.push_back(newEntity);
            lastEntity = newEntity;
        }
        ecsCoordinator.GetComponent<SG::ChildrenList>(*entity).Insert(lastEntity);
        cullingSystem->Update();
    }
    cullingSystem->Update();
}
#include <ECS/Manager.hpp>

#include <SG/Node/Transform.hpp>
#include <SG/Node/ChildrenList.hpp>

#include <vector>
#include <iostream>

using namespace TabGraph;

auto CreateNode() {
    auto entity = ECS::Manager::CreateEntity();
    ECS::Manager::AddComponent<SG::Transform>(entity);
    return entity;
}

auto CreateNodeGroup() {
    auto entity = CreateNode();
    ECS::Manager::AddComponent<SG::ChildrenList>(entity);
    return entity;
}

struct CullingSystem : ECS::System {
    void SetSignature() {
        ECS::Signature signature;
        signature.set(ECS::Manager::GetComponentType<SG::Transform>());
        signature.set(ECS::Manager::GetComponentType<SG::ChildrenList>());
        ECS::Manager::SetSystemSignature<CullingSystem>(signature);
    }
    void Update() {
        for (auto const& entity : entities) {
            auto& transform = ECS::Manager::GetComponent<SG::Transform>(entity);
            auto& children = ECS::Manager::GetComponent<SG::ChildrenList>(entity);

        }
    }
};

struct Scene {
    ~Scene() {
        for (const auto& entity : entities)
            ECS::Manager::DestroyEntity(entity);
    }
    std::set<ECS::Entity> entities;
};

int main() {
    ECS::Manager::RegisterComponent<SG::Transform>();
    ECS::Manager::RegisterComponent<SG::ChildrenList>();
    auto cullingSystem = ECS::Manager::RegisterSystem<CullingSystem>();
    cullingSystem->SetSignature();
    auto entity = CreateNodeGroup();
    {
        Scene scene;
        bool first = true;
        ECS::Entity lastEntity = 0;
        for (auto i = 0u; i < 10; ++i) {
            auto newEntity = CreateNodeGroup();
            if (!first) {
                ECS::Manager::GetComponent<SG::ChildrenList>(lastEntity)->Insert(newEntity);
            }
            ECS::Manager::GetComponent<SG::Transform>(newEntity)->position.x = i;
            scene.entities.insert(newEntity);
            lastEntity = newEntity;
            first = false;
        }
        //should get 11 entities
        cullingSystem->Update();
    }
    //should get 1 entity
    cullingSystem->Update();
}
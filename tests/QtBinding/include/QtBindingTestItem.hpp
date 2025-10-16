#pragma once

#include <MSG/ECS/Registry.hpp>
#include <MSG/QtItem.hpp>
#include <MSG/Scene.hpp>

#include <memory>
#include <vector>

namespace Msg {
class Texture;

class TestScene : public Scene {
public:
    using Scene::Scene;
    void Init()
    {
        GetFogSettings().globalExtinction = 0;
        SetBackgroundColor({ 0.529, 0.808, 0.922 });
        SetSkybox({ .texture = environment });
        for (auto& entity : meshes)
            AddEntity(entity);
        for (auto& light : lights)
            AddEntity(light);
        UpdateWorldTransforms();
    }
    std::shared_ptr<Texture> environment;
    std::vector<ECS::DefaultRegistry::EntityRefType> meshes;
    std::vector<ECS::DefaultRegistry::EntityRefType> lights;
};

class QtBindingTestItem : public Msg::QtItem {
    Q_OBJECT
public:
    QtBindingTestItem();
    void componentComplete() override;
    static void RegisterQMLType();

protected:
    void updatePolish() override;

private slots:
    void createScene();
    void clearScene();
    void updateCameraProj(const QSize& a_NewSize);

private:
    TestScene _scene;
};
}
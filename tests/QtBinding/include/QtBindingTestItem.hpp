#pragma once

#include <MSG/ECS/Registry.hpp>
#include <MSG/QtItem.hpp>
#include <MSG/Scene.hpp>

#include <QElapsedTimer>

#include <memory>
#include <vector>

namespace Msg {
class Texture;

class TestScene : public Scene {
public:
    using Scene::Scene;
    void Init();
    std::shared_ptr<Texture> environment;
    std::vector<ECS::DefaultRegistry::EntityRefType> meshes;
    std::vector<ECS::DefaultRegistry::EntityRefType> lights;
};

class QtBindingTestItem : public Msg::QtItem {
    Q_OBJECT
    Q_PROPERTY(bool skyboxEnabled MEMBER _skyboxEnabled READ skyboxEnabled WRITE setSkyboxEnabled)
public:
    QtBindingTestItem();
    void componentComplete() override;
    static void RegisterQMLType();
    bool skyboxEnabled() { return _skyboxEnabled; }
    void setSkyboxEnabled(const bool& a_Enabled) { _skyboxEnabled = a_Enabled; }

protected:
    void updatePolish() override;

private slots:
    void createScene();
    void clearScene();
    void updateCameraProj(const QSize& a_NewSize);

private:
    bool _skyboxEnabled = true;
    QElapsedTimer _updateTimer;
    TestScene _scene;
};
}
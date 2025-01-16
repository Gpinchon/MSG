# SceneGraph
This Sub-System defines the components of the ECS relative to the SceneGraph like Cameras, Lights, Meshes
It defines Core objects used to build these components and Entity types, which are aggregates of certain types of components.
It also defines things like Animations, Scenes and Skyboxes which are returned as Objects by the Parser.

# How to use
```cpp
//Create the ECS Registry
auto registry = ECS::DefaultRegistry::Create();
//Create the Scene
SG::Scene scene(registry);
for (int i = 0; i < 5; ++i) {
	//Create a Node Group
	auto node = SG::NodeGroup::Create(registry);
	//Add this node group to the scene
	scene.AddEntity(node);
	for (int j = 0; j < 2; ++j) {
		//Create a terminal node
		auto leaf = SG::Node::Create(registry);
		//Add this node to the Node Group
		SG::Node::SetParent(leaf, node);
	}
}
```
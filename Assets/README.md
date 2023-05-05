# Assets
This sub-system is used to generate a SceneGraph using the ECS sub-system from files.
It contains a list of `TabGraph::Objects` and stores any SceneGraph elements it encounters inside an `ECS::Registry`

# How to use the Assets parser
```cpp
//Initialize the parsers
Assets::InitParsers();
//Create the Asset with correct Path, Asset can also use an URI
auto file = std::make_shared<Assets::Asset>(path);
//Create the ECS Registry and give it to the Asset (Registries can be shared accross Assets)
auto registry = ECS::DefaultRegistry::Create();
file->SetECSRegistry(registry);
//Alternatively, the function AddParsingTask can also be used.
//The future returned by this function can then be waited on.
asset = Assets::Parser::Parse(file);
```

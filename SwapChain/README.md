# Renderer
This Sub-System defines the interface of the Renderer (stored in include) and the implementations (stored in Driver)

# How to use
```cpp
//Create a new window
HWND window = ...
//Create the ECS Registry as usual
auto registry = ECS::DefaultRegistry::Create();
//Create a new Renderer with name UnitTest and version 100
auto renderer = Renderer::Create({ "UnitTest", 100 });
//Create a swapchain using the window
Renderer::CreateSwapChainInfo swapChainInfo;
swapChainInfo.vSync = true; //Activate VSync
swapChainInfo.hwnd = window;
swapChainInfo.width = width;
swapChainInfo.height = height;
swapChainInfo.imageCount = 3; //Triple buffering
swapChain = Renderer::SwapChain::Create(renderer, swapChainInfo);

//Build a scene
SG::Scene testScene(registry, "testScene");

//Create a mesh
auto testCube = SG::Cube::CreateMesh("testCube", { 1, 1, 1 });
//Create a new entity and add the Mesh component to it
auto testEntity = SG::Node::Create(registry);
testEntity.AddComponent<SG::Component::Mesh>(testCube);
//Add the entity to the Scene
testScene.AddEntity(testEntity);

//Create a new Camera
auto testCamera = SG::Camera::Create(registry);
//Offset it by 5 units on all axis
testCamera.GetComponent<SG::Component::Transform>().position = { 5, 5, 5 };
//Look at the cube
SG::Node::LookAt(testCamera, glm::vec3(0));
//Add the camera to the scene
testScene.AddEntity(testCamera);
//Set the camera as the current view point
testScene.SetCamera(testCamera);

//Load the scene into the renderer (this creates the necessary components)
Renderer::Load(renderer, testScene);

//Create the render buffer to which the scene will be rendered
auto renderBuffer = Renderer::RenderBuffer::Create(renderer, { width, height });
while (true) {
	//Poll events and do game logic
	...

    //Render the test scene to the render buffer
    Renderer::Render(renderer, testScene, renderBuffer);
    //Present the result to the window
    Renderer::SwapChain::Present(swapChain, renderBuffer);
    //Update necessary data
    Renderer::Update(renderer);
}
```
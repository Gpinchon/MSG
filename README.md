# TabGraph
*tabGraph* is a full SceneGraph featuring a flexible Component system, giving a lot of freedom for adding new components to your scene and organizing it.

# Dependencies
Dependencies are automatically fetched using CMake.

This project was made with the will to reduce dependencies to what's strictly necessary. Thuse it only depends on :
 - [GLEW][8] for OpenGL because I don't wanna query extensions manually
 - [GLM][1] for vectorial math
 - [SDL2][6] for window system and events
 - [SDL2_Image][7] with CMake
 - [ZLib][3] for SDL2_Image and uncompressing compressed FBX vertex data (soon to be redone)
 - [LibPNG][4] for SDL2_Image
 - [JPEG][5] for SDL2_Image
 - [FasTC][9] for texture compression
 - [GDAL][10] when using advance terrains (soon to be redone)
 - [RapidJSON][2] for parsing GLTF

# TODO List
If you want to participate, here is the [TODO List](/TODO.md)

# Features
*tabGraph* features several functionnalities :
 - Temporal Anti-Aliasing
 - Temporal Accumulation
 - Screen Space Reflection inspired by Unreal Engine
 - Screen Space Ambient Occlusion
 - Smooth Shadows
 - Blended Order-Independant Transparency
 - Physically Based Materials, with MaterialExtensions allowing for various PBR workflows.
 - GPU Rigging
 - Animations

# Screenshots
![Mirror's Edge flat](/screenshots/Capture4.PNG "Mirror's Edge flat")
![Bomberman](/screenshots/Capture0.PNG "Bomberman")
![Buster Drone](/screenshots/Capture1.PNG "Buster Drone")
![Empire of Future](/screenshots/Capture2.PNG "Empire of Future")
![Pigman](/screenshots/Capture3.PNG "Pigman")

[See *tabGraph* library](./TabGraph)

[1]: https://github.com/g-truc/glm.git
[2]: https://github.com/Tencent/rapidjson.git
[3]: https://www.zlib.net
[4]: https://github.com/glennrp/libpng.git
[5]: https://github.com/csparker247/jpeg-cmake.git
[6]: https://github.com/libsdl-org/SDL
[7]: https://github.com/Gpinchon/SDL_image.git
[8]: https://github.com/Perlmint/glew-cmake.git
[9]: https://github.com/GammaUNC/FasTC.git
[10]: http://download.gisinternals.com/sdk/
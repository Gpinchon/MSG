# README

## Top-level Schemas

* [MSGAssets-buffer](./buffer.md) – `buffer.schema.json`

* [MSGAssets-camera](./camera.md) – `camera.schema.json`

* [MSGAssets-entity](./entity.md) – `entity.schema.json`

* [MSGAssets-external](./external.md) – `external.schema.json`

* [MSGAssets-fog](./fog.md) – `fog.schema.json`

* [MSGAssets-image](./image.md) – `image.schema.json`

* [MSGAssets-light](./light.md) – `light.schema.json`

* [MSGAssets-material](./material.md) – `material.schema.json`

* [MSGAssets-mesh](./mesh.md) – `mesh.schema.json`

* [MSGAssets-pixelFormat](./pixelformat.md) – `pixelFormat.schema.json`

* [MSGAssets-sampler](./sampler.md) – `sampler.schema.json`

* [MSGAssets-scene](./scene.md) – `scene.schema.json`

* [MSGAssets-shape](./shape.md) – `shape.schema.json`

* [MSGAssets-specs](./specs.md "this describes the structure of a MSGAssets file") – `specs.schema.json`

* [MSGAssets-texture](./texture.md) – `texture.schema.json`

* [MSGAssets-tonemapping](./tonemapping.md) – `tonemapping.schema.json`

* [MSGAssets-transform](./transform.md "defines the transform component") – `transform.schema.json`

* [MSGAssets-vec](./vec.md) – `vec.schema.json`

## Other Schemas

### Objects

* [AutoExposureSettings](./tonemapping-autoexposuresettings.md) – `tonemapping.schema.json#/autoExposureSettings`

* [BloomSettings](./tonemapping-definitions-bloomsettings.md) – `tonemapping.schema.json#/definitions/bloomSettings`

* [Buffer](./buffer-definitions-buffer.md) – `buffer.schema.json#/definitions/buffer`

* [BufferView](./buffer-definitions-bufferview.md) – `buffer.schema.json#/definitions/bufferView`

* [Camera](./camera-definitions-camera.md "a camera to see the scene through") – `camera.schema.json#/definitions/camera`

* [CameraProjection](./camera-definitions-cameraprojection.md) – `camera.schema.json#/definitions/cameraProjection`

* [CameraProjectionOrthographic](./camera-definitions-cameraprojectionorthographic.md) – `camera.schema.json#/definitions/cameraProjectionOrthographic`

* [CameraProjectionPerspective](./camera-definitions-cameraprojectionperspective.md) – `camera.schema.json#/definitions/cameraProjectionPerspective`

* [CameraProjectionPerspectiveInfinite](./camera-definitions-cameraprojectionperspectiveinfinite.md) – `camera.schema.json#/definitions/cameraProjectionPerspectiveInfinite`

* [CameraSettings](./camera-definitions-camerasettings.md) – `camera.schema.json#/definitions/cameraSettings`

* [ColorGradingSettings](./tonemapping-colorgradingsettings.md) – `tonemapping.schema.json#/colorGradingSettings`

* [CubeData](./shape-definitions-cubedata.md) – `shape.schema.json#/definitions/cubeData`

* [Entity](./entity-definitions-entity.md) – `entity.schema.json#/definitions/entity`

* [External](./external-definitions-external.md) – `external.schema.json#/definitions/external`

* [FogArea](./fog-definitions-fogarea.md "a compination of fog shapes") – `fog.schema.json#/definitions/fogArea`

* [LightData](./camera-definitions-lightdata.md) – `camera.schema.json#/definitions/cameraProjectionData`

* [LightData](./light-definitions-lightdata.md) – `light.schema.json#/definitions/lightData`

* [Material](./material-definitions-material.md) – `material.schema.json#/definitions/material`

* [MaterialBaseExtension](./material-definitions-materialbaseextension.md "this material's base extension") – `material.schema.json#/definitions/baseExtension`

* [MaterialMetallicRoughnessExtension](./material-definitions-materialmetallicroughnessextension.md "this allows for the metallic roughness workflow") – `material.schema.json#/definitions/metallicRoughnessExtension`

* [MaterialNormalTextureInfo](./material-definitions-materialnormaltextureinfo.md "the normal map texture info") – `material.schema.json#/definitions/normalTextureInfo`

* [MaterialOcclusionTextureInfo](./material-definitions-materialocclusiontextureinfo.md "the occlusion texture info") – `material.schema.json#/definitions/occlusionTextureInfo`

* [MaterialSpecularGlossinessExtension](./material-definitions-materialspecularglossinessextension.md "this allows for the specular glossiness workflow, this extension takes priority over metallic roughness if both are present") – `material.schema.json#/definitions/specularGlossinessExtension`

* [MaterialTextureInfo](./material-definitions-materialtextureinfo.md) – `material.schema.json#/definitions/textureInfo`

* [Mesh](./mesh-definitions-mesh.md) – `mesh.schema.json#/definitions/mesh`

* [MeshLod](./mesh-definitions-meshlod.md) – `mesh.schema.json#/definitions/meshLod`

* [MeshPrimitive](./mesh-definitions-meshprimitive.md) – `mesh.schema.json#/definitions/primitive`

* [MeshSkin](./mesh-definitions-meshskin.md "a skin that can be added to an entity with a mesh") – `mesh.schema.json#/definitions/skin`

* [PunctualLight](./light-definitions-punctuallight.md "describes a punctual light, the type of the data property is defined by the value of the type property") – `light.schema.json#/definitions/punctualLight`

* [ShadowSettings](./light-definitions-shadowsettings.md) – `light.schema.json#/definitions/shadowSettings`

* [Shape](./shape-definitions-shape.md) – `shape.schema.json#/definitions/shape`

* [ShapeData](./shape-definitions-shapedata.md) – `shape.schema.json#/definitions/shapeData`

* [SphereData](./shape-definitions-spheredata.md) – `shape.schema.json#/definitions/sphereData`

* [ToneMappingSettings](./tonemapping-tonemappingsettings.md) – `tonemapping.schema.json#/toneMappingSettings`

* [Transform](./transform-definitions-transform.md) – `transform.schema.json#/definitions/transform`

* [Untitled object in MSGAssets-image](./image-definitions-image.md "an image if a set of pixels that can be 3 dimensional, it's untyped as the typing comes from Texture") – `image.schema.json#/definitions/image`

* [Untitled object in MSGAssets-mesh](./mesh-definitions-meshlod-properties-primitives.md "a list of primitives with their corresponding material index inside the linked material set") – `mesh.schema.json#/definitions/meshLod/properties/primitives`

* [Untitled object in MSGAssets-sampler](./sampler-definitions-sampler.md) – `sampler.schema.json#/definitions/sampler`

* [Untitled object in MSGAssets-scene](./scene-definitions-scene.md) – `scene.schema.json#/definitions/scene`

* [Untitled object in MSGAssets-shape](./shape-definitions-shapecombinationshape-items.md) – `shape.schema.json#/definitions/shapeCombinationShape/items`

* [Untitled object in MSGAssets-texture](./texture-definitions-texture.md "a texture is a set of images each describing a mipmap level") – `texture.schema.json#/definitions/texture`

### Arrays

* [ShapeCombinationShape](./shape-definitions-shapecombinationshape.md "the shapes building this combination") – `shape.schema.json#/definitions/shapeCombinationShape`

* [Untitled array in MSGAssets-entity](./entity-definitions-entity-properties-materialset.md "a set of materials taken from materials list") – `entity.schema.json#/definitions/entity/properties/materialSet`

* [Untitled array in MSGAssets-light](./light-definitions-imagebasedlightdata-properties-irradiancecoefficients.md "the irradiance coefficients generated using spherical harmonics to get the IBL diffuse color, if absent, generated on the fly using specular texture") – `light.schema.json#/definitions/IBLData/properties/irradianceCoefficients`

* [Untitled array in MSGAssets-mesh](./mesh-definitions-meshprimitive-oneof-fromproperties-properties-hastexcoords.md "set to true if this primitive has the corresponding set of texture coordinates") – `mesh.schema.json#/definitions/primitive/oneOf/0/properties/hasTexCoords`

* [Untitled array in MSGAssets-mesh](./mesh-definitions-mesh-properties-lods.md) – `mesh.schema.json#/definitions/mesh/properties/lods`

* [Untitled array in MSGAssets-mesh](./mesh-definitions-meshskin-properties-joints.md) – `mesh.schema.json#/definitions/skin/properties/joints`

* [Untitled array in MSGAssets-sampler](./sampler-definitions-sampler-properties-wrapmodes.md) – `sampler.schema.json#/definitions/sampler/properties/wrapModes`

* [Untitled array in MSGAssets-scene](./scene-definitions-scene-properties-entities.md "a list of index to query from the entities list to add to this scene's root entity children") – `scene.schema.json#/definitions/scene/properties/entities`

* [Untitled array in MSGAssets-specs](./specs-properties-externals.md "the path to external resources to load prior to loading this file") – `specs.schema.json#/properties/externals`

* [Untitled array in MSGAssets-specs](./specs-properties-buffers.md "a list of buffers") – `specs.schema.json#/properties/buffers`

* [Untitled array in MSGAssets-specs](./specs-properties-bufferviews.md "a list of bufferViews") – `specs.schema.json#/properties/bufferViews`

* [Untitled array in MSGAssets-specs](./specs-properties-images.md "a list of images") – `specs.schema.json#/properties/images`

* [Untitled array in MSGAssets-specs](./specs-properties-textures.md "a list of textures") – `specs.schema.json#/properties/textures`

* [Untitled array in MSGAssets-specs](./specs-properties-samplers.md "a list of samplers") – `specs.schema.json#/properties/samplers`

* [Untitled array in MSGAssets-specs](./specs-properties-materials.md "a list of materials") – `specs.schema.json#/properties/materials`

* [Untitled array in MSGAssets-specs](./specs-properties-primitive.md "a list of primitives") – `specs.schema.json#/properties/primitive`

* [Untitled array in MSGAssets-specs](./specs-properties-entities.md "a list of entities with their components") – `specs.schema.json#/properties/entities`

* [Untitled array in MSGAssets-specs](./specs-properties-scenes.md "a list of scenes") – `specs.schema.json#/properties/scenes`

* [Untitled array in MSGAssets-texture](./texture-definitions-texture-oneof-fromproperties-properties-images.md "the list of images comprising this texture's lods") – `texture.schema.json#/definitions/texture/oneOf/0/properties/images`

* [Untitled array in MSGAssets-vec](./vec-definitions-vec2.md "a vector of 2 floats") – `vec.schema.json#/definitions/vec2`

* [Untitled array in MSGAssets-vec](./vec-definitions-vec3.md "a vector of 3 floats") – `vec.schema.json#/definitions/vec3`

* [Untitled array in MSGAssets-vec](./vec-definitions-vec4.md "a vector of 4 floats") – `vec.schema.json#/definitions/vec4`

## Version Note

The schemas linked above follow the JSON Schema Spec version: `https://json-schema.org/draft/2020-12/schema#`

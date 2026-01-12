# Untitled object in MSGAssets-scene Schema

```txt
scene.schema.json#/definitions/scene/properties/skybox
```

an object containing an index to the texture and the sampler, sampler is optional

| Abstract            | Extensible | Status         | Identifiable | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                                |
| :------------------ | :--------- | :------------- | :----------- | :---------------- | :-------------------- | :------------------ | :------------------------------------------------------------------------ |
| Can be instantiated | No         | Unknown status | No           | Forbidden         | Allowed               | none                | [scene.schema.json\*](json/defs/scene.schema.json "open original schema") |

## skybox Type

`object` ([Details](scene-definitions-scene-properties-skybox.md))

# skybox Properties

| Property            | Type      | Required | Nullable       | Defined by                                                                                                                                                     |
| :------------------ | :-------- | :------- | :------------- | :------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| [texture](#texture) | `integer` | Required | cannot be null | [MSGAssets-scene](scene-definitions-scene-properties-skybox-properties-texture.md "scene.schema.json#/definitions/scene/properties/skybox/properties/texture") |
| [sampler](#sampler) | `integer` | Optional | cannot be null | [MSGAssets-scene](scene-definitions-scene-properties-skybox-properties-sampler.md "scene.schema.json#/definitions/scene/properties/skybox/properties/sampler") |

## texture

an index into the textures list, can point to a cubemap or an equirectangular texture

`texture`

* is required

* Type: `integer`

* cannot be null

* defined in: [MSGAssets-scene](scene-definitions-scene-properties-skybox-properties-texture.md "scene.schema.json#/definitions/scene/properties/skybox/properties/texture")

### texture Type

`integer`

## sampler

an index into the samplers list

`sampler`

* is optional

* Type: `integer`

* cannot be null

* defined in: [MSGAssets-scene](scene-definitions-scene-properties-skybox-properties-sampler.md "scene.schema.json#/definitions/scene/properties/skybox/properties/sampler")

### sampler Type

`integer`

# Untitled object in MSGAssets-scene Schema

```txt
scene.schema.json#/definitions/scene
```



| Abstract            | Extensible | Status         | Identifiable | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                                |
| :------------------ | :--------- | :------------- | :----------- | :---------------- | :-------------------- | :------------------ | :------------------------------------------------------------------------ |
| Can be instantiated | No         | Unknown status | No           | Forbidden         | Allowed               | none                | [scene.schema.json\*](json/defs/scene.schema.json "open original schema") |

## scene Type

`object` ([Details](scene-definitions-scene.md))

# scene Properties

| Property                            | Type     | Required | Nullable       | Defined by                                                                                                                   |
| :---------------------------------- | :------- | :------- | :------------- | :--------------------------------------------------------------------------------------------------------------------------- |
| [name](#name)                       | `string` | Optional | cannot be null | [MSGAssets-scene](scene-definitions-scene-properties-name.md "scene.schema.json#/definitions/scene/properties/name")         |
| [backgroundColor](#backgroundcolor) | `array`  | Optional | cannot be null | [MSGAssets-scene](vec-definitions-vec3.md "scene.schema.json#/definitions/scene/properties/backgroundColor")                 |
| [skybox](#skybox)                   | `object` | Optional | cannot be null | [MSGAssets-scene](scene-definitions-scene-properties-skybox.md "scene.schema.json#/definitions/scene/properties/skybox")     |
| [fogSettings](#fogsettings)         | `object` | Optional | cannot be null | [MSGAssets-scene](fog-definitions-fogsettings.md "scene.schema.json#/definitions/scene/properties/fogSettings")              |
| [camera](#camera)                   | `string` | Optional | cannot be null | [MSGAssets-scene](scene-definitions-scene-properties-camera.md "scene.schema.json#/definitions/scene/properties/camera")     |
| [entities](#entities)               | `array`  | Optional | cannot be null | [MSGAssets-scene](scene-definitions-scene-properties-entities.md "scene.schema.json#/definitions/scene/properties/entities") |
| [uri](#uri)                         | `string` | Optional | cannot be null | [MSGAssets-scene](external-definitions-uri.md "scene.schema.json#/definitions/scene/properties/uri")                         |

## name

this scene's name, it's important as it can be used to link it from a parent assets file, required if uri is not present

`name`

* is optional

* Type: `string`

* cannot be null

* defined in: [MSGAssets-scene](scene-definitions-scene-properties-name.md "scene.schema.json#/definitions/scene/properties/name")

### name Type

`string`

## backgroundColor

the color of the background when no skybox is selected

`backgroundColor`

* is optional

* Type: `number[]`

* cannot be null

* defined in: [MSGAssets-scene](vec-definitions-vec3.md "scene.schema.json#/definitions/scene/properties/backgroundColor")

### backgroundColor Type

`number[]`

### backgroundColor Constraints

**maximum number of items**: the maximum number of items for this array is: `3`

**minimum number of items**: the minimum number of items for this array is: `3`

### backgroundColor Default Value

The default value is:

```json
[
  0,
  0,
  0,
  1
]
```

## skybox

an object containing an index to the texture and the sampler, sampler is optional

`skybox`

* is optional

* Type: `object` ([Details](scene-definitions-scene-properties-skybox.md))

* cannot be null

* defined in: [MSGAssets-scene](scene-definitions-scene-properties-skybox.md "scene.schema.json#/definitions/scene/properties/skybox")

### skybox Type

`object` ([Details](scene-definitions-scene-properties-skybox.md))

## fogSettings



`fogSettings`

* is optional

* Type: `object` ([FogSettings](fog-definitions-fogsettings.md))

* cannot be null

* defined in: [MSGAssets-scene](fog-definitions-fogsettings.md "scene.schema.json#/definitions/scene/properties/fogSettings")

### fogSettings Type

`object` ([FogSettings](fog-definitions-fogsettings.md))

## camera

the name of the entity with a camera component this scene will be seen through

`camera`

* is optional

* Type: `string`

* cannot be null

* defined in: [MSGAssets-scene](scene-definitions-scene-properties-camera.md "scene.schema.json#/definitions/scene/properties/camera")

### camera Type

`string`

## entities

a list of index to query from the entities list to add to this scene's root entity children

`entities`

* is optional

* Type: `number[]`

* cannot be null

* defined in: [MSGAssets-scene](scene-definitions-scene-properties-entities.md "scene.schema.json#/definitions/scene/properties/entities")

### entities Type

`number[]`

## uri

links to an object loaded from an external resource. To load from the externals list, use the "external" scheme then the external's name followed by the object name as a query

`uri`

* is optional

* Type: `string` ([URI](external-definitions-uri.md))

* cannot be null

* defined in: [MSGAssets-scene](external-definitions-uri.md "scene.schema.json#/definitions/scene/properties/uri")

### uri Type

`string` ([URI](external-definitions-uri.md))

### uri Constraints

**URI**: the string must be a URI, according to [RFC 3986](https://tools.ietf.org/html/rfc3986 "check the specification")

### uri Examples

```json
"external:ExternalName?ObjectName"
```

```json
"file:path/to/file/filename.fileExtension"
```

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

| Property              | Type     | Required | Nullable       | Defined by                                                                                                                   |
| :-------------------- | :------- | :------- | :------------- | :--------------------------------------------------------------------------------------------------------------------------- |
| [name](#name)         | `string` | Required | cannot be null | [MSGAssets-scene](scene-definitions-scene-properties-name.md "scene.schema.json#/definitions/scene/properties/name")         |
| [entities](#entities) | `array`  | Optional | cannot be null | [MSGAssets-scene](scene-definitions-scene-properties-entities.md "scene.schema.json#/definitions/scene/properties/entities") |
| [uri](#uri)           | `string` | Optional | cannot be null | [MSGAssets-scene](external-definitions-uri.md "scene.schema.json#/definitions/scene/properties/uri")                         |

## name

this scene's name, it's important as it can be used to link it from a parent assets file

`name`

* is required

* Type: `string`

* cannot be null

* defined in: [MSGAssets-scene](scene-definitions-scene-properties-name.md "scene.schema.json#/definitions/scene/properties/name")

### name Type

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

links to an object loaded from an external resource. To load from the externals list, use the "external" scheme then the external's name followed by the object name as a query, example: "external:ExternalName?ObjectName"

`uri`

* is optional

* Type: `string` ([URI](external-definitions-uri.md))

* cannot be null

* defined in: [MSGAssets-scene](external-definitions-uri.md "scene.schema.json#/definitions/scene/properties/uri")

### uri Type

`string` ([URI](external-definitions-uri.md))

### uri Constraints

**URI**: the string must be a URI, according to [RFC 3986](https://tools.ietf.org/html/rfc3986 "check the specification")

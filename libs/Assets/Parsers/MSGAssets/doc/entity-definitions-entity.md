# Entity Schema

```txt
entity.schema.json#/definitions/entity
```



| Abstract            | Extensible | Status         | Identifiable | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                                  |
| :------------------ | :--------- | :------------- | :----------- | :---------------- | :-------------------- | :------------------ | :-------------------------------------------------------------------------- |
| Can be instantiated | No         | Unknown status | No           | Forbidden         | Allowed               | none                | [entity.schema.json\*](json/defs/entity.schema.json "open original schema") |

## entity Type

`object` ([Entity](entity-definitions-entity.md))

# entity Properties

| Property                        | Type      | Required | Nullable       | Defined by                                                                                                                              |
| :------------------------------ | :-------- | :------- | :------------- | :-------------------------------------------------------------------------------------------------------------------------------------- |
| [name](#name)                   | `string`  | Required | cannot be null | [MSGAssets-entity](entity-definitions-entity-properties-name.md "entity.schema.json#/definitions/entity/properties/name")               |
| [punctualLight](#punctuallight) | `object`  | Optional | cannot be null | [MSGAssets-entity](light-definitions-punctuallight.md "entity.schema.json#/definitions/entity/properties/punctualLight")                |
| [mesh](#mesh)                   | `object`  | Optional | cannot be null | [MSGAssets-entity](mesh-definitions-mesh.md "entity.schema.json#/definitions/entity/properties/mesh")                                   |
| [skin](#skin)                   | `object`  | Optional | cannot be null | [MSGAssets-entity](mesh-definitions-meshskin.md "entity.schema.json#/definitions/entity/properties/skin")                               |
| [transform](#transform)         | `object`  | Optional | cannot be null | [MSGAssets-entity](transform-definitions-transform.md "entity.schema.json#/definitions/entity/properties/transform")                    |
| [camera](#camera)               | `object`  | Optional | cannot be null | [MSGAssets-entity](camera-definitions-camera.md "entity.schema.json#/definitions/entity/properties/camera")                             |
| [fogArea](#fogarea)             | `object`  | Optional | cannot be null | [MSGAssets-entity](fog-definitions-fogarea.md "entity.schema.json#/definitions/entity/properties/fogArea")                              |
| [materialSet](#materialset)     | `array`   | Optional | cannot be null | [MSGAssets-entity](entity-definitions-entity-properties-materialset.md "entity.schema.json#/definitions/entity/properties/materialSet") |
| [parent](#parent)               | `integer` | Optional | cannot be null | [MSGAssets-entity](entity-definitions-entity-properties-parent.md "entity.schema.json#/definitions/entity/properties/parent")           |

## name

this entity's name

`name`

* is required

* Type: `string`

* cannot be null

* defined in: [MSGAssets-entity](entity-definitions-entity-properties-name.md "entity.schema.json#/definitions/entity/properties/name")

### name Type

`string`

## punctualLight

describes a punctual light, the type of the data property is defined by the value of the type property

`punctualLight`

* is optional

* Type: `object` ([PunctualLight](light-definitions-punctuallight.md))

* cannot be null

* defined in: [MSGAssets-entity](light-definitions-punctuallight.md "entity.schema.json#/definitions/entity/properties/punctualLight")

### punctualLight Type

`object` ([PunctualLight](light-definitions-punctuallight.md))

## mesh



`mesh`

* is optional

* Type: `object` ([Mesh](mesh-definitions-mesh.md))

* cannot be null

* defined in: [MSGAssets-entity](mesh-definitions-mesh.md "entity.schema.json#/definitions/entity/properties/mesh")

### mesh Type

`object` ([Mesh](mesh-definitions-mesh.md))

## skin

a skin that can be added to an entity with a mesh

`skin`

* is optional

* Type: `object` ([MeshSkin](mesh-definitions-meshskin.md))

* cannot be null

* defined in: [MSGAssets-entity](mesh-definitions-meshskin.md "entity.schema.json#/definitions/entity/properties/skin")

### skin Type

`object` ([MeshSkin](mesh-definitions-meshskin.md))

## transform



`transform`

* is optional

* Type: `object` ([Transform](transform-definitions-transform.md))

* cannot be null

* defined in: [MSGAssets-entity](transform-definitions-transform.md "entity.schema.json#/definitions/entity/properties/transform")

### transform Type

`object` ([Transform](transform-definitions-transform.md))

## camera

a camera to see the scene through

`camera`

* is optional

* Type: `object` ([Camera](camera-definitions-camera.md))

* cannot be null

* defined in: [MSGAssets-entity](camera-definitions-camera.md "entity.schema.json#/definitions/entity/properties/camera")

### camera Type

`object` ([Camera](camera-definitions-camera.md))

## fogArea

a compination of fog shapes

`fogArea`

* is optional

* Type: `object` ([FogArea](fog-definitions-fogarea.md))

* cannot be null

* defined in: [MSGAssets-entity](fog-definitions-fogarea.md "entity.schema.json#/definitions/entity/properties/fogArea")

### fogArea Type

`object` ([FogArea](fog-definitions-fogarea.md))

## materialSet

a set of materials taken from materials list

`materialSet`

* is optional

* Type: `integer[]`

* cannot be null

* defined in: [MSGAssets-entity](entity-definitions-entity-properties-materialset.md "entity.schema.json#/definitions/entity/properties/materialSet")

### materialSet Type

`integer[]`

## parent

an index to query into entities list

`parent`

* is optional

* Type: `integer`

* cannot be null

* defined in: [MSGAssets-entity](entity-definitions-entity-properties-parent.md "entity.schema.json#/definitions/entity/properties/parent")

### parent Type

`integer`

### parent Constraints

**minimum**: the value of this number must greater than or equal to: `0`

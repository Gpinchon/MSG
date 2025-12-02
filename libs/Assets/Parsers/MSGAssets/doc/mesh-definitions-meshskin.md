# MeshSkin Schema

```txt
mesh.schema.json#/definitions/skin
```

a skin that can be added to an entity with a mesh

| Abstract            | Extensible | Status         | Identifiable | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                              |
| :------------------ | :--------- | :------------- | :----------- | :---------------- | :-------------------- | :------------------ | :---------------------------------------------------------------------- |
| Can be instantiated | No         | Unknown status | No           | Forbidden         | Allowed               | none                | [mesh.schema.json\*](json/defs/mesh.schema.json "open original schema") |

## skin Type

`object` ([MeshSkin](mesh-definitions-meshskin.md))

# skin Properties

| Property                                    | Type     | Required | Nullable       | Defined by                                                                                                                                        |
| :------------------------------------------ | :------- | :------- | :------------- | :------------------------------------------------------------------------------------------------------------------------------------------------ |
| [copyFrom](#copyfrom)                       | `string` | Optional | cannot be null | [MSGAssets-mesh](external-definitions-fromentity.md "mesh.schema.json#/definitions/skin/properties/copyFrom")                                     |
| [jointsRadius](#jointsradius)               | `number` | Optional | cannot be null | [MSGAssets-mesh](mesh-definitions-meshskin-properties-jointsradius.md "mesh.schema.json#/definitions/skin/properties/jointsRadius")               |
| [joints](#joints)                           | `array`  | Optional | cannot be null | [MSGAssets-mesh](mesh-definitions-meshskin-properties-joints.md "mesh.schema.json#/definitions/skin/properties/joints")                           |
| [inverseBindMatrices](#inversebindmatrices) | `number` | Optional | cannot be null | [MSGAssets-mesh](mesh-definitions-meshskin-properties-inversebindmatrices.md "mesh.schema.json#/definitions/skin/properties/inverseBindMatrices") |

## copyFrom

the name of the entity to copy the component from

`copyFrom`

* is optional

* Type: `string` ([FromEntity](external-definitions-fromentity.md))

* cannot be null

* defined in: [MSGAssets-mesh](external-definitions-fromentity.md "mesh.schema.json#/definitions/skin/properties/copyFrom")

### copyFrom Type

`string` ([FromEntity](external-definitions-fromentity.md))

## jointsRadius

the radius of joints used for BV calculation

`jointsRadius`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-mesh](mesh-definitions-meshskin-properties-jointsradius.md "mesh.schema.json#/definitions/skin/properties/jointsRadius")

### jointsRadius Type

`number`

### jointsRadius Default Value

The default value is:

```json
0.1
```

## joints



`joints`

* is optional

* Type: `number[]`

* cannot be null

* defined in: [MSGAssets-mesh](mesh-definitions-meshskin-properties-joints.md "mesh.schema.json#/definitions/skin/properties/joints")

### joints Type

`number[]`

## inverseBindMatrices

an index into the bufferview list

`inverseBindMatrices`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-mesh](mesh-definitions-meshskin-properties-inversebindmatrices.md "mesh.schema.json#/definitions/skin/properties/inverseBindMatrices")

### inverseBindMatrices Type

`number`

### inverseBindMatrices Constraints

**minimum**: the value of this number must greater than or equal to: `0`

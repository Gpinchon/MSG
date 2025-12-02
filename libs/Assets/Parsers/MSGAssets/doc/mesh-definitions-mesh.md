# Mesh Schema

```txt
mesh.schema.json#/definitions/mesh
```



| Abstract            | Extensible | Status         | Identifiable | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                              |
| :------------------ | :--------- | :------------- | :----------- | :---------------- | :-------------------- | :------------------ | :---------------------------------------------------------------------- |
| Can be instantiated | No         | Unknown status | No           | Forbidden         | Allowed               | none                | [mesh.schema.json\*](json/defs/mesh.schema.json "open original schema") |

## mesh Type

`object` ([Mesh](mesh-definitions-mesh.md))

# mesh Properties

| Property              | Type     | Required | Nullable       | Defined by                                                                                                      |
| :-------------------- | :------- | :------- | :------------- | :-------------------------------------------------------------------------------------------------------------- |
| [copyFrom](#copyfrom) | `string` | Optional | cannot be null | [MSGAssets-mesh](external-definitions-fromentity.md "mesh.schema.json#/definitions/mesh/properties/copyFrom")   |
| [lods](#lods)         | `array`  | Optional | cannot be null | [MSGAssets-mesh](mesh-definitions-mesh-properties-lods.md "mesh.schema.json#/definitions/mesh/properties/lods") |

## copyFrom

the name of the entity to copy the component from

`copyFrom`

* is optional

* Type: `string` ([FromEntity](external-definitions-fromentity.md))

* cannot be null

* defined in: [MSGAssets-mesh](external-definitions-fromentity.md "mesh.schema.json#/definitions/mesh/properties/copyFrom")

### copyFrom Type

`string` ([FromEntity](external-definitions-fromentity.md))

## lods



`lods`

* is optional

* Type: `object[]` ([MeshLod](mesh-definitions-meshlod.md))

* cannot be null

* defined in: [MSGAssets-mesh](mesh-definitions-mesh-properties-lods.md "mesh.schema.json#/definitions/mesh/properties/lods")

### lods Type

`object[]` ([MeshLod](mesh-definitions-meshlod.md))

### lods Constraints

**minimum number of items**: the minimum number of items for this array is: `1`

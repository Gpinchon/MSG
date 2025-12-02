# MeshLod Schema

```txt
mesh.schema.json#/definitions/meshLod
```



| Abstract            | Extensible | Status         | Identifiable | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                              |
| :------------------ | :--------- | :------------- | :----------- | :---------------- | :-------------------- | :------------------ | :---------------------------------------------------------------------- |
| Can be instantiated | No         | Unknown status | No           | Forbidden         | Allowed               | none                | [mesh.schema.json\*](json/defs/mesh.schema.json "open original schema") |

## meshLod Type

`object` ([MeshLod](mesh-definitions-meshlod.md))

# meshLod Properties

| Property                          | Type     | Required | Nullable       | Defined by                                                                                                                                |
| :-------------------------------- | :------- | :------- | :------------- | :---------------------------------------------------------------------------------------------------------------------------------------- |
| [screenCoverage](#screencoverage) | `number` | Required | cannot be null | [MSGAssets-mesh](mesh-definitions-meshlod-properties-screencoverage.md "mesh.schema.json#/definitions/meshLod/properties/screenCoverage") |
| [primitives](#primitives)         | `object` | Required | cannot be null | [MSGAssets-mesh](mesh-definitions-meshlod-properties-primitives.md "mesh.schema.json#/definitions/meshLod/properties/primitives")         |

## screenCoverage

the screen coverage of this LOD

`screenCoverage`

* is required

* Type: `number`

* cannot be null

* defined in: [MSGAssets-mesh](mesh-definitions-meshlod-properties-screencoverage.md "mesh.schema.json#/definitions/meshLod/properties/screenCoverage")

### screenCoverage Type

`number`

## primitives

a list of primitives with their corresponding material index inside the linked material set

`primitives`

* is required

* Type: `object` ([Details](mesh-definitions-meshlod-properties-primitives.md))

* cannot be null

* defined in: [MSGAssets-mesh](mesh-definitions-meshlod-properties-primitives.md "mesh.schema.json#/definitions/meshLod/properties/primitives")

### primitives Type

`object` ([Details](mesh-definitions-meshlod-properties-primitives.md))

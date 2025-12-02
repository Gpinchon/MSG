# Untitled object in MSGAssets-mesh Schema

```txt
mesh.schema.json#/definitions/meshLod/properties/primitives
```

a list of primitives with their corresponding material index inside the linked material set

| Abstract            | Extensible | Status         | Identifiable | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                              |
| :------------------ | :--------- | :------------- | :----------- | :---------------- | :-------------------- | :------------------ | :---------------------------------------------------------------------- |
| Can be instantiated | No         | Unknown status | No           | Forbidden         | Allowed               | none                | [mesh.schema.json\*](json/defs/mesh.schema.json "open original schema") |

## primitives Type

`object` ([Details](mesh-definitions-meshlod-properties-primitives.md))

# primitives Properties

| Property                        | Type      | Required | Nullable       | Defined by                                                                                                                                                                          |
| :------------------------------ | :-------- | :------- | :------------- | :---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| [primitive](#primitive)         | `integer` | Required | cannot be null | [MSGAssets-mesh](mesh-definitions-meshlod-properties-primitives-properties-primitive.md "mesh.schema.json#/definitions/meshLod/properties/primitives/properties/primitive")         |
| [materialIndex](#materialindex) | `integer` | Required | cannot be null | [MSGAssets-mesh](mesh-definitions-meshlod-properties-primitives-properties-materialindex.md "mesh.schema.json#/definitions/meshLod/properties/primitives/properties/materialIndex") |

## primitive

the index of a primitive to query from the primitives list

`primitive`

* is required

* Type: `integer`

* cannot be null

* defined in: [MSGAssets-mesh](mesh-definitions-meshlod-properties-primitives-properties-primitive.md "mesh.schema.json#/definitions/meshLod/properties/primitives/properties/primitive")

### primitive Type

`integer`

### primitive Constraints

**minimum**: the value of this number must greater than or equal to: `0`

## materialIndex

the index of the material inside the entity's material set

`materialIndex`

* is required

* Type: `integer`

* cannot be null

* defined in: [MSGAssets-mesh](mesh-definitions-meshlod-properties-primitives-properties-materialindex.md "mesh.schema.json#/definitions/meshLod/properties/primitives/properties/materialIndex")

### materialIndex Type

`integer`

### materialIndex Constraints

**minimum**: the value of this number must greater than or equal to: `0`

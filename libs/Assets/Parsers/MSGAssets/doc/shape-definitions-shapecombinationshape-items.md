# Untitled object in MSGAssets-shape Schema

```txt
shape.schema.json#/definitions/shapeCombinationShape/items
```



| Abstract            | Extensible | Status         | Identifiable | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                                |
| :------------------ | :--------- | :------------- | :----------- | :---------------- | :-------------------- | :------------------ | :------------------------------------------------------------------------ |
| Can be instantiated | No         | Unknown status | No           | Forbidden         | Allowed               | none                | [shape.schema.json\*](json/defs/shape.schema.json "open original schema") |

## items Type

`object` ([Details](shape-definitions-shapecombinationshape-items.md))

# items Properties

| Property        | Type     | Required | Nullable       | Defined by                                                                                                                                                                   |
| :-------------- | :------- | :------- | :------------- | :--------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| [op](#op)       | `string` | Optional | cannot be null | [MSGAssets-shape](shape-definitions-shapecombinationshape-items-properties-shapecombinationop.md "shape.schema.json#/definitions/shapeCombinationShape/items/properties/op") |
| [shape](#shape) | `object` | Required | cannot be null | [MSGAssets-shape](shape-definitions-shape.md "shape.schema.json#/definitions/shapeCombinationShape/items/properties/shape")                                                  |

## op



`op`

* is optional

* Type: `string` ([ShapeCombinationOp](shape-definitions-shapecombinationshape-items-properties-shapecombinationop.md))

* cannot be null

* defined in: [MSGAssets-shape](shape-definitions-shapecombinationshape-items-properties-shapecombinationop.md "shape.schema.json#/definitions/shapeCombinationShape/items/properties/op")

### op Type

`string` ([ShapeCombinationOp](shape-definitions-shapecombinationshape-items-properties-shapecombinationop.md))

### op Constraints

**enum**: the value of this property must be equal to one of the following values:

| Value         | Explanation |
| :------------ | :---------- |
| `"Add"`       |             |
| `"Substract"` |             |
| `"Intersect"` |             |
| `"Xor"`       |             |

### op Default Value

The default value is:

```json
"Add"
```

## shape



`shape`

* is required

* Type: `object` ([Shape](shape-definitions-shape.md))

* cannot be null

* defined in: [MSGAssets-shape](shape-definitions-shape.md "shape.schema.json#/definitions/shapeCombinationShape/items/properties/shape")

### shape Type

`object` ([Shape](shape-definitions-shape.md))

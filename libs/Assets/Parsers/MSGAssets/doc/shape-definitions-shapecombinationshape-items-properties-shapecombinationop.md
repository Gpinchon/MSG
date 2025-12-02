# ShapeCombinationOp Schema

```txt
shape.schema.json#/definitions/shapeCombinationShape/items/properties/op
```



| Abstract            | Extensible | Status         | Identifiable            | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                                |
| :------------------ | :--------- | :------------- | :---------------------- | :---------------- | :-------------------- | :------------------ | :------------------------------------------------------------------------ |
| Can be instantiated | No         | Unknown status | Unknown identifiability | Forbidden         | Allowed               | none                | [shape.schema.json\*](json/defs/shape.schema.json "open original schema") |

## op Type

`string` ([ShapeCombinationOp](shape-definitions-shapecombinationshape-items-properties-shapecombinationop.md))

## op Constraints

**enum**: the value of this property must be equal to one of the following values:

| Value         | Explanation |
| :------------ | :---------- |
| `"Add"`       |             |
| `"Substract"` |             |
| `"Intersect"` |             |
| `"Xor"`       |             |

## op Default Value

The default value is:

```json
"Add"
```

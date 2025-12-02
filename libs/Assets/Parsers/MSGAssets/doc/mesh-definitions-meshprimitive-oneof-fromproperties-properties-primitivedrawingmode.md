# PrimitiveDrawingMode Schema

```txt
mesh.schema.json#/definitions/primitive/oneOf/0/properties/drawingMode
```



| Abstract            | Extensible | Status         | Identifiable            | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                              |
| :------------------ | :--------- | :------------- | :---------------------- | :---------------- | :-------------------- | :------------------ | :---------------------------------------------------------------------- |
| Can be instantiated | No         | Unknown status | Unknown identifiability | Forbidden         | Allowed               | none                | [mesh.schema.json\*](json/defs/mesh.schema.json "open original schema") |

## drawingMode Type

`string` ([PrimitiveDrawingMode](mesh-definitions-meshprimitive-oneof-fromproperties-properties-primitivedrawingmode.md))

## drawingMode Constraints

**enum**: the value of this property must be equal to one of the following values:

| Value             | Explanation |
| :---------------- | :---------- |
| `"Points"`        |             |
| `"Lines"`         |             |
| `"LineStrip"`     |             |
| `"LineLoop"`      |             |
| `"Polygon"`       |             |
| `"Triangles"`     |             |
| `"TriangleStrip"` |             |
| `"TriangleFan"`   |             |
| `"Quads"`         |             |
| `"QuadStrip"`     |             |

## drawingMode Default Value

The default value is:

```json
"Triangles"
```

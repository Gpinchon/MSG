# Shape Schema

```txt
shape.schema.json#/definitions/shape
```



| Abstract            | Extensible | Status         | Identifiable | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                                |
| :------------------ | :--------- | :------------- | :----------- | :---------------- | :-------------------- | :------------------ | :------------------------------------------------------------------------ |
| Can be instantiated | No         | Unknown status | No           | Forbidden         | Allowed               | none                | [shape.schema.json\*](json/defs/shape.schema.json "open original schema") |

## shape Type

`object` ([Shape](shape-definitions-shape.md))

# shape Properties

| Property      | Type     | Required | Nullable       | Defined by                                                                                               |
| :------------ | :------- | :------- | :------------- | :------------------------------------------------------------------------------------------------------- |
| [type](#type) | `string` | Required | cannot be null | [MSGAssets-shape](shape-definitions-shapetype.md "shape.schema.json#/definitions/shape/properties/type") |
| [data](#data) | Merged   | Required | cannot be null | [MSGAssets-shape](shape-definitions-shapedata.md "shape.schema.json#/definitions/shape/properties/data") |

## type



`type`

* is required

* Type: `string` ([ShapeType](shape-definitions-shapetype.md))

* cannot be null

* defined in: [MSGAssets-shape](shape-definitions-shapetype.md "shape.schema.json#/definitions/shape/properties/type")

### type Type

`string` ([ShapeType](shape-definitions-shapetype.md))

### type Constraints

**enum**: the value of this property must be equal to one of the following values:

| Value      | Explanation |
| :--------- | :---------- |
| `"Cube"`   |             |
| `"Sphere"` |             |

## data



`data`

* is required

* Type: `object` ([ShapeData](shape-definitions-shapedata.md))

* cannot be null

* defined in: [MSGAssets-shape](shape-definitions-shapedata.md "shape.schema.json#/definitions/shape/properties/data")

### data Type

`object` ([ShapeData](shape-definitions-shapedata.md))

one (and only one) of

* [CubeData](shape-definitions-cubedata.md "check type definition")

* [SphereData](shape-definitions-spheredata.md "check type definition")

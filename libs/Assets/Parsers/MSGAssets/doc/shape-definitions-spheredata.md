# SphereData Schema

```txt
shape.schema.json#/definitions/sphereData
```



| Abstract            | Extensible | Status         | Identifiable | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                                |
| :------------------ | :--------- | :------------- | :----------- | :---------------- | :-------------------- | :------------------ | :------------------------------------------------------------------------ |
| Can be instantiated | No         | Unknown status | No           | Forbidden         | Allowed               | none                | [shape.schema.json\*](json/defs/shape.schema.json "open original schema") |

## sphereData Type

`object` ([SphereData](shape-definitions-spheredata.md))

# sphereData Properties

| Property          | Type     | Required | Nullable       | Defined by                                                                                                                         |
| :---------------- | :------- | :------- | :------------- | :--------------------------------------------------------------------------------------------------------------------------------- |
| [center](#center) | `array`  | Required | cannot be null | [MSGAssets-shape](vec-definitions-vec3.md "shape.schema.json#/definitions/sphereData/properties/center")                           |
| [radius](#radius) | `number` | Required | cannot be null | [MSGAssets-shape](shape-definitions-spheredata-properties-radius.md "shape.schema.json#/definitions/sphereData/properties/radius") |

## center

a vector of 3 floats

`center`

* is required

* Type: `number[]`

* cannot be null

* defined in: [MSGAssets-shape](vec-definitions-vec3.md "shape.schema.json#/definitions/sphereData/properties/center")

### center Type

`number[]`

### center Constraints

**maximum number of items**: the maximum number of items for this array is: `3`

**minimum number of items**: the minimum number of items for this array is: `3`

## radius

the local radius of this shape

`radius`

* is required

* Type: `number`

* cannot be null

* defined in: [MSGAssets-shape](shape-definitions-spheredata-properties-radius.md "shape.schema.json#/definitions/sphereData/properties/radius")

### radius Type

`number`

### radius Constraints

**minimum**: the value of this number must greater than or equal to: `0`

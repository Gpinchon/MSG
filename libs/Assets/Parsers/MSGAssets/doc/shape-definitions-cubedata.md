# CubeData Schema

```txt
shape.schema.json#/definitions/cubeData
```



| Abstract            | Extensible | Status         | Identifiable | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                                |
| :------------------ | :--------- | :------------- | :----------- | :---------------- | :-------------------- | :------------------ | :------------------------------------------------------------------------ |
| Can be instantiated | No         | Unknown status | No           | Forbidden         | Allowed               | none                | [shape.schema.json\*](json/defs/shape.schema.json "open original schema") |

## cubeData Type

`object` ([CubeData](shape-definitions-cubedata.md))

# cubeData Properties

| Property              | Type    | Required | Nullable       | Defined by                                                                                               |
| :-------------------- | :------ | :------- | :------------- | :------------------------------------------------------------------------------------------------------- |
| [center](#center)     | `array` | Required | cannot be null | [MSGAssets-shape](vec-definitions-vec3.md "shape.schema.json#/definitions/cubeData/properties/center")   |
| [halfSize](#halfsize) | `array` | Required | cannot be null | [MSGAssets-shape](vec-definitions-vec3.md "shape.schema.json#/definitions/cubeData/properties/halfSize") |

## center

the local position of this shape

`center`

* is required

* Type: `number[]`

* cannot be null

* defined in: [MSGAssets-shape](vec-definitions-vec3.md "shape.schema.json#/definitions/cubeData/properties/center")

### center Type

`number[]`

### center Constraints

**maximum number of items**: the maximum number of items for this array is: `3`

**minimum number of items**: the minimum number of items for this array is: `3`

## halfSize

the local half size of this shape

`halfSize`

* is required

* Type: `number[]`

* cannot be null

* defined in: [MSGAssets-shape](vec-definitions-vec3.md "shape.schema.json#/definitions/cubeData/properties/halfSize")

### halfSize Type

`number[]`

### halfSize Constraints

**maximum number of items**: the maximum number of items for this array is: `3`

**minimum number of items**: the minimum number of items for this array is: `3`

# MSGAssets-shape Schema

```txt
shape.schema.json
```



| Abstract               | Extensible | Status         | Identifiable            | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                              |
| :--------------------- | :--------- | :------------- | :---------------------- | :---------------- | :-------------------- | :------------------ | :---------------------------------------------------------------------- |
| Cannot be instantiated | Yes        | Unknown status | Unknown identifiability | Forbidden         | Allowed               | none                | [shape.schema.json](json/defs/shape.schema.json "open original schema") |

## MSGAssets-shape Type

`object` ([MSGAssets-shape](shape.md))

# MSGAssets-shape Definitions

## Definitions group shapeType

Reference this group by using

```json
{"$ref":"shape.schema.json#/definitions/shapeType"}
```

| Property | Type | Required | Nullable | Defined by |
| :------- | :--- | :------- | :------- | :--------- |

## Definitions group shapeCombinationOp

Reference this group by using

```json
{"$ref":"shape.schema.json#/definitions/shapeCombinationOp"}
```

| Property | Type | Required | Nullable | Defined by |
| :------- | :--- | :------- | :------- | :--------- |

## Definitions group cubeData

Reference this group by using

```json
{"$ref":"shape.schema.json#/definitions/cubeData"}
```

| Property              | Type    | Required | Nullable       | Defined by                                                                                               |
| :-------------------- | :------ | :------- | :------------- | :------------------------------------------------------------------------------------------------------- |
| [center](#center)     | `array` | Required | cannot be null | [MSGAssets-shape](vec-definitions-vec3.md "shape.schema.json#/definitions/cubeData/properties/center")   |
| [halfSize](#halfsize) | `array` | Required | cannot be null | [MSGAssets-shape](vec-definitions-vec3.md "shape.schema.json#/definitions/cubeData/properties/halfSize") |

### center

the local position of this shape

`center`

* is required

* Type: `number[]`

* cannot be null

* defined in: [MSGAssets-shape](vec-definitions-vec3.md "shape.schema.json#/definitions/cubeData/properties/center")

#### center Type

`number[]`

#### center Constraints

**maximum number of items**: the maximum number of items for this array is: `3`

**minimum number of items**: the minimum number of items for this array is: `3`

### halfSize

the local half size of this shape

`halfSize`

* is required

* Type: `number[]`

* cannot be null

* defined in: [MSGAssets-shape](vec-definitions-vec3.md "shape.schema.json#/definitions/cubeData/properties/halfSize")

#### halfSize Type

`number[]`

#### halfSize Constraints

**maximum number of items**: the maximum number of items for this array is: `3`

**minimum number of items**: the minimum number of items for this array is: `3`

## Definitions group sphereData

Reference this group by using

```json
{"$ref":"shape.schema.json#/definitions/sphereData"}
```

| Property            | Type     | Required | Nullable       | Defined by                                                                                                                         |
| :------------------ | :------- | :------- | :------------- | :--------------------------------------------------------------------------------------------------------------------------------- |
| [center](#center-1) | `array`  | Required | cannot be null | [MSGAssets-shape](vec-definitions-vec3.md "shape.schema.json#/definitions/sphereData/properties/center")                           |
| [radius](#radius)   | `number` | Required | cannot be null | [MSGAssets-shape](shape-definitions-spheredata-properties-radius.md "shape.schema.json#/definitions/sphereData/properties/radius") |

### center

the local position of this shape

`center`

* is required

* Type: `number[]`

* cannot be null

* defined in: [MSGAssets-shape](vec-definitions-vec3.md "shape.schema.json#/definitions/sphereData/properties/center")

#### center Type

`number[]`

#### center Constraints

**maximum number of items**: the maximum number of items for this array is: `3`

**minimum number of items**: the minimum number of items for this array is: `3`

### radius

the local radius of this shape

`radius`

* is required

* Type: `number`

* cannot be null

* defined in: [MSGAssets-shape](shape-definitions-spheredata-properties-radius.md "shape.schema.json#/definitions/sphereData/properties/radius")

#### radius Type

`number`

#### radius Constraints

**minimum**: the value of this number must greater than or equal to: `0`

## Definitions group shapeData

Reference this group by using

```json
{"$ref":"shape.schema.json#/definitions/shapeData"}
```

| Property | Type | Required | Nullable | Defined by |
| :------- | :--- | :------- | :------- | :--------- |

## Definitions group shape

Reference this group by using

```json
{"$ref":"shape.schema.json#/definitions/shape"}
```

| Property      | Type     | Required | Nullable       | Defined by                                                                                                                |
| :------------ | :------- | :------- | :------------- | :------------------------------------------------------------------------------------------------------------------------ |
| [type](#type) | `string` | Required | cannot be null | [MSGAssets-shape](shape-definitions-shape-properties-shapetype.md "shape.schema.json#/definitions/shape/properties/type") |
| [data](#data) | Merged   | Required | cannot be null | [MSGAssets-shape](shape-definitions-shapedata.md "shape.schema.json#/definitions/shape/properties/data")                  |

### type



`type`

* is required

* Type: `string` ([ShapeType](shape-definitions-shape-properties-shapetype.md))

* cannot be null

* defined in: [MSGAssets-shape](shape-definitions-shape-properties-shapetype.md "shape.schema.json#/definitions/shape/properties/type")

#### type Type

`string` ([ShapeType](shape-definitions-shape-properties-shapetype.md))

#### type Constraints

**enum**: the value of this property must be equal to one of the following values:

| Value      | Explanation |
| :--------- | :---------- |
| `"Cube"`   |             |
| `"Sphere"` |             |

### data



`data`

* is required

* Type: `object` ([ShapeData](shape-definitions-shapedata.md))

* cannot be null

* defined in: [MSGAssets-shape](shape-definitions-shapedata.md "shape.schema.json#/definitions/shape/properties/data")

#### data Type

`object` ([ShapeData](shape-definitions-shapedata.md))

one (and only one) of

* [CubeData](shape-definitions-cubedata.md "check type definition")

* [SphereData](shape-definitions-spheredata.md "check type definition")

## Definitions group shapeCombinationShape

Reference this group by using

```json
{"$ref":"shape.schema.json#/definitions/shapeCombinationShape"}
```

| Property | Type | Required | Nullable | Defined by |
| :------- | :--- | :------- | :------- | :--------- |

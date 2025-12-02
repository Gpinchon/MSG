# Transform Schema

```txt
transform.schema.json#/definitions/transform
```



| Abstract            | Extensible | Status         | Identifiable | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                                        |
| :------------------ | :--------- | :------------- | :----------- | :---------------- | :-------------------- | :------------------ | :-------------------------------------------------------------------------------- |
| Can be instantiated | No         | Unknown status | No           | Forbidden         | Allowed               | none                | [transform.schema.json\*](json/defs/transform.schema.json "open original schema") |

## transform Type

`object` ([Transform](transform-definitions-transform.md))

# transform Properties

| Property              | Type     | Required | Nullable       | Defined by                                                                                                                   |
| :-------------------- | :------- | :------- | :------------- | :--------------------------------------------------------------------------------------------------------------------------- |
| [copyFrom](#copyfrom) | `string` | Optional | cannot be null | [MSGAssets-transform](external-definitions-fromentity.md "transform.schema.json#/definitions/transform/properties/copyFrom") |
| [up](#up)             | `array`  | Optional | cannot be null | [MSGAssets-transform](vec-definitions-vec3.md "transform.schema.json#/definitions/transform/properties/up")                  |
| [right](#right)       | `array`  | Optional | cannot be null | [MSGAssets-transform](vec-definitions-vec3.md "transform.schema.json#/definitions/transform/properties/right")               |
| [forward](#forward)   | `array`  | Optional | cannot be null | [MSGAssets-transform](vec-definitions-vec3.md "transform.schema.json#/definitions/transform/properties/forward")             |
| [position](#position) | `array`  | Optional | cannot be null | [MSGAssets-transform](vec-definitions-vec3.md "transform.schema.json#/definitions/transform/properties/position")            |
| [scale](#scale)       | `array`  | Optional | cannot be null | [MSGAssets-transform](vec-definitions-vec3.md "transform.schema.json#/definitions/transform/properties/scale")               |
| [rotation](#rotation) | `array`  | Optional | cannot be null | [MSGAssets-transform](vec-definitions-vec4.md "transform.schema.json#/definitions/transform/properties/rotation")            |

## copyFrom

the name of the entity to copy the component from

`copyFrom`

* is optional

* Type: `string` ([FromEntity](external-definitions-fromentity.md))

* cannot be null

* defined in: [MSGAssets-transform](external-definitions-fromentity.md "transform.schema.json#/definitions/transform/properties/copyFrom")

### copyFrom Type

`string` ([FromEntity](external-definitions-fromentity.md))

## up

the up vector used for transformations

`up`

* is optional

* Type: `number[]`

* cannot be null

* defined in: [MSGAssets-transform](vec-definitions-vec3.md "transform.schema.json#/definitions/transform/properties/up")

### up Type

`number[]`

### up Constraints

**maximum number of items**: the maximum number of items for this array is: `3`

**minimum number of items**: the minimum number of items for this array is: `3`

### up Default Value

The default value is:

```json
[
  0,
  1,
  0
]
```

## right

the local right vector used for transformations

`right`

* is optional

* Type: `number[]`

* cannot be null

* defined in: [MSGAssets-transform](vec-definitions-vec3.md "transform.schema.json#/definitions/transform/properties/right")

### right Type

`number[]`

### right Constraints

**maximum number of items**: the maximum number of items for this array is: `3`

**minimum number of items**: the minimum number of items for this array is: `3`

### right Default Value

The default value is:

```json
[
  1,
  0,
  0
]
```

## forward

the local forward vector used for transformations

`forward`

* is optional

* Type: `number[]`

* cannot be null

* defined in: [MSGAssets-transform](vec-definitions-vec3.md "transform.schema.json#/definitions/transform/properties/forward")

### forward Type

`number[]`

### forward Constraints

**maximum number of items**: the maximum number of items for this array is: `3`

**minimum number of items**: the minimum number of items for this array is: `3`

### forward Default Value

The default value is:

```json
[
  0,
  0,
  -1
]
```

## position

the local position vector used for transformations

`position`

* is optional

* Type: `number[]`

* cannot be null

* defined in: [MSGAssets-transform](vec-definitions-vec3.md "transform.schema.json#/definitions/transform/properties/position")

### position Type

`number[]`

### position Constraints

**maximum number of items**: the maximum number of items for this array is: `3`

**minimum number of items**: the minimum number of items for this array is: `3`

### position Default Value

The default value is:

```json
[
  0,
  0,
  0
]
```

## scale

the local scale vector used for transformations

`scale`

* is optional

* Type: `number[]`

* cannot be null

* defined in: [MSGAssets-transform](vec-definitions-vec3.md "transform.schema.json#/definitions/transform/properties/scale")

### scale Type

`number[]`

### scale Constraints

**maximum number of items**: the maximum number of items for this array is: `3`

**minimum number of items**: the minimum number of items for this array is: `3`

### scale Default Value

The default value is:

```json
[
  1,
  1,
  1
]
```

## rotation

the local rotation quaternion used for transformations

`rotation`

* is optional

* Type: `number[]`

* cannot be null

* defined in: [MSGAssets-transform](vec-definitions-vec4.md "transform.schema.json#/definitions/transform/properties/rotation")

### rotation Type

`number[]`

### rotation Constraints

**maximum number of items**: the maximum number of items for this array is: `4`

**minimum number of items**: the minimum number of items for this array is: `4`

### rotation Default Value

The default value is:

```json
[
  0,
  0,
  0,
  1
]
```

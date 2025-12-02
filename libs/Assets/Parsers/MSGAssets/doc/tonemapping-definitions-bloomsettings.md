# BloomSettings Schema

```txt
tonemapping.schema.json#/definitions/bloomSettings
```



| Abstract            | Extensible | Status         | Identifiable | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                                            |
| :------------------ | :--------- | :------------- | :----------- | :---------------- | :-------------------- | :------------------ | :------------------------------------------------------------------------------------ |
| Can be instantiated | No         | Unknown status | No           | Forbidden         | Allowed               | none                | [tonemapping.schema.json\*](json/defs/tonemapping.schema.json "open original schema") |

## bloomSettings Type

`object` ([BloomSettings](tonemapping-definitions-bloomsettings.md))

## bloomSettings Default Value

The default value is:

```json
[
  1,
  1,
  1
]
```

# bloomSettings Properties

| Property                | Type     | Required | Nullable       | Defined by                                                                                                                                                       |
| :---------------------- | :------- | :------- | :------------- | :--------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| [intensity](#intensity) | `number` | Optional | cannot be null | [MSGAssets-tonemapping](tonemapping-definitions-bloomsettings-properties-intensity.md "tonemapping.schema.json#/definitions/bloomSettings/properties/intensity") |
| [threshold](#threshold) | `number` | Optional | cannot be null | [MSGAssets-tonemapping](tonemapping-definitions-bloomsettings-properties-threshold.md "tonemapping.schema.json#/definitions/bloomSettings/properties/threshold") |
| [smoothing](#smoothing) | `number` | Optional | cannot be null | [MSGAssets-tonemapping](tonemapping-definitions-bloomsettings-properties-smoothing.md "tonemapping.schema.json#/definitions/bloomSettings/properties/smoothing") |
| [size](#size)           | `number` | Optional | cannot be null | [MSGAssets-tonemapping](tonemapping-definitions-bloomsettings-properties-size.md "tonemapping.schema.json#/definitions/bloomSettings/properties/size")           |
| [tint](#tint)           | `array`  | Optional | cannot be null | [MSGAssets-tonemapping](vec-definitions-vec3.md "tonemapping.schema.json#/definitions/bloomSettings/properties/tint")                                            |

## intensity



`intensity`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-tonemapping](tonemapping-definitions-bloomsettings-properties-intensity.md "tonemapping.schema.json#/definitions/bloomSettings/properties/intensity")

### intensity Type

`number`

### intensity Default Value

The default value is:

```json
1
```

## threshold



`threshold`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-tonemapping](tonemapping-definitions-bloomsettings-properties-threshold.md "tonemapping.schema.json#/definitions/bloomSettings/properties/threshold")

### threshold Type

`number`

### threshold Default Value

The default value is:

```json
1
```

## smoothing



`smoothing`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-tonemapping](tonemapping-definitions-bloomsettings-properties-smoothing.md "tonemapping.schema.json#/definitions/bloomSettings/properties/smoothing")

### smoothing Type

`number`

### smoothing Default Value

The default value is:

```json
10
```

## size



`size`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-tonemapping](tonemapping-definitions-bloomsettings-properties-size.md "tonemapping.schema.json#/definitions/bloomSettings/properties/size")

### size Type

`number`

### size Default Value

The default value is:

```json
25
```

## tint

a vector of 3 floats

`tint`

* is optional

* Type: `number[]`

* cannot be null

* defined in: [MSGAssets-tonemapping](vec-definitions-vec3.md "tonemapping.schema.json#/definitions/bloomSettings/properties/tint")

### tint Type

`number[]`

### tint Constraints

**maximum number of items**: the maximum number of items for this array is: `3`

**minimum number of items**: the minimum number of items for this array is: `3`

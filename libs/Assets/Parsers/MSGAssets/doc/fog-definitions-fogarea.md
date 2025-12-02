# FogArea Schema

```txt
fog.schema.json#/definitions/fogArea
```

a compination of fog shapes

| Abstract            | Extensible | Status         | Identifiable | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                            |
| :------------------ | :--------- | :------------- | :----------- | :---------------- | :-------------------- | :------------------ | :-------------------------------------------------------------------- |
| Can be instantiated | No         | Unknown status | No           | Forbidden         | Allowed               | none                | [fog.schema.json\*](json/defs/fog.schema.json "open original schema") |

## fogArea Type

`object` ([FogArea](fog-definitions-fogarea.md))

# fogArea Properties

| Property                                    | Type     | Required | Nullable       | Defined by                                                                                                                                       |
| :------------------------------------------ | :------- | :------- | :------------- | :----------------------------------------------------------------------------------------------------------------------------------------------- |
| [copyFrom](#copyfrom)                       | `string` | Optional | cannot be null | [MSGAssets-fog](external-definitions-fromentity.md "fog.schema.json#/definitions/fogArea/properties/copyFrom")                                   |
| [scattering](#scattering)                   | `array`  | Optional | cannot be null | [MSGAssets-fog](vec-definitions-vec3.md "fog.schema.json#/definitions/fogArea/properties/scattering")                                            |
| [emissive](#emissive)                       | `array`  | Optional | cannot be null | [MSGAssets-fog](vec-definitions-vec3.md "fog.schema.json#/definitions/fogArea/properties/emissive")                                              |
| [extinction](#extinction)                   | `number` | Optional | cannot be null | [MSGAssets-fog](fog-definitions-fogarea-properties-extinction.md "fog.schema.json#/definitions/fogArea/properties/extinction")                   |
| [phaseG](#phaseg)                           | `number` | Optional | cannot be null | [MSGAssets-fog](fog-definitions-fogarea-properties-phaseg.md "fog.schema.json#/definitions/fogArea/properties/phaseG")                           |
| [attenuationExponant](#attenuationexponant) | `number` | Optional | cannot be null | [MSGAssets-fog](fog-definitions-fogarea-properties-attenuationexponant.md "fog.schema.json#/definitions/fogArea/properties/attenuationExponant") |
| [op](#op)                                   | `string` | Optional | cannot be null | [MSGAssets-fog](fog-definitions-fogareaop.md "fog.schema.json#/definitions/fogArea/properties/op")                                               |
| [shapes](#shapes)                           | `array`  | Required | cannot be null | [MSGAssets-fog](shape-definitions-shapecombinationshape.md "fog.schema.json#/definitions/fogArea/properties/shapes")                             |

## copyFrom

the name of the entity to copy the component from

`copyFrom`

* is optional

* Type: `string` ([FromEntity](external-definitions-fromentity.md))

* cannot be null

* defined in: [MSGAssets-fog](external-definitions-fromentity.md "fog.schema.json#/definitions/fogArea/properties/copyFrom")

### copyFrom Type

`string` ([FromEntity](external-definitions-fromentity.md))

## scattering

a vector of 3 floats

`scattering`

* is optional

* Type: `number[]`

* cannot be null

* defined in: [MSGAssets-fog](vec-definitions-vec3.md "fog.schema.json#/definitions/fogArea/properties/scattering")

### scattering Type

`number[]`

### scattering Constraints

**maximum**: the value of this number must smaller than or equal to: `1`

**minimum**: the value of this number must greater than or equal to: `0`

**maximum number of items**: the maximum number of items for this array is: `3`

**minimum number of items**: the minimum number of items for this array is: `3`

### scattering Default Value

The default value is:

```json
[
  1,
  1,
  1
]
```

## emissive

a vector of 3 floats

`emissive`

* is optional

* Type: `number[]`

* cannot be null

* defined in: [MSGAssets-fog](vec-definitions-vec3.md "fog.schema.json#/definitions/fogArea/properties/emissive")

### emissive Type

`number[]`

### emissive Constraints

**minimum**: the value of this number must greater than or equal to: `0`

**maximum number of items**: the maximum number of items for this array is: `3`

**minimum number of items**: the minimum number of items for this array is: `3`

### emissive Default Value

The default value is:

```json
[
  0,
  0,
  0
]
```

## extinction

the extinction factor of the fog

`extinction`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-fog](fog-definitions-fogarea-properties-extinction.md "fog.schema.json#/definitions/fogArea/properties/extinction")

### extinction Type

`number`

### extinction Constraints

**maximum**: the value of this number must smaller than or equal to: `1`

**minimum**: the value of this number must greater than or equal to: `0`

### extinction Default Value

The default value is:

```json
0.02
```

## phaseG

G phase function describes how much forward (g<0) or backward (g > 0) light scatters around

`phaseG`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-fog](fog-definitions-fogarea-properties-phaseg.md "fog.schema.json#/definitions/fogArea/properties/phaseG")

### phaseG Type

`number`

### phaseG Constraints

**maximum**: the value of this number must smaller than or equal to: `1`

**minimum**: the value of this number must greater than or equal to: `-1`

### phaseG Default Value

The default value is:

```json
0
```

## attenuationExponant

defines how much the fog scattering and extinction gets attenuated near the edges of the shapes

`attenuationExponant`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-fog](fog-definitions-fogarea-properties-attenuationexponant.md "fog.schema.json#/definitions/fogArea/properties/attenuationExponant")

### attenuationExponant Type

`number`

### attenuationExponant Constraints

**maximum**: the value of this number must smaller than or equal to: `1`

**minimum**: the value of this number must greater than or equal to: `0`

### attenuationExponant Default Value

The default value is:

```json
1
```

## op

defines how this fog area will behave with the global fog

`op`

* is optional

* Type: `string`

* cannot be null

* defined in: [MSGAssets-fog](fog-definitions-fogareaop.md "fog.schema.json#/definitions/fogArea/properties/op")

### op Type

`string`

### op Constraints

**enum**: the value of this property must be equal to one of the following values:

| Value       | Explanation |
| :---------- | :---------- |
| `"Add"`     |             |
| `"Replace"` |             |

### op Default Value

The default value is:

```json
"Add"
```

## shapes

the shapes building this combination

`shapes`

* is required

* Type: `object[]` ([Details](shape-definitions-shapecombinationshape-items.md))

* cannot be null

* defined in: [MSGAssets-fog](shape-definitions-shapecombinationshape.md "fog.schema.json#/definitions/fogArea/properties/shapes")

### shapes Type

`object[]` ([Details](shape-definitions-shapecombinationshape-items.md))

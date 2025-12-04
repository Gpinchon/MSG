# MSGAssets-fog Schema

```txt
fog.schema.json
```



| Abstract               | Extensible | Status         | Identifiable            | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                          |
| :--------------------- | :--------- | :------------- | :---------------------- | :---------------- | :-------------------- | :------------------ | :------------------------------------------------------------------ |
| Cannot be instantiated | Yes        | Unknown status | Unknown identifiability | Forbidden         | Allowed               | none                | [fog.schema.json](json/defs/fog.schema.json "open original schema") |

## MSGAssets-fog Type

`object` ([MSGAssets-fog](fog.md))

# MSGAssets-fog Definitions

## Definitions group fogAreaOp

Reference this group by using

```json
{"$ref":"fog.schema.json#/definitions/fogAreaOp"}
```

| Property | Type | Required | Nullable | Defined by |
| :------- | :--- | :------- | :------- | :--------- |

## Definitions group fogArea

Reference this group by using

```json
{"$ref":"fog.schema.json#/definitions/fogArea"}
```

| Property                                    | Type     | Required | Nullable       | Defined by                                                                                                                                       |
| :------------------------------------------ | :------- | :------- | :------------- | :----------------------------------------------------------------------------------------------------------------------------------------------- |
| [copyFrom](#copyfrom)                       | `string` | Optional | cannot be null | [MSGAssets-fog](external-definitions-fromentity.md "fog.schema.json#/definitions/fogArea/properties/copyFrom")                                   |
| [scattering](#scattering)                   | `array`  | Optional | cannot be null | [MSGAssets-fog](vec-definitions-vec3.md "fog.schema.json#/definitions/fogArea/properties/scattering")                                            |
| [emissive](#emissive)                       | `array`  | Optional | cannot be null | [MSGAssets-fog](vec-definitions-vec3.md "fog.schema.json#/definitions/fogArea/properties/emissive")                                              |
| [extinction](#extinction)                   | `number` | Optional | cannot be null | [MSGAssets-fog](fog-definitions-fogarea-properties-extinction.md "fog.schema.json#/definitions/fogArea/properties/extinction")                   |
| [phaseG](#phaseg)                           | `number` | Optional | cannot be null | [MSGAssets-fog](fog-definitions-fogarea-properties-phaseg.md "fog.schema.json#/definitions/fogArea/properties/phaseG")                           |
| [attenuationExponent](#attenuationexponent) | `number` | Optional | cannot be null | [MSGAssets-fog](fog-definitions-fogarea-properties-attenuationexponent.md "fog.schema.json#/definitions/fogArea/properties/attenuationExponent") |
| [op](#op)                                   | `string` | Optional | cannot be null | [MSGAssets-fog](fog-definitions-fogarea-properties-op.md "fog.schema.json#/definitions/fogArea/properties/op")                                   |
| [shapes](#shapes)                           | `array`  | Required | cannot be null | [MSGAssets-fog](shape-definitions-shapecombinationshape.md "fog.schema.json#/definitions/fogArea/properties/shapes")                             |

### copyFrom

the name of the entity to copy the component from

`copyFrom`

* is optional

* Type: `string` ([FromEntity](external-definitions-fromentity.md))

* cannot be null

* defined in: [MSGAssets-fog](external-definitions-fromentity.md "fog.schema.json#/definitions/fogArea/properties/copyFrom")

#### copyFrom Type

`string` ([FromEntity](external-definitions-fromentity.md))

### scattering

how much light get scattered

`scattering`

* is optional

* Type: `number[]`

* cannot be null

* defined in: [MSGAssets-fog](vec-definitions-vec3.md "fog.schema.json#/definitions/fogArea/properties/scattering")

#### scattering Type

`number[]`

#### scattering Constraints

**maximum**: the value of this number must smaller than or equal to: `1`

**minimum**: the value of this number must greater than or equal to: `0`

**maximum number of items**: the maximum number of items for this array is: `3`

**minimum number of items**: the minimum number of items for this array is: `3`

#### scattering Default Value

The default value is:

```json
[
  1,
  1,
  1
]
```

### emissive

how much light get emitted

`emissive`

* is optional

* Type: `number[]`

* cannot be null

* defined in: [MSGAssets-fog](vec-definitions-vec3.md "fog.schema.json#/definitions/fogArea/properties/emissive")

#### emissive Type

`number[]`

#### emissive Constraints

**minimum**: the value of this number must greater than or equal to: `0`

**maximum number of items**: the maximum number of items for this array is: `3`

**minimum number of items**: the minimum number of items for this array is: `3`

#### emissive Default Value

The default value is:

```json
[
  0,
  0,
  0
]
```

### extinction

the extinction factor of the fog

`extinction`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-fog](fog-definitions-fogarea-properties-extinction.md "fog.schema.json#/definitions/fogArea/properties/extinction")

#### extinction Type

`number`

#### extinction Constraints

**maximum**: the value of this number must smaller than or equal to: `1`

**minimum**: the value of this number must greater than or equal to: `0`

#### extinction Default Value

The default value is:

```json
0.02
```

### phaseG

G phase function describes how much forward (g<0) or backward (g > 0) light scatters around

`phaseG`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-fog](fog-definitions-fogarea-properties-phaseg.md "fog.schema.json#/definitions/fogArea/properties/phaseG")

#### phaseG Type

`number`

#### phaseG Constraints

**maximum**: the value of this number must smaller than or equal to: `1`

**minimum**: the value of this number must greater than or equal to: `-1`

#### phaseG Default Value

The default value is:

```json
0
```

### attenuationExponent

defines how much the fog scattering and extinction gets attenuated near the edges of the shapes

`attenuationExponent`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-fog](fog-definitions-fogarea-properties-attenuationexponent.md "fog.schema.json#/definitions/fogArea/properties/attenuationExponent")

#### attenuationExponent Type

`number`

#### attenuationExponent Constraints

**maximum**: the value of this number must smaller than or equal to: `1`

**minimum**: the value of this number must greater than or equal to: `0`

#### attenuationExponent Default Value

The default value is:

```json
1
```

### op

defines how this fog area will behave with the global fog

`op`

* is optional

* Type: `string`

* cannot be null

* defined in: [MSGAssets-fog](fog-definitions-fogarea-properties-op.md "fog.schema.json#/definitions/fogArea/properties/op")

#### op Type

`string`

#### op Constraints

**enum**: the value of this property must be equal to one of the following values:

| Value       | Explanation |
| :---------- | :---------- |
| `"Add"`     |             |
| `"Replace"` |             |

#### op Default Value

The default value is:

```json
"Add"
```

### shapes

the shapes building this combination

`shapes`

* is required

* Type: `object[]` ([Details](shape-definitions-shapecombinationshape-items.md))

* cannot be null

* defined in: [MSGAssets-fog](shape-definitions-shapecombinationshape.md "fog.schema.json#/definitions/fogArea/properties/shapes")

#### shapes Type

`object[]` ([Details](shape-definitions-shapecombinationshape-items.md))

## Definitions group volumetricFogSettings

Reference this group by using

```json
{"$ref":"fog.schema.json#/definitions/volumetricFogSettings"}
```

| Property                                        | Type     | Required | Nullable       | Defined by                                                                                                                                                                       |
| :---------------------------------------------- | :------- | :------- | :------------- | :------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| [minDistance](#mindistance)                     | `number` | Optional | cannot be null | [MSGAssets-fog](fog-definitions-volumetricfogsettings-properties-mindistance.md "fog.schema.json#/definitions/volumetricFogSettings/properties/minDistance")                     |
| [maxDistance](#maxdistance)                     | `number` | Optional | cannot be null | [MSGAssets-fog](fog-definitions-volumetricfogsettings-properties-maxdistance.md "fog.schema.json#/definitions/volumetricFogSettings/properties/maxDistance")                     |
| [depthExponent](#depthexponent)                 | `number` | Optional | cannot be null | [MSGAssets-fog](fog-definitions-volumetricfogsettings-properties-depthexponent.md "fog.schema.json#/definitions/volumetricFogSettings/properties/depthExponent")                 |
| [noiseDensityOffset](#noisedensityoffset)       | `array`  | Optional | cannot be null | [MSGAssets-fog](vec-definitions-vec3.md "fog.schema.json#/definitions/volumetricFogSettings/properties/noiseDensityOffset")                                                      |
| [noiseDensityScale](#noisedensityscale)         | `number` | Optional | cannot be null | [MSGAssets-fog](fog-definitions-volumetricfogsettings-properties-noisedensityscale.md "fog.schema.json#/definitions/volumetricFogSettings/properties/noiseDensityScale")         |
| [noiseDensityIntensity](#noisedensityintensity) | `number` | Optional | cannot be null | [MSGAssets-fog](fog-definitions-volumetricfogsettings-properties-noisedensityintensity.md "fog.schema.json#/definitions/volumetricFogSettings/properties/noiseDensityIntensity") |
| [noiseDensityMaxDist](#noisedensitymaxdist)     | `number` | Optional | cannot be null | [MSGAssets-fog](fog-definitions-volumetricfogsettings-properties-noisedensitymaxdist.md "fog.schema.json#/definitions/volumetricFogSettings/properties/noiseDensityMaxDist")     |

### minDistance

the distance at which the volumetric fog should start rendering

`minDistance`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-fog](fog-definitions-volumetricfogsettings-properties-mindistance.md "fog.schema.json#/definitions/volumetricFogSettings/properties/minDistance")

#### minDistance Type

`number`

#### minDistance Default Value

The default value is:

```json
0.5
```

### maxDistance

the max distance the volumetric fog will be displayed

`maxDistance`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-fog](fog-definitions-volumetricfogsettings-properties-maxdistance.md "fog.schema.json#/definitions/volumetricFogSettings/properties/maxDistance")

#### maxDistance Type

`number`

#### maxDistance Default Value

The default value is:

```json
1000
```

### depthExponent

the depth exponent used to render volumetric fog

`depthExponent`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-fog](fog-definitions-volumetricfogsettings-properties-depthexponent.md "fog.schema.json#/definitions/volumetricFogSettings/properties/depthExponent")

#### depthExponent Type

`number`

#### depthExponent Default Value

The default value is:

```json
5
```

### noiseDensityOffset

the offset inside the noise texture

`noiseDensityOffset`

* is optional

* Type: `number[]`

* cannot be null

* defined in: [MSGAssets-fog](vec-definitions-vec3.md "fog.schema.json#/definitions/volumetricFogSettings/properties/noiseDensityOffset")

#### noiseDensityOffset Type

`number[]`

#### noiseDensityOffset Constraints

**maximum number of items**: the maximum number of items for this array is: `3`

**minimum number of items**: the minimum number of items for this array is: `3`

#### noiseDensityOffset Default Value

The default value is:

```json
[
  0,
  0,
  0
]
```

### noiseDensityScale

the scale of the sampling inside the noise texture

`noiseDensityScale`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-fog](fog-definitions-volumetricfogsettings-properties-noisedensityscale.md "fog.schema.json#/definitions/volumetricFogSettings/properties/noiseDensityScale")

#### noiseDensityScale Type

`number`

#### noiseDensityScale Default Value

The default value is:

```json
0.5
```

### noiseDensityIntensity

the intensity of the noise in the final render

`noiseDensityIntensity`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-fog](fog-definitions-volumetricfogsettings-properties-noisedensityintensity.md "fog.schema.json#/definitions/volumetricFogSettings/properties/noiseDensityIntensity")

#### noiseDensityIntensity Type

`number`

#### noiseDensityIntensity Default Value

The default value is:

```json
1
```

### noiseDensityMaxDist

the maximum depth at which density noise will be applied before being progressively reduced

`noiseDensityMaxDist`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-fog](fog-definitions-volumetricfogsettings-properties-noisedensitymaxdist.md "fog.schema.json#/definitions/volumetricFogSettings/properties/noiseDensityMaxDist")

#### noiseDensityMaxDist Type

`number`

#### noiseDensityMaxDist Default Value

The default value is:

```json
100
```

## Definitions group fogSettings

Reference this group by using

```json
{"$ref":"fog.schema.json#/definitions/fogSettings"}
```

| Property                                  | Type      | Required | Nullable       | Defined by                                                                                                                                   |
| :---------------------------------------- | :-------- | :------- | :------------- | :------------------------------------------------------------------------------------------------------------------------------------------- |
| [scattering](#scattering-1)               | `array`   | Optional | cannot be null | [MSGAssets-fog](vec-definitions-vec3.md "fog.schema.json#/definitions/fogSettings/properties/scattering")                                    |
| [emissive](#emissive-1)                   | `array`   | Optional | cannot be null | [MSGAssets-fog](vec-definitions-vec3.md "fog.schema.json#/definitions/fogSettings/properties/emissive")                                      |
| [phaseG](#phaseg-1)                       | `number`  | Optional | cannot be null | [MSGAssets-fog](fog-definitions-fogsettings-properties-phaseg.md "fog.schema.json#/definitions/fogSettings/properties/phaseG")               |
| [extinction](#extinction-1)               | `number`  | Optional | cannot be null | [MSGAssets-fog](fog-definitions-fogsettings-properties-extinction.md "fog.schema.json#/definitions/fogSettings/properties/extinction")       |
| [fogBackground](#fogbackground)           | `boolean` | Optional | cannot be null | [MSGAssets-fog](fog-definitions-fogsettings-properties-fogbackground.md "fog.schema.json#/definitions/fogSettings/properties/fogBackground") |
| [volumetricSettings](#volumetricsettings) | `object`  | Optional | cannot be null | [MSGAssets-fog](fog-definitions-volumetricfogsettings.md "fog.schema.json#/definitions/fogSettings/properties/volumetricSettings")           |

### scattering



`scattering`

* is optional

* Type: `number[]`

* cannot be null

* defined in: [MSGAssets-fog](vec-definitions-vec3.md "fog.schema.json#/definitions/fogSettings/properties/scattering")

#### scattering Type

`number[]`

#### scattering Constraints

**maximum number of items**: the maximum number of items for this array is: `3`

**minimum number of items**: the minimum number of items for this array is: `3`

#### scattering Default Value

The default value is:

```json
[
  1,
  1,
  1
]
```

### emissive



`emissive`

* is optional

* Type: `number[]`

* cannot be null

* defined in: [MSGAssets-fog](vec-definitions-vec3.md "fog.schema.json#/definitions/fogSettings/properties/emissive")

#### emissive Type

`number[]`

#### emissive Constraints

**maximum number of items**: the maximum number of items for this array is: `3`

**minimum number of items**: the minimum number of items for this array is: `3`

#### emissive Default Value

The default value is:

```json
[
  0,
  0,
  0
]
```

### phaseG



`phaseG`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-fog](fog-definitions-fogsettings-properties-phaseg.md "fog.schema.json#/definitions/fogSettings/properties/phaseG")

#### phaseG Type

`number`

#### phaseG Constraints

**maximum**: the value of this number must smaller than or equal to: `1`

**minimum**: the value of this number must greater than or equal to: `-1`

#### phaseG Default Value

The default value is:

```json
0
```

### extinction



`extinction`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-fog](fog-definitions-fogsettings-properties-extinction.md "fog.schema.json#/definitions/fogSettings/properties/extinction")

#### extinction Type

`number`

#### extinction Constraints

**maximum**: the value of this number must smaller than or equal to: `1`

**minimum**: the value of this number must greater than or equal to: `0`

#### extinction Default Value

The default value is:

```json
0.025
```

### fogBackground



`fogBackground`

* is optional

* Type: `boolean`

* cannot be null

* defined in: [MSGAssets-fog](fog-definitions-fogsettings-properties-fogbackground.md "fog.schema.json#/definitions/fogSettings/properties/fogBackground")

#### fogBackground Type

`boolean`

#### fogBackground Default Value

The default value is:

```json
true
```

### volumetricSettings

it is highly recommended to customize these settings depending on your scene

`volumetricSettings`

* is optional

* Type: `object` ([VolumetricFogSettings](fog-definitions-volumetricfogsettings.md))

* cannot be null

* defined in: [MSGAssets-fog](fog-definitions-volumetricfogsettings.md "fog.schema.json#/definitions/fogSettings/properties/volumetricSettings")

#### volumetricSettings Type

`object` ([VolumetricFogSettings](fog-definitions-volumetricfogsettings.md))

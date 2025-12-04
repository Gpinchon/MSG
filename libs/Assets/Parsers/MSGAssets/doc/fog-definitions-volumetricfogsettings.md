# VolumetricFogSettings Schema

```txt
fog.schema.json#/definitions/volumetricFogSettings
```



| Abstract            | Extensible | Status         | Identifiable | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                            |
| :------------------ | :--------- | :------------- | :----------- | :---------------- | :-------------------- | :------------------ | :-------------------------------------------------------------------- |
| Can be instantiated | No         | Unknown status | No           | Forbidden         | Allowed               | none                | [fog.schema.json\*](json/defs/fog.schema.json "open original schema") |

## volumetricFogSettings Type

`object` ([VolumetricFogSettings](fog-definitions-volumetricfogsettings.md))

# volumetricFogSettings Properties

| Property                                        | Type     | Required | Nullable       | Defined by                                                                                                                                                                       |
| :---------------------------------------------- | :------- | :------- | :------------- | :------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| [minDistance](#mindistance)                     | `number` | Optional | cannot be null | [MSGAssets-fog](fog-definitions-volumetricfogsettings-properties-mindistance.md "fog.schema.json#/definitions/volumetricFogSettings/properties/minDistance")                     |
| [maxDistance](#maxdistance)                     | `number` | Optional | cannot be null | [MSGAssets-fog](fog-definitions-volumetricfogsettings-properties-maxdistance.md "fog.schema.json#/definitions/volumetricFogSettings/properties/maxDistance")                     |
| [depthExponent](#depthexponent)                 | `number` | Optional | cannot be null | [MSGAssets-fog](fog-definitions-volumetricfogsettings-properties-depthexponent.md "fog.schema.json#/definitions/volumetricFogSettings/properties/depthExponent")                 |
| [noiseDensityOffset](#noisedensityoffset)       | `array`  | Optional | cannot be null | [MSGAssets-fog](vec-definitions-vec3.md "fog.schema.json#/definitions/volumetricFogSettings/properties/noiseDensityOffset")                                                      |
| [noiseDensityScale](#noisedensityscale)         | `number` | Optional | cannot be null | [MSGAssets-fog](fog-definitions-volumetricfogsettings-properties-noisedensityscale.md "fog.schema.json#/definitions/volumetricFogSettings/properties/noiseDensityScale")         |
| [noiseDensityIntensity](#noisedensityintensity) | `number` | Optional | cannot be null | [MSGAssets-fog](fog-definitions-volumetricfogsettings-properties-noisedensityintensity.md "fog.schema.json#/definitions/volumetricFogSettings/properties/noiseDensityIntensity") |
| [noiseDensityMaxDist](#noisedensitymaxdist)     | `number` | Optional | cannot be null | [MSGAssets-fog](fog-definitions-volumetricfogsettings-properties-noisedensitymaxdist.md "fog.schema.json#/definitions/volumetricFogSettings/properties/noiseDensityMaxDist")     |

## minDistance

the distance at which the volumetric fog should start rendering

`minDistance`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-fog](fog-definitions-volumetricfogsettings-properties-mindistance.md "fog.schema.json#/definitions/volumetricFogSettings/properties/minDistance")

### minDistance Type

`number`

### minDistance Default Value

The default value is:

```json
0.5
```

## maxDistance

the max distance the volumetric fog will be displayed

`maxDistance`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-fog](fog-definitions-volumetricfogsettings-properties-maxdistance.md "fog.schema.json#/definitions/volumetricFogSettings/properties/maxDistance")

### maxDistance Type

`number`

### maxDistance Default Value

The default value is:

```json
1000
```

## depthExponent

the depth exponent used to render volumetric fog

`depthExponent`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-fog](fog-definitions-volumetricfogsettings-properties-depthexponent.md "fog.schema.json#/definitions/volumetricFogSettings/properties/depthExponent")

### depthExponent Type

`number`

### depthExponent Default Value

The default value is:

```json
5
```

## noiseDensityOffset

the offset inside the noise texture

`noiseDensityOffset`

* is optional

* Type: `number[]`

* cannot be null

* defined in: [MSGAssets-fog](vec-definitions-vec3.md "fog.schema.json#/definitions/volumetricFogSettings/properties/noiseDensityOffset")

### noiseDensityOffset Type

`number[]`

### noiseDensityOffset Constraints

**maximum number of items**: the maximum number of items for this array is: `3`

**minimum number of items**: the minimum number of items for this array is: `3`

### noiseDensityOffset Default Value

The default value is:

```json
[
  0,
  0,
  0
]
```

## noiseDensityScale

the scale of the sampling inside the noise texture

`noiseDensityScale`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-fog](fog-definitions-volumetricfogsettings-properties-noisedensityscale.md "fog.schema.json#/definitions/volumetricFogSettings/properties/noiseDensityScale")

### noiseDensityScale Type

`number`

### noiseDensityScale Default Value

The default value is:

```json
0.5
```

## noiseDensityIntensity

the intensity of the noise in the final render

`noiseDensityIntensity`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-fog](fog-definitions-volumetricfogsettings-properties-noisedensityintensity.md "fog.schema.json#/definitions/volumetricFogSettings/properties/noiseDensityIntensity")

### noiseDensityIntensity Type

`number`

### noiseDensityIntensity Default Value

The default value is:

```json
1
```

## noiseDensityMaxDist

the maximum depth at which density noise will be applied before being progressively reduced

`noiseDensityMaxDist`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-fog](fog-definitions-volumetricfogsettings-properties-noisedensitymaxdist.md "fog.schema.json#/definitions/volumetricFogSettings/properties/noiseDensityMaxDist")

### noiseDensityMaxDist Type

`number`

### noiseDensityMaxDist Default Value

The default value is:

```json
100
```

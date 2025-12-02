# MSGAssets-light Schema

```txt
light.schema.json
```



| Abstract               | Extensible | Status         | Identifiable            | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                              |
| :--------------------- | :--------- | :------------- | :---------------------- | :---------------- | :-------------------- | :------------------ | :---------------------------------------------------------------------- |
| Cannot be instantiated | Yes        | Unknown status | Unknown identifiability | Forbidden         | Allowed               | none                | [light.schema.json](json/defs/light.schema.json "open original schema") |

## MSGAssets-light Type

`object` ([MSGAssets-light](light.md))

# MSGAssets-light Definitions

## Definitions group shadowSettings

Reference this group by using

```json
{"$ref":"light.schema.json#/definitions/shadowSettings"}
```

| Property                            | Type      | Required | Nullable       | Defined by                                                                                                                                                   |
| :---------------------------------- | :-------- | :------- | :------------- | :----------------------------------------------------------------------------------------------------------------------------------------------------------- |
| [castShadow](#castshadow)           | `boolean` | Optional | cannot be null | [MSGAssets-light](light-definitions-shadowsettings-properties-castshadow.md "light.schema.json#/definitions/shadowSettings/properties/castShadow")           |
| [shadowPrecision](#shadowprecision) | `string`  | Optional | cannot be null | [MSGAssets-light](light-definitions-shadowsettings-properties-shadowprecision.md "light.schema.json#/definitions/shadowSettings/properties/shadowPrecision") |
| [bias](#bias)                       | `number`  | Optional | cannot be null | [MSGAssets-light](light-definitions-shadowsettings-properties-bias.md "light.schema.json#/definitions/shadowSettings/properties/bias")                       |
| [normalBias](#normalbias)           | `number`  | Optional | cannot be null | [MSGAssets-light](light-definitions-shadowsettings-properties-normalbias.md "light.schema.json#/definitions/shadowSettings/properties/normalBias")           |
| [blurRadius](#blurradius)           | `number`  | Optional | cannot be null | [MSGAssets-light](light-definitions-shadowsettings-properties-blurradius.md "light.schema.json#/definitions/shadowSettings/properties/blurRadius")           |
| [resolution](#resolution)           | `number`  | Optional | cannot be null | [MSGAssets-light](light-definitions-shadowsettings-properties-resolution.md "light.schema.json#/definitions/shadowSettings/properties/resolution")           |
| [cascadeCount](#cascadecount)       | `number`  | Optional | cannot be null | [MSGAssets-light](light-definitions-shadowsettings-properties-cascadecount.md "light.schema.json#/definitions/shadowSettings/properties/cascadeCount")       |

### castShadow

if true this light will cast a dynamic shadow

`castShadow`

* is optional

* Type: `boolean`

* cannot be null

* defined in: [MSGAssets-light](light-definitions-shadowsettings-properties-castshadow.md "light.schema.json#/definitions/shadowSettings/properties/castShadow")

#### castShadow Type

`boolean`

#### castShadow Default Value

The default value is:

```json
false
```

### shadowPrecision

allows the renderer to lower shadow buffer precision to save space and performance

`shadowPrecision`

* is optional

* Type: `string`

* cannot be null

* defined in: [MSGAssets-light](light-definitions-shadowsettings-properties-shadowprecision.md "light.schema.json#/definitions/shadowSettings/properties/shadowPrecision")

#### shadowPrecision Type

`string`

#### shadowPrecision Constraints

**enum**: the value of this property must be equal to one of the following values:

| Value      | Explanation |
| :--------- | :---------- |
| `"High"`   |             |
| `"Medium"` |             |
| `"Low"`    |             |

#### shadowPrecision Default Value

The default value is:

```json
"High"
```

### bias

the base amount of bias to apply to shadow maps

`bias`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-light](light-definitions-shadowsettings-properties-bias.md "light.schema.json#/definitions/shadowSettings/properties/bias")

#### bias Type

`number`

#### bias Default Value

The default value is:

```json
0.0025
```

### normalBias

the amount of extra bias to be applied to 90 degrees normals in texels

`normalBias`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-light](light-definitions-shadowsettings-properties-normalbias.md "light.schema.json#/definitions/shadowSettings/properties/normalBias")

#### normalBias Type

`number`

#### normalBias Constraints

**minimum**: the value of this number must greater than or equal to: `0`

#### normalBias Default Value

The default value is:

```json
0.1
```

### blurRadius

blur radius in texels

`blurRadius`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-light](light-definitions-shadowsettings-properties-blurradius.md "light.schema.json#/definitions/shadowSettings/properties/blurRadius")

#### blurRadius Type

`number`

#### blurRadius Constraints

**minimum**: the value of this number must greater than or equal to: `0`

#### blurRadius Default Value

The default value is:

```json
1
```

### resolution

the shadow map resolution

`resolution`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-light](light-definitions-shadowsettings-properties-resolution.md "light.schema.json#/definitions/shadowSettings/properties/resolution")

#### resolution Type

`number`

#### resolution Constraints

**minimum**: the value of this number must greater than or equal to: `128`

#### resolution Default Value

The default value is:

```json
512
```

### cascadeCount

1 means no cascades

`cascadeCount`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-light](light-definitions-shadowsettings-properties-cascadecount.md "light.schema.json#/definitions/shadowSettings/properties/cascadeCount")

#### cascadeCount Type

`number`

#### cascadeCount Default Value

The default value is:

```json
1
```

## Definitions group lightType

Reference this group by using

```json
{"$ref":"light.schema.json#/definitions/lightType"}
```

| Property | Type | Required | Nullable | Defined by |
| :------- | :--- | :------- | :------- | :--------- |

## Definitions group lightPointData

Reference this group by using

```json
{"$ref":"light.schema.json#/definitions/lightPointData"}
```

| Property        | Type     | Required | Nullable       | Defined by                                                                                                                               |
| :-------------- | :------- | :------- | :------------- | :--------------------------------------------------------------------------------------------------------------------------------------- |
| [range](#range) | `number` | Optional | cannot be null | [MSGAssets-light](light-definitions-pointlightdata-properties-range.md "light.schema.json#/definitions/lightPointData/properties/range") |

### range

the range of this light in meters

`range`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-light](light-definitions-pointlightdata-properties-range.md "light.schema.json#/definitions/lightPointData/properties/range")

#### range Type

`number`

#### range Constraints

**minimum**: the value of this number must greater than or equal to: `0`

#### range Default Value

The default value is:

```json
"Infinity"
```

## Definitions group lightSpotData

Reference this group by using

```json
{"$ref":"light.schema.json#/definitions/lightSpotData"}
```

| Property                          | Type     | Required | Nullable       | Defined by                                                                                                                                               |
| :-------------------------------- | :------- | :------- | :------------- | :------------------------------------------------------------------------------------------------------------------------------------------------------- |
| [range](#range-1)                 | `number` | Optional | cannot be null | [MSGAssets-light](light-definitions-spotlightdata-properties-range.md "light.schema.json#/definitions/lightSpotData/properties/range")                   |
| [innerConeAngle](#innerconeangle) | `number` | Optional | cannot be null | [MSGAssets-light](light-definitions-spotlightdata-properties-innerconeangle.md "light.schema.json#/definitions/lightSpotData/properties/innerConeAngle") |
| [outerConeAngle](#outerconeangle) | `number` | Optional | cannot be null | [MSGAssets-light](light-definitions-spotlightdata-properties-outerconeangle.md "light.schema.json#/definitions/lightSpotData/properties/outerConeAngle") |

### range

the range of this light in meters

`range`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-light](light-definitions-spotlightdata-properties-range.md "light.schema.json#/definitions/lightSpotData/properties/range")

#### range Type

`number`

#### range Constraints

**minimum**: the value of this number must greater than or equal to: `0`

#### range Default Value

The default value is:

```json
"Infinity"
```

### innerConeAngle

mandatory if type is Spot; the angle at which the spot light attenuation starts in radians

`innerConeAngle`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-light](light-definitions-spotlightdata-properties-innerconeangle.md "light.schema.json#/definitions/lightSpotData/properties/innerConeAngle")

#### innerConeAngle Type

`number`

#### innerConeAngle Constraints

**minimum**: the value of this number must greater than or equal to: `0`

#### innerConeAngle Default Value

The default value is:

```json
0
```

### outerConeAngle

mandatory if type is Spot; the spot light angle in radians, default is pi/4

`outerConeAngle`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-light](light-definitions-spotlightdata-properties-outerconeangle.md "light.schema.json#/definitions/lightSpotData/properties/outerConeAngle")

#### outerConeAngle Type

`number`

#### outerConeAngle Constraints

**minimum**: the value of this number must greater than or equal to: `0`

#### outerConeAngle Default Value

The default value is:

```json
0.785398
```

## Definitions group lightDirData

Reference this group by using

```json
{"$ref":"light.schema.json#/definitions/lightDirData"}
```

| Property | Type | Required | Nullable | Defined by |
| :------- | :--- | :------- | :------- | :--------- |

## Definitions group IBLData

Reference this group by using

```json
{"$ref":"light.schema.json#/definitions/IBLData"}
```

| Property                                          | Type      | Required | Nullable       | Defined by                                                                                                                                                               |
| :------------------------------------------------ | :-------- | :------- | :------------- | :----------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| [halfSize](#halfsize)                             | `array`   | Optional | cannot be null | [MSGAssets-light](vec-definitions-vec3.md "light.schema.json#/definitions/IBLData/properties/halfSize")                                                                  |
| [boxProjection](#boxprojection)                   | `boolean` | Optional | cannot be null | [MSGAssets-light](light-definitions-imagebasedlightdata-properties-boxprojection.md "light.schema.json#/definitions/IBLData/properties/boxProjection")                   |
| [specular](#specular)                             | `number`  | Optional | cannot be null | [MSGAssets-light](light-definitions-imagebasedlightdata-properties-specular.md "light.schema.json#/definitions/IBLData/properties/specular")                             |
| [specularSampler](#specularsampler)               | `number`  | Optional | cannot be null | [MSGAssets-light](light-definitions-imagebasedlightdata-properties-specularsampler.md "light.schema.json#/definitions/IBLData/properties/specularSampler")               |
| [irradianceCoefficients](#irradiancecoefficients) | `array`   | Optional | cannot be null | [MSGAssets-light](light-definitions-imagebasedlightdata-properties-irradiancecoefficients.md "light.schema.json#/definitions/IBLData/properties/irradianceCoefficients") |

### halfSize

a vector of 3 floats

`halfSize`

* is optional

* Type: `number[]`

* cannot be null

* defined in: [MSGAssets-light](vec-definitions-vec3.md "light.schema.json#/definitions/IBLData/properties/halfSize")

#### halfSize Type

`number[]`

#### halfSize Constraints

**maximum number of items**: the maximum number of items for this array is: `3`

**minimum number of items**: the minimum number of items for this array is: `3`

### boxProjection

if set to true, the light sampling will be offset using box projection

`boxProjection`

* is optional

* Type: `boolean`

* cannot be null

* defined in: [MSGAssets-light](light-definitions-imagebasedlightdata-properties-boxprojection.md "light.schema.json#/definitions/IBLData/properties/boxProjection")

#### boxProjection Type

`boolean`

#### boxProjection Default Value

The default value is:

```json
true
```

### specular

an index into the textures array to use for specular/reflection, if the texture is 2D, a cubemap will be generated

`specular`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-light](light-definitions-imagebasedlightdata-properties-specular.md "light.schema.json#/definitions/IBLData/properties/specular")

#### specular Type

`number`

#### specular Constraints

**minimum**: the value of this number must greater than or equal to: `0`

### specularSampler

an index into the samplers array to use for specular/reflection

`specularSampler`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-light](light-definitions-imagebasedlightdata-properties-specularsampler.md "light.schema.json#/definitions/IBLData/properties/specularSampler")

#### specularSampler Type

`number`

#### specularSampler Constraints

**minimum**: the value of this number must greater than or equal to: `0`

### irradianceCoefficients

the irradiance coefficients generated using spherical harmonics to get the IBL diffuse color, if absent, generated on the fly using specular texture

`irradianceCoefficients`

* is optional

* Type: `number[][]`

* cannot be null

* defined in: [MSGAssets-light](light-definitions-imagebasedlightdata-properties-irradiancecoefficients.md "light.schema.json#/definitions/IBLData/properties/irradianceCoefficients")

#### irradianceCoefficients Type

`number[][]`

#### irradianceCoefficients Constraints

**maximum number of items**: the maximum number of items for this array is: `16`

**minimum number of items**: the minimum number of items for this array is: `16`

## Definitions group lightData

Reference this group by using

```json
{"$ref":"light.schema.json#/definitions/lightData"}
```

| Property | Type | Required | Nullable | Defined by |
| :------- | :--- | :------- | :------- | :--------- |

## Definitions group punctualLight

Reference this group by using

```json
{"$ref":"light.schema.json#/definitions/punctualLight"}
```

| Property                                    | Type     | Required | Nullable       | Defined by                                                                                                                                                         |
| :------------------------------------------ | :------- | :------- | :------------- | :----------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| [copyFrom](#copyfrom)                       | `string` | Optional | cannot be null | [MSGAssets-light](external-definitions-fromentity.md "light.schema.json#/definitions/punctualLight/properties/copyFrom")                                           |
| [type](#type)                               | `string` | Required | cannot be null | [MSGAssets-light](light-definitions-punctuallight-properties-type.md "light.schema.json#/definitions/punctualLight/properties/type")                               |
| [color](#color)                             | `array`  | Optional | cannot be null | [MSGAssets-light](vec-definitions-vec3.md "light.schema.json#/definitions/punctualLight/properties/color")                                                         |
| [intensity](#intensity)                     | `number` | Optional | cannot be null | [MSGAssets-light](light-definitions-punctuallight-properties-intensity.md "light.schema.json#/definitions/punctualLight/properties/intensity")                     |
| [falloff](#falloff)                         | `number` | Optional | cannot be null | [MSGAssets-light](light-definitions-punctuallight-properties-falloff.md "light.schema.json#/definitions/punctualLight/properties/falloff")                         |
| [lightShaftIntensity](#lightshaftintensity) | `number` | Optional | cannot be null | [MSGAssets-light](light-definitions-punctuallight-properties-lightshaftintensity.md "light.schema.json#/definitions/punctualLight/properties/lightShaftIntensity") |
| [priority](#priority)                       | `number` | Optional | cannot be null | [MSGAssets-light](light-definitions-punctuallight-properties-priority.md "light.schema.json#/definitions/punctualLight/properties/priority")                       |
| [shadowSettings](#shadowsettings)           | `object` | Optional | cannot be null | [MSGAssets-light](light-definitions-shadowsettings.md "light.schema.json#/definitions/punctualLight/properties/shadowSettings")                                    |
| [data](#data)                               | Merged   | Required | cannot be null | [MSGAssets-light](light-definitions-lightdata.md "light.schema.json#/definitions/punctualLight/properties/data")                                                   |

### copyFrom

the name of the entity to copy the component from

`copyFrom`

* is optional

* Type: `string` ([FromEntity](external-definitions-fromentity.md))

* cannot be null

* defined in: [MSGAssets-light](external-definitions-fromentity.md "light.schema.json#/definitions/punctualLight/properties/copyFrom")

#### copyFrom Type

`string` ([FromEntity](external-definitions-fromentity.md))

### type

the type of light, must be: point, spot, directional or IBL

`type`

* is required

* Type: `string`

* cannot be null

* defined in: [MSGAssets-light](light-definitions-punctuallight-properties-type.md "light.schema.json#/definitions/punctualLight/properties/type")

#### type Type

`string`

#### type Constraints

**enum**: the value of this property must be equal to one of the following values:

| Value           | Explanation |
| :-------------- | :---------- |
| `"Point"`       |             |
| `"Spot"`        |             |
| `"Directional"` |             |
| `"IBL"`         |             |

### color

a vector of 3 floats

`color`

* is optional

* Type: `number[]`

* cannot be null

* defined in: [MSGAssets-light](vec-definitions-vec3.md "light.schema.json#/definitions/punctualLight/properties/color")

#### color Type

`number[]`

#### color Constraints

**maximum number of items**: the maximum number of items for this array is: `3`

**minimum number of items**: the minimum number of items for this array is: `3`

#### color Default Value

The default value is:

```json
[
  1,
  1,
  1
]
```

### intensity

the intensity of the light in lm/sr for spot and directional, lm/m² for directional, and a unitless muliplier for IBL

`intensity`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-light](light-definitions-punctuallight-properties-intensity.md "light.schema.json#/definitions/punctualLight/properties/intensity")

#### intensity Type

`number`

#### intensity Constraints

**minimum**: the value of this number must greater than or equal to: `0`

#### intensity Default Value

The default value is:

```json
1
```

### falloff

used to compute inverse square root for attenuation

`falloff`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-light](light-definitions-punctuallight-properties-falloff.md "light.schema.json#/definitions/punctualLight/properties/falloff")

#### falloff Type

`number`

#### falloff Constraints

**minimum**: the value of this number must greater than or equal to: `0`

#### falloff Default Value

The default value is:

```json
2
```

### lightShaftIntensity

defines how visible this light will be when volumetric fog is enabled

`lightShaftIntensity`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-light](light-definitions-punctuallight-properties-lightshaftintensity.md "light.schema.json#/definitions/punctualLight/properties/lightShaftIntensity")

#### lightShaftIntensity Type

`number`

#### lightShaftIntensity Constraints

**minimum**: the value of this number must greater than or equal to: `0`

#### lightShaftIntensity Default Value

The default value is:

```json
1
```

### priority

lights with higher priorities will be displayed in priority

`priority`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-light](light-definitions-punctuallight-properties-priority.md "light.schema.json#/definitions/punctualLight/properties/priority")

#### priority Type

`number`

#### priority Default Value

The default value is:

```json
0
```

### shadowSettings



`shadowSettings`

* is optional

* Type: `object` ([ShadowSettings](light-definitions-shadowsettings.md))

* cannot be null

* defined in: [MSGAssets-light](light-definitions-shadowsettings.md "light.schema.json#/definitions/punctualLight/properties/shadowSettings")

#### shadowSettings Type

`object` ([ShadowSettings](light-definitions-shadowsettings.md))

### data



`data`

* is required

* Type: `object` ([LightData](light-definitions-lightdata.md))

* cannot be null

* defined in: [MSGAssets-light](light-definitions-lightdata.md "light.schema.json#/definitions/punctualLight/properties/data")

#### data Type

`object` ([LightData](light-definitions-lightdata.md))

one (and only one) of

* [PointLightData](light-definitions-pointlightdata.md "check type definition")

* [SpotLightData](light-definitions-spotlightdata.md "check type definition")

* [DirectionalLightData](light-definitions-lightdata-oneof-directionallightdata.md "check type definition")

* [ImageBasedLightData](light-definitions-imagebasedlightdata.md "check type definition")

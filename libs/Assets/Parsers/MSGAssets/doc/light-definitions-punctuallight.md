# PunctualLight Schema

```txt
light.schema.json#/definitions/punctualLight
```

describes a punctual light, the type of the data property is defined by the value of the type property

| Abstract            | Extensible | Status         | Identifiable | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                                |
| :------------------ | :--------- | :------------- | :----------- | :---------------- | :-------------------- | :------------------ | :------------------------------------------------------------------------ |
| Can be instantiated | No         | Unknown status | No           | Forbidden         | Allowed               | none                | [light.schema.json\*](json/defs/light.schema.json "open original schema") |

## punctualLight Type

`object` ([PunctualLight](light-definitions-punctuallight.md))

# punctualLight Properties

| Property                                    | Type     | Required | Nullable       | Defined by                                                                                                                                                         |
| :------------------------------------------ | :------- | :------- | :------------- | :----------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| [copyFrom](#copyfrom)                       | `string` | Optional | cannot be null | [MSGAssets-light](external-definitions-fromentity.md "light.schema.json#/definitions/punctualLight/properties/copyFrom")                                           |
| [type](#type)                               | `string` | Optional | cannot be null | [MSGAssets-light](light-definitions-lighttype.md "light.schema.json#/definitions/punctualLight/properties/type")                                                   |
| [color](#color)                             | `array`  | Optional | cannot be null | [MSGAssets-light](vec-definitions-vec3.md "light.schema.json#/definitions/punctualLight/properties/color")                                                         |
| [intensity](#intensity)                     | `number` | Optional | cannot be null | [MSGAssets-light](light-definitions-punctuallight-properties-intensity.md "light.schema.json#/definitions/punctualLight/properties/intensity")                     |
| [falloff](#falloff)                         | `number` | Optional | cannot be null | [MSGAssets-light](light-definitions-punctuallight-properties-falloff.md "light.schema.json#/definitions/punctualLight/properties/falloff")                         |
| [lightShaftIntensity](#lightshaftintensity) | `number` | Optional | cannot be null | [MSGAssets-light](light-definitions-punctuallight-properties-lightshaftintensity.md "light.schema.json#/definitions/punctualLight/properties/lightShaftIntensity") |
| [priority](#priority)                       | `number` | Optional | cannot be null | [MSGAssets-light](light-definitions-punctuallight-properties-priority.md "light.schema.json#/definitions/punctualLight/properties/priority")                       |
| [shadowSettings](#shadowsettings)           | `object` | Optional | cannot be null | [MSGAssets-light](light-definitions-shadowsettings.md "light.schema.json#/definitions/punctualLight/properties/shadowSettings")                                    |
| [data](#data)                               | Merged   | Optional | cannot be null | [MSGAssets-light](light-definitions-lightdata.md "light.schema.json#/definitions/punctualLight/properties/data")                                                   |

## copyFrom

the name of the entity to copy the component from

`copyFrom`

* is optional

* Type: `string` ([FromEntity](external-definitions-fromentity.md))

* cannot be null

* defined in: [MSGAssets-light](external-definitions-fromentity.md "light.schema.json#/definitions/punctualLight/properties/copyFrom")

### copyFrom Type

`string` ([FromEntity](external-definitions-fromentity.md))

## type

the type of light, must be: point, spot, directional or IBL

`type`

* is optional

* Type: `string`

* cannot be null

* defined in: [MSGAssets-light](light-definitions-lighttype.md "light.schema.json#/definitions/punctualLight/properties/type")

### type Type

`string`

### type Constraints

**enum**: the value of this property must be equal to one of the following values:

| Value           | Explanation |
| :-------------- | :---------- |
| `"Point"`       |             |
| `"Spot"`        |             |
| `"Directional"` |             |
| `"IBL"`         |             |

## color

the color of the light in RGB format

`color`

* is optional

* Type: `number[]`

* cannot be null

* defined in: [MSGAssets-light](vec-definitions-vec3.md "light.schema.json#/definitions/punctualLight/properties/color")

### color Type

`number[]`

### color Constraints

**maximum number of items**: the maximum number of items for this array is: `3`

**minimum number of items**: the minimum number of items for this array is: `3`

### color Default Value

The default value is:

```json
[
  1,
  1,
  1
]
```

## intensity

the intensity of the light in lm/sr for spot and directional, lm/m² for directional, and a unitless muliplier for IBL

`intensity`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-light](light-definitions-punctuallight-properties-intensity.md "light.schema.json#/definitions/punctualLight/properties/intensity")

### intensity Type

`number`

### intensity Constraints

**minimum**: the value of this number must greater than or equal to: `0`

### intensity Default Value

The default value is:

```json
1
```

## falloff

used to compute inverse square root for attenuation

`falloff`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-light](light-definitions-punctuallight-properties-falloff.md "light.schema.json#/definitions/punctualLight/properties/falloff")

### falloff Type

`number`

### falloff Constraints

**minimum**: the value of this number must greater than or equal to: `0`

### falloff Default Value

The default value is:

```json
2
```

## lightShaftIntensity

defines how visible this light will be when volumetric fog is enabled

`lightShaftIntensity`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-light](light-definitions-punctuallight-properties-lightshaftintensity.md "light.schema.json#/definitions/punctualLight/properties/lightShaftIntensity")

### lightShaftIntensity Type

`number`

### lightShaftIntensity Constraints

**minimum**: the value of this number must greater than or equal to: `0`

### lightShaftIntensity Default Value

The default value is:

```json
1
```

## priority

lights with higher priorities will be displayed in priority

`priority`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-light](light-definitions-punctuallight-properties-priority.md "light.schema.json#/definitions/punctualLight/properties/priority")

### priority Type

`number`

### priority Default Value

The default value is:

```json
0
```

## shadowSettings



`shadowSettings`

* is optional

* Type: `object` ([ShadowSettings](light-definitions-shadowsettings.md))

* cannot be null

* defined in: [MSGAssets-light](light-definitions-shadowsettings.md "light.schema.json#/definitions/punctualLight/properties/shadowSettings")

### shadowSettings Type

`object` ([ShadowSettings](light-definitions-shadowsettings.md))

## data



`data`

* is optional

* Type: `object` ([LightData](light-definitions-lightdata.md))

* cannot be null

* defined in: [MSGAssets-light](light-definitions-lightdata.md "light.schema.json#/definitions/punctualLight/properties/data")

### data Type

`object` ([LightData](light-definitions-lightdata.md))

one (and only one) of

* [PointLightData](light-definitions-pointlightdata.md "check type definition")

* [SpotLightData](light-definitions-spotlightdata.md "check type definition")

* [DirectionalLightData](light-definitions-directionallightdata.md "check type definition")

* [ImageBasedLightData](light-definitions-imagebasedlightdata.md "check type definition")

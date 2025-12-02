# SpotLightData Schema

```txt
light.schema.json#/definitions/lightData/oneOf/1
```



| Abstract            | Extensible | Status         | Identifiable | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                                |
| :------------------ | :--------- | :------------- | :----------- | :---------------- | :-------------------- | :------------------ | :------------------------------------------------------------------------ |
| Can be instantiated | No         | Unknown status | No           | Forbidden         | Allowed               | none                | [light.schema.json\*](json/defs/light.schema.json "open original schema") |

## 1 Type

unknown ([SpotLightData](light-definitions-lightdata-oneof-spotlightdata.md))

# 1 Properties

| Property                          | Type     | Required | Nullable       | Defined by                                                                                                                                               |
| :-------------------------------- | :------- | :------- | :------------- | :------------------------------------------------------------------------------------------------------------------------------------------------------- |
| [range](#range)                   | `number` | Optional | cannot be null | [MSGAssets-light](light-definitions-spotlightdata-properties-range.md "light.schema.json#/definitions/lightSpotData/properties/range")                   |
| [innerConeAngle](#innerconeangle) | `number` | Optional | cannot be null | [MSGAssets-light](light-definitions-spotlightdata-properties-innerconeangle.md "light.schema.json#/definitions/lightSpotData/properties/innerConeAngle") |
| [outerConeAngle](#outerconeangle) | `number` | Optional | cannot be null | [MSGAssets-light](light-definitions-spotlightdata-properties-outerconeangle.md "light.schema.json#/definitions/lightSpotData/properties/outerConeAngle") |

## range

the range of this light in meters

`range`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-light](light-definitions-spotlightdata-properties-range.md "light.schema.json#/definitions/lightSpotData/properties/range")

### range Type

`number`

### range Constraints

**minimum**: the value of this number must greater than or equal to: `0`

### range Default Value

The default value is:

```json
"Infinity"
```

## innerConeAngle

mandatory if type is Spot; the angle at which the spot light attenuation starts in radians

`innerConeAngle`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-light](light-definitions-spotlightdata-properties-innerconeangle.md "light.schema.json#/definitions/lightSpotData/properties/innerConeAngle")

### innerConeAngle Type

`number`

### innerConeAngle Constraints

**minimum**: the value of this number must greater than or equal to: `0`

### innerConeAngle Default Value

The default value is:

```json
0
```

## outerConeAngle

mandatory if type is Spot; the spot light angle in radians, default is pi/4

`outerConeAngle`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-light](light-definitions-spotlightdata-properties-outerconeangle.md "light.schema.json#/definitions/lightSpotData/properties/outerConeAngle")

### outerConeAngle Type

`number`

### outerConeAngle Constraints

**minimum**: the value of this number must greater than or equal to: `0`

### outerConeAngle Default Value

The default value is:

```json
0.785398
```

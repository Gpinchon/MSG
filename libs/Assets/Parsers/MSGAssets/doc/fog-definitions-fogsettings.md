# FogSettings Schema

```txt
fog.schema.json#/definitions/fogSettings
```



| Abstract            | Extensible | Status         | Identifiable | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                            |
| :------------------ | :--------- | :------------- | :----------- | :---------------- | :-------------------- | :------------------ | :-------------------------------------------------------------------- |
| Can be instantiated | No         | Unknown status | No           | Forbidden         | Allowed               | none                | [fog.schema.json\*](json/defs/fog.schema.json "open original schema") |

## fogSettings Type

`object` ([FogSettings](fog-definitions-fogsettings.md))

# fogSettings Properties

| Property                                  | Type      | Required | Nullable       | Defined by                                                                                                                                   |
| :---------------------------------------- | :-------- | :------- | :------------- | :------------------------------------------------------------------------------------------------------------------------------------------- |
| [scattering](#scattering)                 | `array`   | Optional | cannot be null | [MSGAssets-fog](vec-definitions-vec3.md "fog.schema.json#/definitions/fogSettings/properties/scattering")                                    |
| [emissive](#emissive)                     | `array`   | Optional | cannot be null | [MSGAssets-fog](vec-definitions-vec3.md "fog.schema.json#/definitions/fogSettings/properties/emissive")                                      |
| [phaseG](#phaseg)                         | `number`  | Optional | cannot be null | [MSGAssets-fog](fog-definitions-fogsettings-properties-phaseg.md "fog.schema.json#/definitions/fogSettings/properties/phaseG")               |
| [extinction](#extinction)                 | `number`  | Optional | cannot be null | [MSGAssets-fog](fog-definitions-fogsettings-properties-extinction.md "fog.schema.json#/definitions/fogSettings/properties/extinction")       |
| [fogBackground](#fogbackground)           | `boolean` | Optional | cannot be null | [MSGAssets-fog](fog-definitions-fogsettings-properties-fogbackground.md "fog.schema.json#/definitions/fogSettings/properties/fogBackground") |
| [volumetricSettings](#volumetricsettings) | `object`  | Optional | cannot be null | [MSGAssets-fog](fog-definitions-volumetricfogsettings.md "fog.schema.json#/definitions/fogSettings/properties/volumetricSettings")           |

## scattering



`scattering`

* is optional

* Type: `number[]`

* cannot be null

* defined in: [MSGAssets-fog](vec-definitions-vec3.md "fog.schema.json#/definitions/fogSettings/properties/scattering")

### scattering Type

`number[]`

### scattering Constraints

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



`emissive`

* is optional

* Type: `number[]`

* cannot be null

* defined in: [MSGAssets-fog](vec-definitions-vec3.md "fog.schema.json#/definitions/fogSettings/properties/emissive")

### emissive Type

`number[]`

### emissive Constraints

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

## phaseG



`phaseG`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-fog](fog-definitions-fogsettings-properties-phaseg.md "fog.schema.json#/definitions/fogSettings/properties/phaseG")

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

## extinction



`extinction`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-fog](fog-definitions-fogsettings-properties-extinction.md "fog.schema.json#/definitions/fogSettings/properties/extinction")

### extinction Type

`number`

### extinction Constraints

**maximum**: the value of this number must smaller than or equal to: `1`

**minimum**: the value of this number must greater than or equal to: `0`

### extinction Default Value

The default value is:

```json
0.025
```

## fogBackground



`fogBackground`

* is optional

* Type: `boolean`

* cannot be null

* defined in: [MSGAssets-fog](fog-definitions-fogsettings-properties-fogbackground.md "fog.schema.json#/definitions/fogSettings/properties/fogBackground")

### fogBackground Type

`boolean`

### fogBackground Default Value

The default value is:

```json
true
```

## volumetricSettings

it is highly recommended to customize these settings depending on your scene

`volumetricSettings`

* is optional

* Type: `object` ([VolumetricFogSettings](fog-definitions-volumetricfogsettings.md))

* cannot be null

* defined in: [MSGAssets-fog](fog-definitions-volumetricfogsettings.md "fog.schema.json#/definitions/fogSettings/properties/volumetricSettings")

### volumetricSettings Type

`object` ([VolumetricFogSettings](fog-definitions-volumetricfogsettings.md))

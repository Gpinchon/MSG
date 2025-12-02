# MaterialMetallicRoughnessExtension Schema

```txt
material.schema.json#/definitions/metallicRoughnessExtension
```

this allows for the metallic roughness workflow

| Abstract            | Extensible | Status         | Identifiable | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                                      |
| :------------------ | :--------- | :------------- | :----------- | :---------------- | :-------------------- | :------------------ | :------------------------------------------------------------------------------ |
| Can be instantiated | No         | Unknown status | No           | Forbidden         | Allowed               | none                | [material.schema.json\*](json/defs/material.schema.json "open original schema") |

## metallicRoughnessExtension Type

`object` ([MaterialMetallicRoughnessExtension](material-definitions-materialmetallicroughnessextension.md))

# metallicRoughnessExtension Properties

| Property                                                      | Type     | Required | Nullable       | Defined by                                                                                                                                                                                            |
| :------------------------------------------------------------ | :------- | :------- | :------------- | :---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| [colorTextureInfo](#colortextureinfo)                         | `object` | Optional | cannot be null | [MSGAssets-material](material-definitions-materialtextureinfo.md "material.schema.json#/definitions/metallicRoughnessExtension/properties/colorTextureInfo")                                          |
| [metallicRoughnessTextureInfo](#metallicroughnesstextureinfo) | `object` | Optional | cannot be null | [MSGAssets-material](material-definitions-materialtextureinfo.md "material.schema.json#/definitions/metallicRoughnessExtension/properties/metallicRoughnessTextureInfo")                              |
| [colorFactor](#colorfactor)                                   | `array`  | Optional | cannot be null | [MSGAssets-material](vec-definitions-vec4.md "material.schema.json#/definitions/metallicRoughnessExtension/properties/colorFactor")                                                                   |
| [metallicFactor](#metallicfactor)                             | `number` | Optional | cannot be null | [MSGAssets-material](material-definitions-materialmetallicroughnessextension-properties-metallicfactor.md "material.schema.json#/definitions/metallicRoughnessExtension/properties/metallicFactor")   |
| [roughnessFactor](#roughnessfactor)                           | `number` | Optional | cannot be null | [MSGAssets-material](material-definitions-materialmetallicroughnessextension-properties-roughnessfactor.md "material.schema.json#/definitions/metallicRoughnessExtension/properties/roughnessFactor") |

## colorTextureInfo

the color/albedo texture info

`colorTextureInfo`

* is optional

* Type: `object` ([MaterialTextureInfo](material-definitions-materialtextureinfo.md))

* cannot be null

* defined in: [MSGAssets-material](material-definitions-materialtextureinfo.md "material.schema.json#/definitions/metallicRoughnessExtension/properties/colorTextureInfo")

### colorTextureInfo Type

`object` ([MaterialTextureInfo](material-definitions-materialtextureinfo.md))

## metallicRoughnessTextureInfo

the metallic/roughness texture info; R=metallic, G=roughness

`metallicRoughnessTextureInfo`

* is optional

* Type: `object` ([MaterialTextureInfo](material-definitions-materialtextureinfo.md))

* cannot be null

* defined in: [MSGAssets-material](material-definitions-materialtextureinfo.md "material.schema.json#/definitions/metallicRoughnessExtension/properties/metallicRoughnessTextureInfo")

### metallicRoughnessTextureInfo Type

`object` ([MaterialTextureInfo](material-definitions-materialtextureinfo.md))

## colorFactor

a vector of 4 floats

`colorFactor`

* is optional

* Type: `number[]`

* cannot be null

* defined in: [MSGAssets-material](vec-definitions-vec4.md "material.schema.json#/definitions/metallicRoughnessExtension/properties/colorFactor")

### colorFactor Type

`number[]`

### colorFactor Constraints

**maximum number of items**: the maximum number of items for this array is: `4`

**minimum number of items**: the minimum number of items for this array is: `4`

### colorFactor Default Value

The default value is:

```json
[
  1,
  1,
  1,
  1
]
```

## metallicFactor

the metalness of this material, if metallicRoughnessTexture is present: used as a multiplier

`metallicFactor`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-material](material-definitions-materialmetallicroughnessextension-properties-metallicfactor.md "material.schema.json#/definitions/metallicRoughnessExtension/properties/metallicFactor")

### metallicFactor Type

`number`

### metallicFactor Constraints

**minimum**: the value of this number must greater than or equal to: `0`

### metallicFactor Default Value

The default value is:

```json
1
```

## roughnessFactor

the roughness of this material, if metallicRoughnessTexture is present: used as a multiplier

`roughnessFactor`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-material](material-definitions-materialmetallicroughnessextension-properties-roughnessfactor.md "material.schema.json#/definitions/metallicRoughnessExtension/properties/roughnessFactor")

### roughnessFactor Type

`number`

### roughnessFactor Constraints

**minimum**: the value of this number must greater than or equal to: `0`

### roughnessFactor Default Value

The default value is:

```json
1
```

# MaterialBaseExtension Schema

```txt
material.schema.json#/definitions/baseExtension
```

this material's base extension

| Abstract            | Extensible | Status         | Identifiable | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                                      |
| :------------------ | :--------- | :------------- | :----------- | :---------------- | :-------------------- | :------------------ | :------------------------------------------------------------------------------ |
| Can be instantiated | No         | Unknown status | No           | Forbidden         | Allowed               | none                | [material.schema.json\*](json/defs/material.schema.json "open original schema") |

## baseExtension Type

`object` ([MaterialBaseExtension](material-definitions-materialbaseextension.md))

# baseExtension Properties

| Property                                      | Type      | Required | Nullable       | Defined by                                                                                                                                                          |
| :-------------------------------------------- | :-------- | :------- | :------------- | :------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| [normalTextureInfo](#normaltextureinfo)       | `object`  | Optional | cannot be null | [MSGAssets-material](material-definitions-materialnormaltextureinfo.md "material.schema.json#/definitions/baseExtension/properties/normalTextureInfo")              |
| [occlusionTextureInfo](#occlusiontextureinfo) | `object`  | Optional | cannot be null | [MSGAssets-material](material-definitions-materialocclusiontextureinfo.md "material.schema.json#/definitions/baseExtension/properties/occlusionTextureInfo")        |
| [emissiveTexture](#emissivetexture)           | `object`  | Optional | cannot be null | [MSGAssets-material](material-definitions-materialtextureinfo.md "material.schema.json#/definitions/baseExtension/properties/emissiveTexture")                      |
| [emissiveFactor](#emissivefactor)             | `array`   | Optional | cannot be null | [MSGAssets-material](vec-definitions-vec3.md "material.schema.json#/definitions/baseExtension/properties/emissiveFactor")                                           |
| [alphaMode](#alphamode)                       | `string`  | Optional | cannot be null | [MSGAssets-material](material-definitions-materialalphamode.md "material.schema.json#/definitions/baseExtension/properties/alphaMode")                              |
| [alphaCutoff](#alphacutoff)                   | `number`  | Optional | cannot be null | [MSGAssets-material](material-definitions-materialbaseextension-properties-alphacutoff.md "material.schema.json#/definitions/baseExtension/properties/alphaCutoff") |
| [doubleSided](#doublesided)                   | `boolean` | Optional | cannot be null | [MSGAssets-material](material-definitions-materialbaseextension-properties-doublesided.md "material.schema.json#/definitions/baseExtension/properties/doubleSided") |
| [unlit](#unlit)                               | `boolean` | Optional | cannot be null | [MSGAssets-material](material-definitions-materialbaseextension-properties-unlit.md "material.schema.json#/definitions/baseExtension/properties/unlit")             |

## normalTextureInfo

the normal map texture info

`normalTextureInfo`

* is optional

* Type: `object` ([MaterialNormalTextureInfo](material-definitions-materialnormaltextureinfo.md))

* cannot be null

* defined in: [MSGAssets-material](material-definitions-materialnormaltextureinfo.md "material.schema.json#/definitions/baseExtension/properties/normalTextureInfo")

### normalTextureInfo Type

`object` ([MaterialNormalTextureInfo](material-definitions-materialnormaltextureinfo.md))

## occlusionTextureInfo

the occlusion texture info

`occlusionTextureInfo`

* is optional

* Type: `object` ([MaterialOcclusionTextureInfo](material-definitions-materialocclusiontextureinfo.md))

* cannot be null

* defined in: [MSGAssets-material](material-definitions-materialocclusiontextureinfo.md "material.schema.json#/definitions/baseExtension/properties/occlusionTextureInfo")

### occlusionTextureInfo Type

`object` ([MaterialOcclusionTextureInfo](material-definitions-materialocclusiontextureinfo.md))

## emissiveTexture

the emissive texture info

`emissiveTexture`

* is optional

* Type: `object` ([MaterialTextureInfo](material-definitions-materialtextureinfo.md))

* cannot be null

* defined in: [MSGAssets-material](material-definitions-materialtextureinfo.md "material.schema.json#/definitions/baseExtension/properties/emissiveTexture")

### emissiveTexture Type

`object` ([MaterialTextureInfo](material-definitions-materialtextureinfo.md))

## emissiveFactor

a vector of 3 floats

`emissiveFactor`

* is optional

* Type: `number[]`

* cannot be null

* defined in: [MSGAssets-material](vec-definitions-vec3.md "material.schema.json#/definitions/baseExtension/properties/emissiveFactor")

### emissiveFactor Type

`number[]`

### emissiveFactor Constraints

**maximum number of items**: the maximum number of items for this array is: `3`

**minimum number of items**: the minimum number of items for this array is: `3`

### emissiveFactor Default Value

The default value is:

```json
[
  0,
  0,
  0
]
```

## alphaMode

is this material opaque, blended or uses an alpha mask ?

`alphaMode`

* is optional

* Type: `string` ([MaterialAlphaMode](material-definitions-materialalphamode.md))

* cannot be null

* defined in: [MSGAssets-material](material-definitions-materialalphamode.md "material.schema.json#/definitions/baseExtension/properties/alphaMode")

### alphaMode Type

`string` ([MaterialAlphaMode](material-definitions-materialalphamode.md))

### alphaMode Constraints

**enum**: the value of this property must be equal to one of the following values:

| Value      | Explanation |
| :--------- | :---------- |
| `"Opaque"` |             |
| `"Mask"`   |             |
| `"Blend"`  |             |

### alphaMode Default Value

The default value is:

```json
"Opaque"
```

## alphaCutoff

when using Mask alpha mode, every pixels of this material under this alpha cutoff is discarded

`alphaCutoff`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-material](material-definitions-materialbaseextension-properties-alphacutoff.md "material.schema.json#/definitions/baseExtension/properties/alphaCutoff")

### alphaCutoff Type

`number`

### alphaCutoff Constraints

**maximum**: the value of this number must smaller than or equal to: `1`

**minimum**: the value of this number must greater than or equal to: `0`

### alphaCutoff Default Value

The default value is:

```json
0.5
```

## doubleSided

is this material double sided ? If not backface culling is applied

`doubleSided`

* is optional

* Type: `boolean`

* cannot be null

* defined in: [MSGAssets-material](material-definitions-materialbaseextension-properties-doublesided.md "material.schema.json#/definitions/baseExtension/properties/doubleSided")

### doubleSided Type

`boolean`

### doubleSided Default Value

The default value is:

```json
false
```

## unlit

if true this material won't take any lighting

`unlit`

* is optional

* Type: `boolean`

* cannot be null

* defined in: [MSGAssets-material](material-definitions-materialbaseextension-properties-unlit.md "material.schema.json#/definitions/baseExtension/properties/unlit")

### unlit Type

`boolean`

### unlit Default Value

The default value is:

```json
false
```

# MSGAssets-material Schema

```txt
material.schema.json
```



| Abstract               | Extensible | Status         | Identifiable            | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                                    |
| :--------------------- | :--------- | :------------- | :---------------------- | :---------------- | :-------------------- | :------------------ | :---------------------------------------------------------------------------- |
| Cannot be instantiated | Yes        | Unknown status | Unknown identifiability | Forbidden         | Allowed               | none                | [material.schema.json](json/defs/material.schema.json "open original schema") |

## MSGAssets-material Type

`object` ([MSGAssets-material](material.md))

# MSGAssets-material Definitions

## Definitions group materialAlphaMode

Reference this group by using

```json
{"$ref":"material.schema.json#/definitions/materialAlphaMode"}
```

| Property | Type | Required | Nullable | Defined by |
| :------- | :--- | :------- | :------- | :--------- |

## Definitions group textureInfo

Reference this group by using

```json
{"$ref":"material.schema.json#/definitions/textureInfo"}
```

| Property                                  | Type      | Required | Nullable       | Defined by                                                                                                                                                                    |
| :---------------------------------------- | :-------- | :------- | :------------- | :---------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| [texture](#texture)                       | `integer` | Required | cannot be null | [MSGAssets-material](material-definitions-materialtextureinfo-properties-texture.md "material.schema.json#/definitions/textureInfo/properties/texture")                       |
| [sampler](#sampler)                       | `integer` | Optional | cannot be null | [MSGAssets-material](material-definitions-materialtextureinfo-properties-sampler.md "material.schema.json#/definitions/textureInfo/properties/sampler")                       |
| [textureCoordinates](#texturecoordinates) | `integer` | Optional | cannot be null | [MSGAssets-material](material-definitions-materialtextureinfo-properties-texturecoordinates.md "material.schema.json#/definitions/textureInfo/properties/textureCoordinates") |
| [transformScale](#transformscale)         | `array`   | Optional | cannot be null | [MSGAssets-material](vec-definitions-vec2.md "material.schema.json#/definitions/textureInfo/properties/transformScale")                                                       |
| [transformOffset](#transformoffset)       | `array`   | Optional | cannot be null | [MSGAssets-material](vec-definitions-vec2.md "material.schema.json#/definitions/textureInfo/properties/transformOffset")                                                      |
| [transformRotation](#transformrotation)   | `number`  | Optional | cannot be null | [MSGAssets-material](material-definitions-materialtextureinfo-properties-transformrotation.md "material.schema.json#/definitions/textureInfo/properties/transformRotation")   |

### texture

an index into the textures list

`texture`

* is required

* Type: `integer`

* cannot be null

* defined in: [MSGAssets-material](material-definitions-materialtextureinfo-properties-texture.md "material.schema.json#/definitions/textureInfo/properties/texture")

#### texture Type

`integer`

#### texture Constraints

**minimum**: the value of this number must greater than or equal to: `0`

### sampler

an index into the samplers list

`sampler`

* is optional

* Type: `integer`

* cannot be null

* defined in: [MSGAssets-material](material-definitions-materialtextureinfo-properties-sampler.md "material.schema.json#/definitions/textureInfo/properties/sampler")

#### sampler Type

`integer`

#### sampler Constraints

**minimum**: the value of this number must greater than or equal to: `0`

### textureCoordinates

the index of texture coordinates to use

`textureCoordinates`

* is optional

* Type: `integer`

* cannot be null

* defined in: [MSGAssets-material](material-definitions-materialtextureinfo-properties-texturecoordinates.md "material.schema.json#/definitions/textureInfo/properties/textureCoordinates")

#### textureCoordinates Type

`integer`

#### textureCoordinates Constraints

**maximum**: the value of this number must smaller than or equal to: `3`

**minimum**: the value of this number must greater than or equal to: `0`

#### textureCoordinates Default Value

The default value is:

```json
0
```

### transformScale

the texture coordinates scale

`transformScale`

* is optional

* Type: `number[]`

* cannot be null

* defined in: [MSGAssets-material](vec-definitions-vec2.md "material.schema.json#/definitions/textureInfo/properties/transformScale")

#### transformScale Type

`number[]`

#### transformScale Constraints

**maximum number of items**: the maximum number of items for this array is: `2`

**minimum number of items**: the minimum number of items for this array is: `2`

#### transformScale Default Value

The default value is:

```json
[
  1,
  1
]
```

### transformOffset

the texture coordinates offset

`transformOffset`

* is optional

* Type: `number[]`

* cannot be null

* defined in: [MSGAssets-material](vec-definitions-vec2.md "material.schema.json#/definitions/textureInfo/properties/transformOffset")

#### transformOffset Type

`number[]`

#### transformOffset Constraints

**maximum number of items**: the maximum number of items for this array is: `2`

**minimum number of items**: the minimum number of items for this array is: `2`

#### transformOffset Default Value

The default value is:

```json
[
  0,
  0
]
```

### transformRotation

the texture coordinates rotation in radians

`transformRotation`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-material](material-definitions-materialtextureinfo-properties-transformrotation.md "material.schema.json#/definitions/textureInfo/properties/transformRotation")

#### transformRotation Type

`number`

#### transformRotation Default Value

The default value is:

```json
0
```

## Definitions group normalTextureInfo

Reference this group by using

```json
{"$ref":"material.schema.json#/definitions/normalTextureInfo"}
```

| Property                                    | Type      | Required | Nullable       | Defined by                                                                                                                                                                                |
| :------------------------------------------ | :-------- | :------- | :------------- | :---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| [texture](#texture-1)                       | `integer` | Required | cannot be null | [MSGAssets-material](material-definitions-materialnormaltextureinfo-properties-texture.md "material.schema.json#/definitions/normalTextureInfo/properties/texture")                       |
| [sampler](#sampler-1)                       | `integer` | Optional | cannot be null | [MSGAssets-material](material-definitions-materialnormaltextureinfo-properties-sampler.md "material.schema.json#/definitions/normalTextureInfo/properties/sampler")                       |
| [textureCoordinates](#texturecoordinates-1) | `integer` | Optional | cannot be null | [MSGAssets-material](material-definitions-materialnormaltextureinfo-properties-texturecoordinates.md "material.schema.json#/definitions/normalTextureInfo/properties/textureCoordinates") |
| [transformScale](#transformscale-1)         | `array`   | Optional | cannot be null | [MSGAssets-material](vec-definitions-vec2.md "material.schema.json#/definitions/normalTextureInfo/properties/transformScale")                                                             |
| [transformOffset](#transformoffset-1)       | `array`   | Optional | cannot be null | [MSGAssets-material](vec-definitions-vec2.md "material.schema.json#/definitions/normalTextureInfo/properties/transformOffset")                                                            |
| [transformRotation](#transformrotation-1)   | `number`  | Optional | cannot be null | [MSGAssets-material](material-definitions-materialnormaltextureinfo-properties-transformrotation.md "material.schema.json#/definitions/normalTextureInfo/properties/transformRotation")   |
| [scale](#scale)                             | `number`  | Optional | cannot be null | [MSGAssets-material](material-definitions-materialnormaltextureinfo-properties-scale.md "material.schema.json#/definitions/normalTextureInfo/properties/scale")                           |

### texture

an index into the textures list

`texture`

* is required

* Type: `integer`

* cannot be null

* defined in: [MSGAssets-material](material-definitions-materialnormaltextureinfo-properties-texture.md "material.schema.json#/definitions/normalTextureInfo/properties/texture")

#### texture Type

`integer`

#### texture Constraints

**minimum**: the value of this number must greater than or equal to: `0`

### sampler

an index into the samplers list

`sampler`

* is optional

* Type: `integer`

* cannot be null

* defined in: [MSGAssets-material](material-definitions-materialnormaltextureinfo-properties-sampler.md "material.schema.json#/definitions/normalTextureInfo/properties/sampler")

#### sampler Type

`integer`

#### sampler Constraints

**minimum**: the value of this number must greater than or equal to: `0`

### textureCoordinates

the index of texture coordinates to use

`textureCoordinates`

* is optional

* Type: `integer`

* cannot be null

* defined in: [MSGAssets-material](material-definitions-materialnormaltextureinfo-properties-texturecoordinates.md "material.schema.json#/definitions/normalTextureInfo/properties/textureCoordinates")

#### textureCoordinates Type

`integer`

#### textureCoordinates Constraints

**maximum**: the value of this number must smaller than or equal to: `3`

**minimum**: the value of this number must greater than or equal to: `0`

#### textureCoordinates Default Value

The default value is:

```json
0
```

### transformScale

the texture coordinates scale

`transformScale`

* is optional

* Type: `number[]`

* cannot be null

* defined in: [MSGAssets-material](vec-definitions-vec2.md "material.schema.json#/definitions/normalTextureInfo/properties/transformScale")

#### transformScale Type

`number[]`

#### transformScale Constraints

**maximum number of items**: the maximum number of items for this array is: `2`

**minimum number of items**: the minimum number of items for this array is: `2`

#### transformScale Default Value

The default value is:

```json
[
  1,
  1
]
```

### transformOffset

the texture coordinates offset

`transformOffset`

* is optional

* Type: `number[]`

* cannot be null

* defined in: [MSGAssets-material](vec-definitions-vec2.md "material.schema.json#/definitions/normalTextureInfo/properties/transformOffset")

#### transformOffset Type

`number[]`

#### transformOffset Constraints

**maximum number of items**: the maximum number of items for this array is: `2`

**minimum number of items**: the minimum number of items for this array is: `2`

#### transformOffset Default Value

The default value is:

```json
[
  0,
  0
]
```

### transformRotation

the texture coordinates rotation in radians

`transformRotation`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-material](material-definitions-materialnormaltextureinfo-properties-transformrotation.md "material.schema.json#/definitions/normalTextureInfo/properties/transformRotation")

#### transformRotation Type

`number`

#### transformRotation Default Value

The default value is:

```json
0
```

### scale

the normal map scale

`scale`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-material](material-definitions-materialnormaltextureinfo-properties-scale.md "material.schema.json#/definitions/normalTextureInfo/properties/scale")

#### scale Type

`number`

#### scale Default Value

The default value is:

```json
1
```

## Definitions group occlusionTextureInfo

Reference this group by using

```json
{"$ref":"material.schema.json#/definitions/occlusionTextureInfo"}
```

| Property                                    | Type      | Required | Nullable       | Defined by                                                                                                                                                                                      |
| :------------------------------------------ | :-------- | :------- | :------------- | :---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| [texture](#texture-2)                       | `integer` | Optional | cannot be null | [MSGAssets-material](material-definitions-materialocclusiontextureinfo-properties-texture.md "material.schema.json#/definitions/occlusionTextureInfo/properties/texture")                       |
| [sampler](#sampler-2)                       | `integer` | Optional | cannot be null | [MSGAssets-material](material-definitions-materialocclusiontextureinfo-properties-sampler.md "material.schema.json#/definitions/occlusionTextureInfo/properties/sampler")                       |
| [textureCoordinates](#texturecoordinates-2) | `integer` | Optional | cannot be null | [MSGAssets-material](material-definitions-materialocclusiontextureinfo-properties-texturecoordinates.md "material.schema.json#/definitions/occlusionTextureInfo/properties/textureCoordinates") |
| [transformScale](#transformscale-2)         | `array`   | Optional | cannot be null | [MSGAssets-material](vec-definitions-vec2.md "material.schema.json#/definitions/occlusionTextureInfo/properties/transformScale")                                                                |
| [transformOffset](#transformoffset-2)       | `array`   | Optional | cannot be null | [MSGAssets-material](vec-definitions-vec2.md "material.schema.json#/definitions/occlusionTextureInfo/properties/transformOffset")                                                               |
| [transformRotation](#transformrotation-2)   | `number`  | Optional | cannot be null | [MSGAssets-material](material-definitions-materialocclusiontextureinfo-properties-transformrotation.md "material.schema.json#/definitions/occlusionTextureInfo/properties/transformRotation")   |
| [strength](#strength)                       | `number`  | Optional | cannot be null | [MSGAssets-material](material-definitions-materialocclusiontextureinfo-properties-strength.md "material.schema.json#/definitions/occlusionTextureInfo/properties/strength")                     |

### texture

an index into the textures list

`texture`

* is optional

* Type: `integer`

* cannot be null

* defined in: [MSGAssets-material](material-definitions-materialocclusiontextureinfo-properties-texture.md "material.schema.json#/definitions/occlusionTextureInfo/properties/texture")

#### texture Type

`integer`

#### texture Constraints

**minimum**: the value of this number must greater than or equal to: `0`

### sampler

an index into the samplers list

`sampler`

* is optional

* Type: `integer`

* cannot be null

* defined in: [MSGAssets-material](material-definitions-materialocclusiontextureinfo-properties-sampler.md "material.schema.json#/definitions/occlusionTextureInfo/properties/sampler")

#### sampler Type

`integer`

#### sampler Constraints

**minimum**: the value of this number must greater than or equal to: `0`

### textureCoordinates

the index of texture coordinates to use

`textureCoordinates`

* is optional

* Type: `integer`

* cannot be null

* defined in: [MSGAssets-material](material-definitions-materialocclusiontextureinfo-properties-texturecoordinates.md "material.schema.json#/definitions/occlusionTextureInfo/properties/textureCoordinates")

#### textureCoordinates Type

`integer`

#### textureCoordinates Constraints

**maximum**: the value of this number must smaller than or equal to: `3`

**minimum**: the value of this number must greater than or equal to: `0`

#### textureCoordinates Default Value

The default value is:

```json
0
```

### transformScale

the texture coordinates scale

`transformScale`

* is optional

* Type: `number[]`

* cannot be null

* defined in: [MSGAssets-material](vec-definitions-vec2.md "material.schema.json#/definitions/occlusionTextureInfo/properties/transformScale")

#### transformScale Type

`number[]`

#### transformScale Constraints

**maximum number of items**: the maximum number of items for this array is: `2`

**minimum number of items**: the minimum number of items for this array is: `2`

#### transformScale Default Value

The default value is:

```json
[
  1,
  1
]
```

### transformOffset

the texture coordinates offset

`transformOffset`

* is optional

* Type: `number[]`

* cannot be null

* defined in: [MSGAssets-material](vec-definitions-vec2.md "material.schema.json#/definitions/occlusionTextureInfo/properties/transformOffset")

#### transformOffset Type

`number[]`

#### transformOffset Constraints

**maximum number of items**: the maximum number of items for this array is: `2`

**minimum number of items**: the minimum number of items for this array is: `2`

#### transformOffset Default Value

The default value is:

```json
[
  0,
  0
]
```

### transformRotation

the texture coordinates rotation in radians

`transformRotation`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-material](material-definitions-materialocclusiontextureinfo-properties-transformrotation.md "material.schema.json#/definitions/occlusionTextureInfo/properties/transformRotation")

#### transformRotation Type

`number`

#### transformRotation Default Value

The default value is:

```json
0
```

### strength

the occlusion strength

`strength`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-material](material-definitions-materialocclusiontextureinfo-properties-strength.md "material.schema.json#/definitions/occlusionTextureInfo/properties/strength")

#### strength Type

`number`

#### strength Default Value

The default value is:

```json
1
```

## Definitions group baseExtension

Reference this group by using

```json
{"$ref":"material.schema.json#/definitions/baseExtension"}
```

| Property                                      | Type      | Required | Nullable       | Defined by                                                                                                                                                              |
| :-------------------------------------------- | :-------- | :------- | :------------- | :---------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| [normalTextureInfo](#normaltextureinfo)       | `object`  | Optional | cannot be null | [MSGAssets-material](material-definitions-materialnormaltextureinfo.md "material.schema.json#/definitions/baseExtension/properties/normalTextureInfo")                  |
| [occlusionTextureInfo](#occlusiontextureinfo) | `object`  | Optional | cannot be null | [MSGAssets-material](material-definitions-materialocclusiontextureinfo.md "material.schema.json#/definitions/baseExtension/properties/occlusionTextureInfo")            |
| [emissiveTexture](#emissivetexture)           | `object`  | Optional | cannot be null | [MSGAssets-material](material-definitions-materialtextureinfo.md "material.schema.json#/definitions/baseExtension/properties/emissiveTexture")                          |
| [emissiveFactor](#emissivefactor)             | `array`   | Optional | cannot be null | [MSGAssets-material](vec-definitions-vec3.md "material.schema.json#/definitions/baseExtension/properties/emissiveFactor")                                               |
| [alphaMode](#alphamode)                       | `string`  | Optional | cannot be null | [MSGAssets-material](material-definitions-materialbaseextension-properties-materialalphamode.md "material.schema.json#/definitions/baseExtension/properties/alphaMode") |
| [alphaCutoff](#alphacutoff)                   | `number`  | Optional | cannot be null | [MSGAssets-material](material-definitions-materialbaseextension-properties-alphacutoff.md "material.schema.json#/definitions/baseExtension/properties/alphaCutoff")     |
| [doubleSided](#doublesided)                   | `boolean` | Optional | cannot be null | [MSGAssets-material](material-definitions-materialbaseextension-properties-doublesided.md "material.schema.json#/definitions/baseExtension/properties/doubleSided")     |
| [unlit](#unlit)                               | `boolean` | Optional | cannot be null | [MSGAssets-material](material-definitions-materialbaseextension-properties-unlit.md "material.schema.json#/definitions/baseExtension/properties/unlit")                 |

### normalTextureInfo

the normal map texture info

`normalTextureInfo`

* is optional

* Type: `object` ([MaterialNormalTextureInfo](material-definitions-materialnormaltextureinfo.md))

* cannot be null

* defined in: [MSGAssets-material](material-definitions-materialnormaltextureinfo.md "material.schema.json#/definitions/baseExtension/properties/normalTextureInfo")

#### normalTextureInfo Type

`object` ([MaterialNormalTextureInfo](material-definitions-materialnormaltextureinfo.md))

### occlusionTextureInfo

the occlusion texture info

`occlusionTextureInfo`

* is optional

* Type: `object` ([MaterialOcclusionTextureInfo](material-definitions-materialocclusiontextureinfo.md))

* cannot be null

* defined in: [MSGAssets-material](material-definitions-materialocclusiontextureinfo.md "material.schema.json#/definitions/baseExtension/properties/occlusionTextureInfo")

#### occlusionTextureInfo Type

`object` ([MaterialOcclusionTextureInfo](material-definitions-materialocclusiontextureinfo.md))

### emissiveTexture

the emissive texture info

`emissiveTexture`

* is optional

* Type: `object` ([MaterialTextureInfo](material-definitions-materialtextureinfo.md))

* cannot be null

* defined in: [MSGAssets-material](material-definitions-materialtextureinfo.md "material.schema.json#/definitions/baseExtension/properties/emissiveTexture")

#### emissiveTexture Type

`object` ([MaterialTextureInfo](material-definitions-materialtextureinfo.md))

### emissiveFactor

how emissive this material is in RGB format, used as a multiplier if emissiveTexture is present

`emissiveFactor`

* is optional

* Type: `number[]`

* cannot be null

* defined in: [MSGAssets-material](vec-definitions-vec3.md "material.schema.json#/definitions/baseExtension/properties/emissiveFactor")

#### emissiveFactor Type

`number[]`

#### emissiveFactor Constraints

**maximum number of items**: the maximum number of items for this array is: `3`

**minimum number of items**: the minimum number of items for this array is: `3`

#### emissiveFactor Default Value

The default value is:

```json
[
  0,
  0,
  0
]
```

### alphaMode

is this material opaque, blended or uses an alpha mask ?

`alphaMode`

* is optional

* Type: `string` ([MaterialAlphaMode](material-definitions-materialbaseextension-properties-materialalphamode.md))

* cannot be null

* defined in: [MSGAssets-material](material-definitions-materialbaseextension-properties-materialalphamode.md "material.schema.json#/definitions/baseExtension/properties/alphaMode")

#### alphaMode Type

`string` ([MaterialAlphaMode](material-definitions-materialbaseextension-properties-materialalphamode.md))

#### alphaMode Constraints

**enum**: the value of this property must be equal to one of the following values:

| Value      | Explanation |
| :--------- | :---------- |
| `"Opaque"` |             |
| `"Mask"`   |             |
| `"Blend"`  |             |

#### alphaMode Default Value

The default value is:

```json
"Opaque"
```

### alphaCutoff

when using Mask alpha mode, every pixels of this material under this alpha cutoff is discarded

`alphaCutoff`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-material](material-definitions-materialbaseextension-properties-alphacutoff.md "material.schema.json#/definitions/baseExtension/properties/alphaCutoff")

#### alphaCutoff Type

`number`

#### alphaCutoff Constraints

**maximum**: the value of this number must smaller than or equal to: `1`

**minimum**: the value of this number must greater than or equal to: `0`

#### alphaCutoff Default Value

The default value is:

```json
0.5
```

### doubleSided

is this material double sided ? If not backface culling is applied

`doubleSided`

* is optional

* Type: `boolean`

* cannot be null

* defined in: [MSGAssets-material](material-definitions-materialbaseextension-properties-doublesided.md "material.schema.json#/definitions/baseExtension/properties/doubleSided")

#### doubleSided Type

`boolean`

#### doubleSided Default Value

The default value is:

```json
false
```

### unlit

if true this material won't take any lighting

`unlit`

* is optional

* Type: `boolean`

* cannot be null

* defined in: [MSGAssets-material](material-definitions-materialbaseextension-properties-unlit.md "material.schema.json#/definitions/baseExtension/properties/unlit")

#### unlit Type

`boolean`

#### unlit Default Value

The default value is:

```json
false
```

## Definitions group metallicRoughnessExtension

Reference this group by using

```json
{"$ref":"material.schema.json#/definitions/metallicRoughnessExtension"}
```

| Property                                                      | Type     | Required | Nullable       | Defined by                                                                                                                                                                                            |
| :------------------------------------------------------------ | :------- | :------- | :------------- | :---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| [colorTextureInfo](#colortextureinfo)                         | `object` | Optional | cannot be null | [MSGAssets-material](material-definitions-materialtextureinfo.md "material.schema.json#/definitions/metallicRoughnessExtension/properties/colorTextureInfo")                                          |
| [metallicRoughnessTextureInfo](#metallicroughnesstextureinfo) | `object` | Optional | cannot be null | [MSGAssets-material](material-definitions-materialtextureinfo.md "material.schema.json#/definitions/metallicRoughnessExtension/properties/metallicRoughnessTextureInfo")                              |
| [colorFactor](#colorfactor)                                   | `array`  | Optional | cannot be null | [MSGAssets-material](vec-definitions-vec4.md "material.schema.json#/definitions/metallicRoughnessExtension/properties/colorFactor")                                                                   |
| [metallicFactor](#metallicfactor)                             | `number` | Optional | cannot be null | [MSGAssets-material](material-definitions-materialmetallicroughnessextension-properties-metallicfactor.md "material.schema.json#/definitions/metallicRoughnessExtension/properties/metallicFactor")   |
| [roughnessFactor](#roughnessfactor)                           | `number` | Optional | cannot be null | [MSGAssets-material](material-definitions-materialmetallicroughnessextension-properties-roughnessfactor.md "material.schema.json#/definitions/metallicRoughnessExtension/properties/roughnessFactor") |

### colorTextureInfo

the color/albedo texture info

`colorTextureInfo`

* is optional

* Type: `object` ([MaterialTextureInfo](material-definitions-materialtextureinfo.md))

* cannot be null

* defined in: [MSGAssets-material](material-definitions-materialtextureinfo.md "material.schema.json#/definitions/metallicRoughnessExtension/properties/colorTextureInfo")

#### colorTextureInfo Type

`object` ([MaterialTextureInfo](material-definitions-materialtextureinfo.md))

### metallicRoughnessTextureInfo

the metallic/roughness texture info; R=metallic, G=roughness

`metallicRoughnessTextureInfo`

* is optional

* Type: `object` ([MaterialTextureInfo](material-definitions-materialtextureinfo.md))

* cannot be null

* defined in: [MSGAssets-material](material-definitions-materialtextureinfo.md "material.schema.json#/definitions/metallicRoughnessExtension/properties/metallicRoughnessTextureInfo")

#### metallicRoughnessTextureInfo Type

`object` ([MaterialTextureInfo](material-definitions-materialtextureinfo.md))

### colorFactor

the albedo of this material, if colorTexture is present, used as a multiplier

`colorFactor`

* is optional

* Type: `number[]`

* cannot be null

* defined in: [MSGAssets-material](vec-definitions-vec4.md "material.schema.json#/definitions/metallicRoughnessExtension/properties/colorFactor")

#### colorFactor Type

`number[]`

#### colorFactor Constraints

**maximum number of items**: the maximum number of items for this array is: `4`

**minimum number of items**: the minimum number of items for this array is: `4`

#### colorFactor Default Value

The default value is:

```json
[
  1,
  1,
  1,
  1
]
```

### metallicFactor

the metalness of this material, if metallicRoughnessTexture is present: used as a multiplier

`metallicFactor`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-material](material-definitions-materialmetallicroughnessextension-properties-metallicfactor.md "material.schema.json#/definitions/metallicRoughnessExtension/properties/metallicFactor")

#### metallicFactor Type

`number`

#### metallicFactor Constraints

**minimum**: the value of this number must greater than or equal to: `0`

#### metallicFactor Default Value

The default value is:

```json
1
```

### roughnessFactor

the roughness of this material, if metallicRoughnessTexture is present: used as a multiplier

`roughnessFactor`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-material](material-definitions-materialmetallicroughnessextension-properties-roughnessfactor.md "material.schema.json#/definitions/metallicRoughnessExtension/properties/roughnessFactor")

#### roughnessFactor Type

`number`

#### roughnessFactor Constraints

**minimum**: the value of this number must greater than or equal to: `0`

#### roughnessFactor Default Value

The default value is:

```json
1
```

## Definitions group specularGlossinessExtension

Reference this group by using

```json
{"$ref":"material.schema.json#/definitions/specularGlossinessExtension"}
```

| Property | Type | Required | Nullable | Defined by |
| :------- | :--- | :------- | :------- | :--------- |

## Definitions group material

Reference this group by using

```json
{"$ref":"material.schema.json#/definitions/material"}
```

| Property                                                    | Type     | Required | Nullable       | Defined by                                                                                                                                                            |
| :---------------------------------------------------------- | :------- | :------- | :------------- | :-------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| [uri](#uri)                                                 | `string` | Optional | cannot be null | [MSGAssets-material](external-definitions-uri.md "material.schema.json#/definitions/material/properties/uri")                                                         |
| [name](#name)                                               | `string` | Optional | cannot be null | [MSGAssets-material](material-definitions-material-properties-name.md "material.schema.json#/definitions/material/properties/name")                                   |
| [baseExtension](#baseextension)                             | `object` | Optional | cannot be null | [MSGAssets-material](material-definitions-materialbaseextension.md "material.schema.json#/definitions/material/properties/baseExtension")                             |
| [metallicRoughnessExtension](#metallicroughnessextension)   | `object` | Optional | cannot be null | [MSGAssets-material](material-definitions-materialmetallicroughnessextension.md "material.schema.json#/definitions/material/properties/metallicRoughnessExtension")   |
| [specularGlossinessExtension](#specularglossinessextension) | `object` | Optional | cannot be null | [MSGAssets-material](material-definitions-materialspecularglossinessextension.md "material.schema.json#/definitions/material/properties/specularGlossinessExtension") |

### uri

links to an object loaded from an external resource. To load from the externals list, use the "external" scheme then the external's name followed by the object name as a query

`uri`

* is optional

* Type: `string` ([URI](external-definitions-uri.md))

* cannot be null

* defined in: [MSGAssets-material](external-definitions-uri.md "material.schema.json#/definitions/material/properties/uri")

#### uri Type

`string` ([URI](external-definitions-uri.md))

#### uri Constraints

**URI**: the string must be a URI, according to [RFC 3986](https://tools.ietf.org/html/rfc3986 "check the specification")

#### uri Examples

```json
"external:ExternalName?ObjectName"
```

```json
"file:path/to/file/filename.fileExtension"
```

### name

this material's name, required if uri is not present

`name`

* is optional

* Type: `string`

* cannot be null

* defined in: [MSGAssets-material](material-definitions-material-properties-name.md "material.schema.json#/definitions/material/properties/name")

#### name Type

`string`

### baseExtension

this material's base extension

`baseExtension`

* is optional

* Type: `object` ([MaterialBaseExtension](material-definitions-materialbaseextension.md))

* cannot be null

* defined in: [MSGAssets-material](material-definitions-materialbaseextension.md "material.schema.json#/definitions/material/properties/baseExtension")

#### baseExtension Type

`object` ([MaterialBaseExtension](material-definitions-materialbaseextension.md))

### metallicRoughnessExtension

this allows for the metallic roughness workflow

`metallicRoughnessExtension`

* is optional

* Type: `object` ([MaterialMetallicRoughnessExtension](material-definitions-materialmetallicroughnessextension.md))

* cannot be null

* defined in: [MSGAssets-material](material-definitions-materialmetallicroughnessextension.md "material.schema.json#/definitions/material/properties/metallicRoughnessExtension")

#### metallicRoughnessExtension Type

`object` ([MaterialMetallicRoughnessExtension](material-definitions-materialmetallicroughnessextension.md))

### specularGlossinessExtension

this allows for the specular glossiness workflow, this extension takes priority over metallic roughness if both are present

`specularGlossinessExtension`

* is optional

* Type: `object` ([MaterialSpecularGlossinessExtension](material-definitions-materialspecularglossinessextension.md))

* cannot be null

* defined in: [MSGAssets-material](material-definitions-materialspecularglossinessextension.md "material.schema.json#/definitions/material/properties/specularGlossinessExtension")

#### specularGlossinessExtension Type

`object` ([MaterialSpecularGlossinessExtension](material-definitions-materialspecularglossinessextension.md))

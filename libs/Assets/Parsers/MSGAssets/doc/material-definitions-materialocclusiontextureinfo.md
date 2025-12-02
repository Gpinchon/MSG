# MaterialOcclusionTextureInfo Schema

```txt
material.schema.json#/definitions/occlusionTextureInfo
```

the occlusion texture info

| Abstract            | Extensible | Status         | Identifiable | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                                      |
| :------------------ | :--------- | :------------- | :----------- | :---------------- | :-------------------- | :------------------ | :------------------------------------------------------------------------------ |
| Can be instantiated | No         | Unknown status | No           | Forbidden         | Allowed               | none                | [material.schema.json\*](json/defs/material.schema.json "open original schema") |

## occlusionTextureInfo Type

`object` ([MaterialOcclusionTextureInfo](material-definitions-materialocclusiontextureinfo.md))

# occlusionTextureInfo Properties

| Property                                  | Type      | Required | Nullable       | Defined by                                                                                                                                                                                      |
| :---------------------------------------- | :-------- | :------- | :------------- | :---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| [texture](#texture)                       | `integer` | Optional | cannot be null | [MSGAssets-material](material-definitions-materialocclusiontextureinfo-properties-texture.md "material.schema.json#/definitions/occlusionTextureInfo/properties/texture")                       |
| [sampler](#sampler)                       | `integer` | Optional | cannot be null | [MSGAssets-material](material-definitions-materialocclusiontextureinfo-properties-sampler.md "material.schema.json#/definitions/occlusionTextureInfo/properties/sampler")                       |
| [textureCoordinates](#texturecoordinates) | `integer` | Optional | cannot be null | [MSGAssets-material](material-definitions-materialocclusiontextureinfo-properties-texturecoordinates.md "material.schema.json#/definitions/occlusionTextureInfo/properties/textureCoordinates") |
| [transformScale](#transformscale)         | `array`   | Optional | cannot be null | [MSGAssets-material](vec-definitions-vec2.md "material.schema.json#/definitions/occlusionTextureInfo/properties/transformScale")                                                                |
| [transformOffset](#transformoffset)       | `array`   | Optional | cannot be null | [MSGAssets-material](vec-definitions-vec2.md "material.schema.json#/definitions/occlusionTextureInfo/properties/transformOffset")                                                               |
| [transformRotation](#transformrotation)   | `number`  | Optional | cannot be null | [MSGAssets-material](material-definitions-materialocclusiontextureinfo-properties-transformrotation.md "material.schema.json#/definitions/occlusionTextureInfo/properties/transformRotation")   |
| [strength](#strength)                     | `number`  | Optional | cannot be null | [MSGAssets-material](material-definitions-materialocclusiontextureinfo-properties-strength.md "material.schema.json#/definitions/occlusionTextureInfo/properties/strength")                     |

## texture

an index into the textures list

`texture`

* is optional

* Type: `integer`

* cannot be null

* defined in: [MSGAssets-material](material-definitions-materialocclusiontextureinfo-properties-texture.md "material.schema.json#/definitions/occlusionTextureInfo/properties/texture")

### texture Type

`integer`

### texture Constraints

**minimum**: the value of this number must greater than or equal to: `0`

## sampler

an index into the samplers list

`sampler`

* is optional

* Type: `integer`

* cannot be null

* defined in: [MSGAssets-material](material-definitions-materialocclusiontextureinfo-properties-sampler.md "material.schema.json#/definitions/occlusionTextureInfo/properties/sampler")

### sampler Type

`integer`

### sampler Constraints

**minimum**: the value of this number must greater than or equal to: `0`

## textureCoordinates

the index of texture coordinates to use

`textureCoordinates`

* is optional

* Type: `integer`

* cannot be null

* defined in: [MSGAssets-material](material-definitions-materialocclusiontextureinfo-properties-texturecoordinates.md "material.schema.json#/definitions/occlusionTextureInfo/properties/textureCoordinates")

### textureCoordinates Type

`integer`

### textureCoordinates Constraints

**maximum**: the value of this number must smaller than or equal to: `3`

**minimum**: the value of this number must greater than or equal to: `0`

### textureCoordinates Default Value

The default value is:

```json
0
```

## transformScale

a vector of 2 floats

`transformScale`

* is optional

* Type: `number[]`

* cannot be null

* defined in: [MSGAssets-material](vec-definitions-vec2.md "material.schema.json#/definitions/occlusionTextureInfo/properties/transformScale")

### transformScale Type

`number[]`

### transformScale Constraints

**maximum number of items**: the maximum number of items for this array is: `2`

**minimum number of items**: the minimum number of items for this array is: `2`

### transformScale Default Value

The default value is:

```json
[
  1,
  1
]
```

## transformOffset

a vector of 2 floats

`transformOffset`

* is optional

* Type: `number[]`

* cannot be null

* defined in: [MSGAssets-material](vec-definitions-vec2.md "material.schema.json#/definitions/occlusionTextureInfo/properties/transformOffset")

### transformOffset Type

`number[]`

### transformOffset Constraints

**maximum number of items**: the maximum number of items for this array is: `2`

**minimum number of items**: the minimum number of items for this array is: `2`

### transformOffset Default Value

The default value is:

```json
[
  0,
  0
]
```

## transformRotation

the texture coordinates rotation in radians

`transformRotation`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-material](material-definitions-materialocclusiontextureinfo-properties-transformrotation.md "material.schema.json#/definitions/occlusionTextureInfo/properties/transformRotation")

### transformRotation Type

`number`

### transformRotation Default Value

The default value is:

```json
0
```

## strength

the occlusion strength

`strength`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-material](material-definitions-materialocclusiontextureinfo-properties-strength.md "material.schema.json#/definitions/occlusionTextureInfo/properties/strength")

### strength Type

`number`

### strength Default Value

The default value is:

```json
1
```

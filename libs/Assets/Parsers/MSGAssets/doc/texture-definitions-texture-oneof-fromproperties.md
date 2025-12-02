# FromProperties Schema

```txt
texture.schema.json#/definitions/texture/oneOf/0
```



| Abstract            | Extensible | Status         | Identifiable | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                                    |
| :------------------ | :--------- | :------------- | :----------- | :---------------- | :-------------------- | :------------------ | :---------------------------------------------------------------------------- |
| Can be instantiated | No         | Unknown status | No           | Forbidden         | Allowed               | none                | [texture.schema.json\*](json/defs/texture.schema.json "open original schema") |

## 0 Type

unknown ([FromProperties](texture-definitions-texture-oneof-fromproperties.md))

# 0 Properties

| Property                    | Type          | Required | Nullable       | Defined by                                                                                                                                                                |
| :-------------------------- | :------------ | :------- | :------------- | :------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| [name](#name)               | `string`      | Required | cannot be null | [MSGAssets-texture](texture-definitions-texture-oneof-fromproperties-properties-name.md "texture.schema.json#/definitions/texture/oneOf/0/properties/name")               |
| [type](#type)               | Not specified | Required | cannot be null | [MSGAssets-texture](texture-definitions-texture-oneof-fromproperties-properties-type.md "texture.schema.json#/definitions/texture/oneOf/0/properties/type")               |
| [pixelFormat](#pixelformat) | `string`      | Optional | cannot be null | [MSGAssets-texture](texture-definitions-texture-oneof-fromproperties-properties-pixelformat.md "texture.schema.json#/definitions/texture/oneOf/0/properties/pixelFormat") |
| [size](#size)               | Not specified | Optional | cannot be null | [MSGAssets-texture](texture-definitions-texture-oneof-fromproperties-properties-size.md "texture.schema.json#/definitions/texture/oneOf/0/properties/size")               |
| [offset](#offset)           | Not specified | Optional | cannot be null | [MSGAssets-texture](texture-definitions-texture-oneof-fromproperties-properties-offset.md "texture.schema.json#/definitions/texture/oneOf/0/properties/offset")           |
| [compressed](#compressed)   | `boolean`     | Optional | cannot be null | [MSGAssets-texture](texture-definitions-texture-oneof-fromproperties-properties-compressed.md "texture.schema.json#/definitions/texture/oneOf/0/properties/compressed")   |
| [images](#images)           | `array`       | Required | cannot be null | [MSGAssets-texture](texture-definitions-texture-oneof-fromproperties-properties-images.md "texture.schema.json#/definitions/texture/oneOf/0/properties/images")           |
| [uri](#uri)                 | `string`      | Optional | cannot be null | [MSGAssets-texture](texture-definitions-texture-oneof-fromproperties-properties-uri.md "texture.schema.json#/definitions/texture/oneOf/0/properties/uri")                 |

## name



`name`

* is required

* Type: `string`

* cannot be null

* defined in: [MSGAssets-texture](texture-definitions-texture-oneof-fromproperties-properties-name.md "texture.schema.json#/definitions/texture/oneOf/0/properties/name")

### name Type

`string`

## type



`type`

* is required

* Type: unknown

* cannot be null

* defined in: [MSGAssets-texture](texture-definitions-texture-oneof-fromproperties-properties-type.md "texture.schema.json#/definitions/texture/oneOf/0/properties/type")

### type Type

unknown

## pixelFormat

the format used to interpret the image, defaults to the image's pixel format

`pixelFormat`

* is optional

* Type: `string`

* cannot be null

* defined in: [MSGAssets-texture](texture-definitions-texture-oneof-fromproperties-properties-pixelformat.md "texture.schema.json#/definitions/texture/oneOf/0/properties/pixelFormat")

### pixelFormat Type

`string`

### pixelFormat Constraints

**enum**: the value of this property must be equal to one of the following values:

| Value                     | Explanation |
| :------------------------ | :---------- |
| `"Uint8_NormalizedR"`     |             |
| `"Uint8_NormalizedRG"`    |             |
| `"Uint8_NormalizedRGB"`   |             |
| `"Uint8_NormalizedRGBA"`  |             |
| `"Int8_NormalizedR"`      |             |
| `"Int8_NormalizedRG"`     |             |
| `"Int8_NormalizedRGB"`    |             |
| `"Int8_NormalizedRGBA"`   |             |
| `"Uint8_R"`               |             |
| `"Uint8_RG"`              |             |
| `"Uint8_RGB"`             |             |
| `"Uint8_RGBA"`            |             |
| `"Int8_R"`                |             |
| `"Int8_RG"`               |             |
| `"Int8_RGB"`              |             |
| `"Int8_RGBA"`             |             |
| `"Uint16_NormalizedR"`    |             |
| `"Uint16_NormalizedRG"`   |             |
| `"Uint16_NormalizedRGB"`  |             |
| `"Uint16_NormalizedRGBA"` |             |
| `"Int16_NormalizedR"`     |             |
| `"Int16_NormalizedRG"`    |             |
| `"Int16_NormalizedRGB"`   |             |
| `"Int16_NormalizedRGBA"`  |             |
| `"Uint16_R"`              |             |
| `"Uint16_RG"`             |             |
| `"Uint16_RGB"`            |             |
| `"Uint16_RGBA"`           |             |
| `"Int16_R"`               |             |
| `"Int16_RG"`              |             |
| `"Int16_RGB"`             |             |
| `"Int16_RGBA"`            |             |
| `"Uint32_R"`              |             |
| `"Uint32_RG"`             |             |
| `"Uint32_RGB"`            |             |
| `"Uint32_RGBA"`           |             |
| `"Int32_R"`               |             |
| `"Int32_RG"`              |             |
| `"Int32_RGB"`             |             |
| `"Int32_RGBA"`            |             |
| `"Float16_R"`             |             |
| `"Float16_RG"`            |             |
| `"Float16_RGB"`           |             |
| `"Float16_RGBA"`          |             |
| `"Float32_R"`             |             |
| `"Float32_RG"`            |             |
| `"Float32_RGB"`           |             |
| `"Float32_RGBA"`          |             |
| `"Depth16"`               |             |
| `"Depth24"`               |             |
| `"Depth32"`               |             |
| `"Depth32F"`              |             |
| `"Depth24_Stencil8"`      |             |
| `"Depth32F_Stencil8"`     |             |
| `"Stencil8"`              |             |
| `"DXT5_RGBA"`             |             |

## size

the size of the base image, can be a subset, defaults to the image's size

`size`

* is optional

* Type: unknown

* cannot be null

* defined in: [MSGAssets-texture](texture-definitions-texture-oneof-fromproperties-properties-size.md "texture.schema.json#/definitions/texture/oneOf/0/properties/size")

### size Type

unknown

## offset

the offset inside the base image

`offset`

* is optional

* Type: unknown

* cannot be null

* defined in: [MSGAssets-texture](texture-definitions-texture-oneof-fromproperties-properties-offset.md "texture.schema.json#/definitions/texture/oneOf/0/properties/offset")

### offset Type

unknown

### offset Default Value

The default value is:

```json
[
  0,
  0,
  0
]
```

## compressed



`compressed`

* is optional

* Type: `boolean`

* cannot be null

* defined in: [MSGAssets-texture](texture-definitions-texture-oneof-fromproperties-properties-compressed.md "texture.schema.json#/definitions/texture/oneOf/0/properties/compressed")

### compressed Type

`boolean`

### compressed Default Value

The default value is:

```json
false
```

## images

the list of images comprising this texture's lods

`images`

* is required

* Type: `integer[]`

* cannot be null

* defined in: [MSGAssets-texture](texture-definitions-texture-oneof-fromproperties-properties-images.md "texture.schema.json#/definitions/texture/oneOf/0/properties/images")

### images Type

`integer[]`

### images Constraints

**minimum number of items**: the minimum number of items for this array is: `1`

## uri

links to an object loaded from an external resource. To load from the externals list, use the "external" scheme then the external's name followed by the object name as a query, example: "external:ExternalName?ObjectName"

`uri`

* is optional

* Type: `string` ([URI](texture-definitions-texture-oneof-fromproperties-properties-uri.md))

* cannot be null

* defined in: [MSGAssets-texture](texture-definitions-texture-oneof-fromproperties-properties-uri.md "texture.schema.json#/definitions/texture/oneOf/0/properties/uri")

### uri Type

`string` ([URI](texture-definitions-texture-oneof-fromproperties-properties-uri.md))

### uri Constraints

**URI**: the string must be a URI, according to [RFC 3986](https://tools.ietf.org/html/rfc3986 "check the specification")

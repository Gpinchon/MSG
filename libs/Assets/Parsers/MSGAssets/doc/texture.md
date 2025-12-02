# MSGAssets-texture Schema

```txt
texture.schema.json
```



| Abstract               | Extensible | Status         | Identifiable            | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                                  |
| :--------------------- | :--------- | :------------- | :---------------------- | :---------------- | :-------------------- | :------------------ | :-------------------------------------------------------------------------- |
| Cannot be instantiated | Yes        | Unknown status | Unknown identifiability | Forbidden         | Allowed               | none                | [texture.schema.json](json/defs/texture.schema.json "open original schema") |

## MSGAssets-texture Type

`object` ([MSGAssets-texture](texture.md))

# MSGAssets-texture Definitions

## Definitions group textureType

Reference this group by using

```json
{"$ref":"texture.schema.json#/definitions/textureType"}
```

| Property | Type | Required | Nullable | Defined by |
| :------- | :--- | :------- | :------- | :--------- |

## Definitions group texture

Reference this group by using

```json
{"$ref":"texture.schema.json#/definitions/texture"}
```

| Property                    | Type      | Required | Nullable       | Defined by                                                                                                                                 |
| :-------------------------- | :-------- | :------- | :------------- | :----------------------------------------------------------------------------------------------------------------------------------------- |
| [uri](#uri)                 | `string`  | Optional | cannot be null | [MSGAssets-texture](external-definitions-uri.md "texture.schema.json#/definitions/texture/properties/uri")                                 |
| [name](#name)               | `string`  | Optional | cannot be null | [MSGAssets-texture](texture-definitions-texture-properties-name.md "texture.schema.json#/definitions/texture/properties/name")             |
| [type](#type)               | `string`  | Optional | cannot be null | [MSGAssets-texture](texture-definitions-texture-properties-type.md "texture.schema.json#/definitions/texture/properties/type")             |
| [pixelFormat](#pixelformat) | `string`  | Optional | cannot be null | [MSGAssets-texture](pixelformat-definitions-pixelformat.md "texture.schema.json#/definitions/texture/properties/pixelFormat")              |
| [size](#size)               | `array`   | Optional | cannot be null | [MSGAssets-texture](vec-definitions-vec3.md "texture.schema.json#/definitions/texture/properties/size")                                    |
| [offset](#offset)           | `array`   | Optional | cannot be null | [MSGAssets-texture](vec-definitions-vec3.md "texture.schema.json#/definitions/texture/properties/offset")                                  |
| [compressed](#compressed)   | `boolean` | Optional | cannot be null | [MSGAssets-texture](texture-definitions-texture-properties-compressed.md "texture.schema.json#/definitions/texture/properties/compressed") |
| [images](#images)           | `array`   | Optional | cannot be null | [MSGAssets-texture](texture-definitions-texture-properties-images.md "texture.schema.json#/definitions/texture/properties/images")         |

### uri

links to an object loaded from an external resource. To load from the externals list, use the "external" scheme then the external's name followed by the object name as a query

`uri`

* is optional

* Type: `string` ([URI](external-definitions-uri.md))

* cannot be null

* defined in: [MSGAssets-texture](external-definitions-uri.md "texture.schema.json#/definitions/texture/properties/uri")

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

the name of the texture

> required if uri is not present

`name`

* is optional

* Type: `string`

* cannot be null

* defined in: [MSGAssets-texture](texture-definitions-texture-properties-name.md "texture.schema.json#/definitions/texture/properties/name")

#### name Type

`string`

### type

the type of the texture

> required if uri is not present

`type`

* is optional

* Type: `string`

* cannot be null

* defined in: [MSGAssets-texture](texture-definitions-texture-properties-type.md "texture.schema.json#/definitions/texture/properties/type")

#### type Type

`string`

#### type Constraints

**enum**: the value of this property must be equal to one of the following values:

| Value                         | Explanation |
| :---------------------------- | :---------- |
| `"Texture1D"`                 |             |
| `"Texture1DArray"`            |             |
| `"Texture2D"`                 |             |
| `"Texture2DArray"`            |             |
| `"Texture2DMultisample"`      |             |
| `"Texture2DMultisampleArray"` |             |
| `"Texture3D"`                 |             |
| `"TextureBuffer"`             |             |
| `"TextureCubemap"`            |             |
| `"TextureCubemapArray"`       |             |
| `"TextureRectangle"`          |             |

### pixelFormat

the format used to interpret the image

> defaults to the image's pixel format

`pixelFormat`

* is optional

* Type: `string`

* cannot be null

* defined in: [MSGAssets-texture](pixelformat-definitions-pixelformat.md "texture.schema.json#/definitions/texture/properties/pixelFormat")

#### pixelFormat Type

`string`

#### pixelFormat Constraints

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

### size

the size of the base image, can be a subset

> defaults to the image's size

`size`

* is optional

* Type: `number[]`

* cannot be null

* defined in: [MSGAssets-texture](vec-definitions-vec3.md "texture.schema.json#/definitions/texture/properties/size")

#### size Type

`number[]`

#### size Constraints

**maximum number of items**: the maximum number of items for this array is: `3`

**minimum number of items**: the minimum number of items for this array is: `3`

### offset

the offset inside the base image

`offset`

* is optional

* Type: `number[]`

* cannot be null

* defined in: [MSGAssets-texture](vec-definitions-vec3.md "texture.schema.json#/definitions/texture/properties/offset")

#### offset Type

`number[]`

#### offset Constraints

**maximum number of items**: the maximum number of items for this array is: `3`

**minimum number of items**: the minimum number of items for this array is: `3`

#### offset Default Value

The default value is:

```json
[
  0,
  0,
  0
]
```

### compressed



`compressed`

* is optional

* Type: `boolean`

* cannot be null

* defined in: [MSGAssets-texture](texture-definitions-texture-properties-compressed.md "texture.schema.json#/definitions/texture/properties/compressed")

#### compressed Type

`boolean`

#### compressed Default Value

The default value is:

```json
false
```

### images

the list of images comprising this texture's lods

> required if uri is not present

`images`

* is optional

* Type: `integer[]`

* cannot be null

* defined in: [MSGAssets-texture](texture-definitions-texture-properties-images.md "texture.schema.json#/definitions/texture/properties/images")

#### images Type

`integer[]`

#### images Constraints

**minimum number of items**: the minimum number of items for this array is: `1`

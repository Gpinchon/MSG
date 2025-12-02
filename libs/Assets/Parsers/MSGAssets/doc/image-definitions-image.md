# Image Schema

```txt
image.schema.json#/definitions/image
```

an image if a set of pixels that can be 3 dimensional, it's untyped as the typing comes from Texture

| Abstract            | Extensible | Status         | Identifiable | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                                |
| :------------------ | :--------- | :------------- | :----------- | :---------------- | :-------------------- | :------------------ | :------------------------------------------------------------------------ |
| Can be instantiated | No         | Unknown status | No           | Forbidden         | Allowed               | none                | [image.schema.json\*](json/defs/image.schema.json "open original schema") |

## image Type

`object` ([Image](image-definitions-image.md))

# image Properties

| Property                    | Type     | Required | Nullable       | Defined by                                                                                                                         |
| :-------------------------- | :------- | :------- | :------------- | :--------------------------------------------------------------------------------------------------------------------------------- |
| [uri](#uri)                 | `string` | Optional | cannot be null | [MSGAssets-image](external-definitions-uri.md "image.schema.json#/definitions/image/properties/uri")                               |
| [name](#name)               | `string` | Optional | cannot be null | [MSGAssets-image](image-definitions-image-properties-name.md "image.schema.json#/definitions/image/properties/name")               |
| [pixelFormat](#pixelformat) | `string` | Optional | cannot be null | [MSGAssets-image](image-definitions-image-properties-pixelformat.md "image.schema.json#/definitions/image/properties/pixelFormat") |
| [size](#size)               | `array`  | Optional | cannot be null | [MSGAssets-image](vec-definitions-vec3.md "image.schema.json#/definitions/image/properties/size")                                  |
| [bufferView](#bufferview)   | `number` | Optional | cannot be null | [MSGAssets-image](image-definitions-image-properties-bufferview.md "image.schema.json#/definitions/image/properties/bufferView")   |

## uri

links to an object loaded from an external resource. To load from the externals list, use the "external" scheme then the external's name followed by the object name as a query

`uri`

* is optional

* Type: `string` ([URI](external-definitions-uri.md))

* cannot be null

* defined in: [MSGAssets-image](external-definitions-uri.md "image.schema.json#/definitions/image/properties/uri")

### uri Type

`string` ([URI](external-definitions-uri.md))

### uri Constraints

**URI**: the string must be a URI, according to [RFC 3986](https://tools.ietf.org/html/rfc3986 "check the specification")

### uri Examples

```json
"external:ExternalName?ObjectName"
```

```json
"file:path/to/file/filename.fileExtension"
```

## name

the image's name

> required if uri is not present

`name`

* is optional

* Type: `string`

* cannot be null

* defined in: [MSGAssets-image](image-definitions-image-properties-name.md "image.schema.json#/definitions/image/properties/name")

### name Type

`string`

## pixelFormat

the format used to interpret the image

> required if uri is not present

`pixelFormat`

* is optional

* Type: `string`

* cannot be null

* defined in: [MSGAssets-image](image-definitions-image-properties-pixelformat.md "image.schema.json#/definitions/image/properties/pixelFormat")

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

the size of the image

> required if uri is not present

`size`

* is optional

* Type: `number[]`

* cannot be null

* defined in: [MSGAssets-image](vec-definitions-vec3.md "image.schema.json#/definitions/image/properties/size")

### size Type

`number[]`

### size Constraints

**maximum number of items**: the maximum number of items for this array is: `3`

**minimum number of items**: the minimum number of items for this array is: `3`

## bufferView

an index into the bufferviews list

> required if uri is not present

`bufferView`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-image](image-definitions-image-properties-bufferview.md "image.schema.json#/definitions/image/properties/bufferView")

### bufferView Type

`number`

### bufferView Constraints

**minimum**: the value of this number must greater than or equal to: `0`

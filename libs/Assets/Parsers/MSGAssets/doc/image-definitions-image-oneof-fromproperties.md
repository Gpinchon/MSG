# FromProperties Schema

```txt
image.schema.json#/definitions/image/oneOf/0
```



| Abstract            | Extensible | Status         | Identifiable | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                                |
| :------------------ | :--------- | :------------- | :----------- | :---------------- | :-------------------- | :------------------ | :------------------------------------------------------------------------ |
| Can be instantiated | No         | Unknown status | No           | Forbidden         | Allowed               | none                | [image.schema.json\*](json/defs/image.schema.json "open original schema") |

## 0 Type

unknown ([FromProperties](image-definitions-image-oneof-fromproperties.md))

# 0 Properties

| Property                    | Type          | Required | Nullable       | Defined by                                                                                                                                                      |
| :-------------------------- | :------------ | :------- | :------------- | :-------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| [name](#name)               | Not specified | Required | cannot be null | [MSGAssets-image](image-definitions-image-oneof-fromproperties-properties-name.md "image.schema.json#/definitions/image/oneOf/0/properties/name")               |
| [pixelFormat](#pixelformat) | `string`      | Required | cannot be null | [MSGAssets-image](image-definitions-image-oneof-fromproperties-properties-pixelformat.md "image.schema.json#/definitions/image/oneOf/0/properties/pixelFormat") |
| [size](#size)               | Not specified | Required | cannot be null | [MSGAssets-image](image-definitions-image-oneof-fromproperties-properties-size.md "image.schema.json#/definitions/image/oneOf/0/properties/size")               |
| [bufferView](#bufferview)   | `number`      | Required | cannot be null | [MSGAssets-image](image-definitions-image-oneof-fromproperties-properties-bufferview.md "image.schema.json#/definitions/image/oneOf/0/properties/bufferView")   |
| [uri](#uri)                 | `string`      | Optional | cannot be null | [MSGAssets-image](image-definitions-image-oneof-fromproperties-properties-uri.md "image.schema.json#/definitions/image/oneOf/0/properties/uri")                 |

## name

the image's name, important because it allows for the image to be referenced

`name`

* is required

* Type: unknown

* cannot be null

* defined in: [MSGAssets-image](image-definitions-image-oneof-fromproperties-properties-name.md "image.schema.json#/definitions/image/oneOf/0/properties/name")

### name Type

unknown

## pixelFormat

the format used to interpret the image

`pixelFormat`

* is required

* Type: `string`

* cannot be null

* defined in: [MSGAssets-image](image-definitions-image-oneof-fromproperties-properties-pixelformat.md "image.schema.json#/definitions/image/oneOf/0/properties/pixelFormat")

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

`size`

* is required

* Type: unknown

* cannot be null

* defined in: [MSGAssets-image](image-definitions-image-oneof-fromproperties-properties-size.md "image.schema.json#/definitions/image/oneOf/0/properties/size")

### size Type

unknown

## bufferView

an index into the bufferviews list

`bufferView`

* is required

* Type: `number`

* cannot be null

* defined in: [MSGAssets-image](image-definitions-image-oneof-fromproperties-properties-bufferview.md "image.schema.json#/definitions/image/oneOf/0/properties/bufferView")

### bufferView Type

`number`

### bufferView Constraints

**minimum**: the value of this number must greater than or equal to: `0`

## uri

links to an object loaded from an external resource. To load from the externals list, use the "external" scheme then the external's name followed by the object name as a query, example: "external:ExternalName?ObjectName"

`uri`

* is optional

* Type: `string` ([URI](image-definitions-image-oneof-fromproperties-properties-uri.md))

* cannot be null

* defined in: [MSGAssets-image](image-definitions-image-oneof-fromproperties-properties-uri.md "image.schema.json#/definitions/image/oneOf/0/properties/uri")

### uri Type

`string` ([URI](image-definitions-image-oneof-fromproperties-properties-uri.md))

### uri Constraints

**URI**: the string must be a URI, according to [RFC 3986](https://tools.ietf.org/html/rfc3986 "check the specification")

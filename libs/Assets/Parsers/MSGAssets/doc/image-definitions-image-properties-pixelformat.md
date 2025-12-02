# Untitled string in MSGAssets-image Schema

```txt
image.schema.json#/definitions/image/properties/pixelFormat
```

the format used to interpret the image

> required if uri is not present

| Abstract            | Extensible | Status         | Identifiable            | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                                |
| :------------------ | :--------- | :------------- | :---------------------- | :---------------- | :-------------------- | :------------------ | :------------------------------------------------------------------------ |
| Can be instantiated | No         | Unknown status | Unknown identifiability | Forbidden         | Allowed               | none                | [image.schema.json\*](json/defs/image.schema.json "open original schema") |

## pixelFormat Type

`string`

## pixelFormat Constraints

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

# Untitled string in MSGAssets-texture Schema

```txt
texture.schema.json#/definitions/texture/properties/type
```

the type of the texture

> required if uri is not present

| Abstract            | Extensible | Status         | Identifiable            | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                                    |
| :------------------ | :--------- | :------------- | :---------------------- | :---------------- | :-------------------- | :------------------ | :---------------------------------------------------------------------------- |
| Can be instantiated | No         | Unknown status | Unknown identifiability | Forbidden         | Allowed               | none                | [texture.schema.json\*](json/defs/texture.schema.json "open original schema") |

## type Type

`string`

## type Constraints

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

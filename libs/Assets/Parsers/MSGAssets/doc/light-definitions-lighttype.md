# Untitled string in MSGAssets-light Schema

```txt
light.schema.json#/definitions/lightType
```

the type of light, must be: point, spot, directional or IBL

| Abstract            | Extensible | Status         | Identifiable            | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                                |
| :------------------ | :--------- | :------------- | :---------------------- | :---------------- | :-------------------- | :------------------ | :------------------------------------------------------------------------ |
| Can be instantiated | No         | Unknown status | Unknown identifiability | Forbidden         | Allowed               | none                | [light.schema.json\*](json/defs/light.schema.json "open original schema") |

## lightType Type

`string`

## lightType Constraints

**enum**: the value of this property must be equal to one of the following values:

| Value           | Explanation |
| :-------------- | :---------- |
| `"Point"`       |             |
| `"Spot"`        |             |
| `"Directional"` |             |
| `"IBL"`         |             |

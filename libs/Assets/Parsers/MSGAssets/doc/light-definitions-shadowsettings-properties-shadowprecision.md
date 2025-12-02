# Untitled string in MSGAssets-light Schema

```txt
light.schema.json#/definitions/shadowSettings/properties/shadowPrecision
```

allows the renderer to lower shadow buffer precision to save space and performance

| Abstract            | Extensible | Status         | Identifiable            | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                                |
| :------------------ | :--------- | :------------- | :---------------------- | :---------------- | :-------------------- | :------------------ | :------------------------------------------------------------------------ |
| Can be instantiated | No         | Unknown status | Unknown identifiability | Forbidden         | Allowed               | none                | [light.schema.json\*](json/defs/light.schema.json "open original schema") |

## shadowPrecision Type

`string`

## shadowPrecision Constraints

**enum**: the value of this property must be equal to one of the following values:

| Value      | Explanation |
| :--------- | :---------- |
| `"High"`   |             |
| `"Medium"` |             |
| `"Low"`    |             |

## shadowPrecision Default Value

The default value is:

```json
"High"
```

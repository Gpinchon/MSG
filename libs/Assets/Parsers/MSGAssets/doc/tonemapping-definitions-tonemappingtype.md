# ToneMappingType Schema

```txt
tonemapping.schema.json#/toneMappingSettings/properties/type
```



| Abstract            | Extensible | Status         | Identifiable            | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                                            |
| :------------------ | :--------- | :------------- | :---------------------- | :---------------- | :-------------------- | :------------------ | :------------------------------------------------------------------------------------ |
| Can be instantiated | No         | Unknown status | Unknown identifiability | Forbidden         | Allowed               | none                | [tonemapping.schema.json\*](json/defs/tonemapping.schema.json "open original schema") |

## type Type

`string` ([ToneMappingType](tonemapping-definitions-tonemappingtype.md))

## type Constraints

**enum**: the value of this property must be equal to one of the following values:

| Value        | Explanation |
| :----------- | :---------- |
| `"None"`     |             |
| `"ACES"`     |             |
| `"Reinhard"` |             |
| `"Lottes"`   |             |
| `"Neutral"`  |             |

## type Default Value

The default value is:

```json
"ACES"
```

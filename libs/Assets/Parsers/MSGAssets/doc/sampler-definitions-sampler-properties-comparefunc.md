# Untitled string in MSGAssets-sampler Schema

```txt
sampler.schema.json#/definitions/sampler/properties/compareFunc
```



| Abstract            | Extensible | Status         | Identifiable            | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                                    |
| :------------------ | :--------- | :------------- | :---------------------- | :---------------- | :-------------------- | :------------------ | :---------------------------------------------------------------------------- |
| Can be instantiated | No         | Unknown status | Unknown identifiability | Forbidden         | Allowed               | none                | [sampler.schema.json\*](json/defs/sampler.schema.json "open original schema") |

## compareFunc Type

`string`

## compareFunc Constraints

**enum**: the value of this property must be equal to one of the following values:

| Value            | Explanation |
| :--------------- | :---------- |
| `"LessEqual"`    |             |
| `"GreaterEqual"` |             |
| `"Less"`         |             |
| `"Greater"`      |             |
| `"Equal"`        |             |
| `"NotEqual"`     |             |
| `"Always"`       |             |
| `"Never"`        |             |

## compareFunc Default Value

The default value is:

```json
"Always"
```

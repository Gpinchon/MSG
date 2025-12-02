# Untitled string in MSGAssets-sampler Schema

```txt
sampler.schema.json#/definitions/sampler/properties/magFilter
```



| Abstract            | Extensible | Status         | Identifiable            | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                                    |
| :------------------ | :--------- | :------------- | :---------------------- | :---------------- | :-------------------- | :------------------ | :---------------------------------------------------------------------------- |
| Can be instantiated | No         | Unknown status | Unknown identifiability | Forbidden         | Allowed               | none                | [sampler.schema.json\*](json/defs/sampler.schema.json "open original schema") |

## magFilter Type

`string`

## magFilter Constraints

**enum**: the value of this property must be equal to one of the following values:

| Value                    | Explanation |
| :----------------------- | :---------- |
| `"Nearest"`              |             |
| `"Linear"`               |             |
| `"NearestMipmapLinear"`  |             |
| `"NearestMipmapNearest"` |             |
| `"LinearMipmapLinear"`   |             |
| `"LinearMipmapNearest"`  |             |

## magFilter Default Value

The default value is:

```json
"Linear"
```

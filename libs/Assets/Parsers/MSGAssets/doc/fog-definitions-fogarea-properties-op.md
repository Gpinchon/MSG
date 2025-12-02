# Untitled string in MSGAssets-fog Schema

```txt
fog.schema.json#/definitions/fogArea/properties/op
```

defines how this fog area will behave with the global fog

| Abstract            | Extensible | Status         | Identifiable            | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                            |
| :------------------ | :--------- | :------------- | :---------------------- | :---------------- | :-------------------- | :------------------ | :-------------------------------------------------------------------- |
| Can be instantiated | No         | Unknown status | Unknown identifiability | Forbidden         | Allowed               | none                | [fog.schema.json\*](json/defs/fog.schema.json "open original schema") |

## op Type

`string`

## op Constraints

**enum**: the value of this property must be equal to one of the following values:

| Value       | Explanation |
| :---------- | :---------- |
| `"Add"`     |             |
| `"Replace"` |             |

## op Default Value

The default value is:

```json
"Add"
```

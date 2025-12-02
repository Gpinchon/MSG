# MaterialAlphaMode Schema

```txt
material.schema.json#/definitions/materialAlphaMode
```

is this material opaque, blended or uses an alpha mask ?

| Abstract            | Extensible | Status         | Identifiable            | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                                      |
| :------------------ | :--------- | :------------- | :---------------------- | :---------------- | :-------------------- | :------------------ | :------------------------------------------------------------------------------ |
| Can be instantiated | No         | Unknown status | Unknown identifiability | Forbidden         | Allowed               | none                | [material.schema.json\*](json/defs/material.schema.json "open original schema") |

## materialAlphaMode Type

`string` ([MaterialAlphaMode](material-definitions-materialalphamode.md))

## materialAlphaMode Constraints

**enum**: the value of this property must be equal to one of the following values:

| Value      | Explanation |
| :--------- | :---------- |
| `"Opaque"` |             |
| `"Mask"`   |             |
| `"Blend"`  |             |

## materialAlphaMode Default Value

The default value is:

```json
"Opaque"
```

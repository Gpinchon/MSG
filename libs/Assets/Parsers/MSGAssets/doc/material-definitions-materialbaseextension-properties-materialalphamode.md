# MaterialAlphaMode Schema

```txt
material.schema.json#/definitions/baseExtension/properties/alphaMode
```

is this material opaque, blended or uses an alpha mask ?

| Abstract            | Extensible | Status         | Identifiable            | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                                      |
| :------------------ | :--------- | :------------- | :---------------------- | :---------------- | :-------------------- | :------------------ | :------------------------------------------------------------------------------ |
| Can be instantiated | No         | Unknown status | Unknown identifiability | Forbidden         | Allowed               | none                | [material.schema.json\*](json/defs/material.schema.json "open original schema") |

## alphaMode Type

`string` ([MaterialAlphaMode](material-definitions-materialbaseextension-properties-materialalphamode.md))

## alphaMode Constraints

**enum**: the value of this property must be equal to one of the following values:

| Value      | Explanation |
| :--------- | :---------- |
| `"Opaque"` |             |
| `"Mask"`   |             |
| `"Blend"`  |             |

## alphaMode Default Value

The default value is:

```json
"Opaque"
```

# Untitled number in MSGAssets-material Schema

```txt
material.schema.json#/definitions/baseExtension/properties/alphaCutoff
```

when using Mask alpha mode, every pixels of this material under this alpha cutoff is discarded

| Abstract            | Extensible | Status         | Identifiable            | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                                      |
| :------------------ | :--------- | :------------- | :---------------------- | :---------------- | :-------------------- | :------------------ | :------------------------------------------------------------------------------ |
| Can be instantiated | No         | Unknown status | Unknown identifiability | Forbidden         | Allowed               | none                | [material.schema.json\*](json/defs/material.schema.json "open original schema") |

## alphaCutoff Type

`number`

## alphaCutoff Constraints

**maximum**: the value of this number must smaller than or equal to: `1`

**minimum**: the value of this number must greater than or equal to: `0`

## alphaCutoff Default Value

The default value is:

```json
0.5
```

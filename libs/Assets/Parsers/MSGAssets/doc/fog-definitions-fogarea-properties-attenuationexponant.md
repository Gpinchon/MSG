# Untitled number in MSGAssets-fog Schema

```txt
fog.schema.json#/definitions/fogArea/properties/attenuationExponant
```

defines how much the fog scattering and extinction gets attenuated near the edges of the shapes

| Abstract            | Extensible | Status         | Identifiable            | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                            |
| :------------------ | :--------- | :------------- | :---------------------- | :---------------- | :-------------------- | :------------------ | :-------------------------------------------------------------------- |
| Can be instantiated | No         | Unknown status | Unknown identifiability | Forbidden         | Allowed               | none                | [fog.schema.json\*](json/defs/fog.schema.json "open original schema") |

## attenuationExponant Type

`number`

## attenuationExponant Constraints

**maximum**: the value of this number must smaller than or equal to: `1`

**minimum**: the value of this number must greater than or equal to: `0`

## attenuationExponant Default Value

The default value is:

```json
1
```

# Untitled number in MSGAssets-light Schema

```txt
light.schema.json#/definitions/punctualLight/properties/intensity
```

the intensity of the light in lm/sr for spot and directional, lm/m² for directional, and a unitless muliplier for IBL

| Abstract            | Extensible | Status         | Identifiable            | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                                |
| :------------------ | :--------- | :------------- | :---------------------- | :---------------- | :-------------------- | :------------------ | :------------------------------------------------------------------------ |
| Can be instantiated | No         | Unknown status | Unknown identifiability | Forbidden         | Allowed               | none                | [light.schema.json\*](json/defs/light.schema.json "open original schema") |

## intensity Type

`number`

## intensity Constraints

**minimum**: the value of this number must greater than or equal to: `0`

## intensity Default Value

The default value is:

```json
1
```

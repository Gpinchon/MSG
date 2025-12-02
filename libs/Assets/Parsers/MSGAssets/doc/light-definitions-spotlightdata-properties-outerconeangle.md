# Untitled number in MSGAssets-light Schema

```txt
light.schema.json#/definitions/lightSpotData/properties/outerConeAngle
```

mandatory if type is Spot; the spot light angle in radians, default is pi/4

| Abstract            | Extensible | Status         | Identifiable            | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                                |
| :------------------ | :--------- | :------------- | :---------------------- | :---------------- | :-------------------- | :------------------ | :------------------------------------------------------------------------ |
| Can be instantiated | No         | Unknown status | Unknown identifiability | Forbidden         | Allowed               | none                | [light.schema.json\*](json/defs/light.schema.json "open original schema") |

## outerConeAngle Type

`number`

## outerConeAngle Constraints

**minimum**: the value of this number must greater than or equal to: `0`

## outerConeAngle Default Value

The default value is:

```json
0.785398
```

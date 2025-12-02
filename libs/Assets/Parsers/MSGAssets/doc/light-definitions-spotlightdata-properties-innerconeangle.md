# Untitled number in MSGAssets-light Schema

```txt
light.schema.json#/definitions/lightSpotData/properties/innerConeAngle
```

mandatory if type is Spot; the angle at which the spot light attenuation starts in radians

| Abstract            | Extensible | Status         | Identifiable            | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                                |
| :------------------ | :--------- | :------------- | :---------------------- | :---------------- | :-------------------- | :------------------ | :------------------------------------------------------------------------ |
| Can be instantiated | No         | Unknown status | Unknown identifiability | Forbidden         | Allowed               | none                | [light.schema.json\*](json/defs/light.schema.json "open original schema") |

## innerConeAngle Type

`number`

## innerConeAngle Constraints

**minimum**: the value of this number must greater than or equal to: `0`

## innerConeAngle Default Value

The default value is:

```json
0
```

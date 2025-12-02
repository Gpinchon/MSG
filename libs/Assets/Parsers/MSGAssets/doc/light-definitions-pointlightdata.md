# PointLightData Schema

```txt
light.schema.json#/definitions/lightPointData
```



| Abstract            | Extensible | Status         | Identifiable | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                                |
| :------------------ | :--------- | :------------- | :----------- | :---------------- | :-------------------- | :------------------ | :------------------------------------------------------------------------ |
| Can be instantiated | No         | Unknown status | No           | Forbidden         | Allowed               | none                | [light.schema.json\*](json/defs/light.schema.json "open original schema") |

## lightPointData Type

unknown ([PointLightData](light-definitions-pointlightdata.md))

# lightPointData Properties

| Property        | Type     | Required | Nullable       | Defined by                                                                                                                               |
| :-------------- | :------- | :------- | :------------- | :--------------------------------------------------------------------------------------------------------------------------------------- |
| [range](#range) | `number` | Optional | cannot be null | [MSGAssets-light](light-definitions-pointlightdata-properties-range.md "light.schema.json#/definitions/lightPointData/properties/range") |

## range

the range of this light in meters

`range`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-light](light-definitions-pointlightdata-properties-range.md "light.schema.json#/definitions/lightPointData/properties/range")

### range Type

`number`

### range Constraints

**minimum**: the value of this number must greater than or equal to: `0`

### range Default Value

The default value is:

```json
"Infinity"
```

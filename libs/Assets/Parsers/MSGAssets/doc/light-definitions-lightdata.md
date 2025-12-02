# LightData Schema

```txt
light.schema.json#/definitions/lightData
```



| Abstract            | Extensible | Status         | Identifiable            | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                                |
| :------------------ | :--------- | :------------- | :---------------------- | :---------------- | :-------------------- | :------------------ | :------------------------------------------------------------------------ |
| Can be instantiated | No         | Unknown status | Unknown identifiability | Forbidden         | Allowed               | none                | [light.schema.json\*](json/defs/light.schema.json "open original schema") |

## lightData Type

`object` ([LightData](light-definitions-lightdata.md))

one (and only one) of

* [PointLightData](light-definitions-pointlightdata.md "check type definition")

* [SpotLightData](light-definitions-spotlightdata.md "check type definition")

* [DirectionalLightData](light-definitions-directionallightdata.md "check type definition")

* [ImageBasedLightData](light-definitions-imagebasedlightdata.md "check type definition")

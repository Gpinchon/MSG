# ToneMappingSettings Schema

```txt
tonemapping.schema.json#/toneMappingSettings
```



| Abstract            | Extensible | Status         | Identifiable | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                                            |
| :------------------ | :--------- | :------------- | :----------- | :---------------- | :-------------------- | :------------------ | :------------------------------------------------------------------------------------ |
| Can be instantiated | No         | Unknown status | No           | Forbidden         | Allowed               | none                | [tonemapping.schema.json\*](json/defs/tonemapping.schema.json "open original schema") |

## toneMappingSettings Type

`object` ([ToneMappingSettings](tonemapping-tonemappingsettings.md))

# toneMappingSettings Properties

| Property                          | Type          | Required | Nullable       | Defined by                                                                                                                                   |
| :-------------------------------- | :------------ | :------- | :------------- | :------------------------------------------------------------------------------------------------------------------------------------------- |
| [type](#type)                     | `string`      | Optional | cannot be null | [MSGAssets-tonemapping](tonemapping-definitions-tonemappingtype.md "tonemapping.schema.json#/toneMappingSettings/properties/type")           |
| [lottesSettings](#lottessettings) | Not specified | Optional | cannot be null | [MSGAssets-tonemapping](tonemapping-definitions-lottessettings.md "tonemapping.schema.json#/toneMappingSettings/properties/lottesSettings")  |
| [gamma](#gamma)                   | `number`      | Optional | cannot be null | [MSGAssets-tonemapping](tonemapping-tonemappingsettings-properties-gamma.md "tonemapping.schema.json#/toneMappingSettings/properties/gamma") |

## type



`type`

* is optional

* Type: `string` ([ToneMappingType](tonemapping-definitions-tonemappingtype.md))

* cannot be null

* defined in: [MSGAssets-tonemapping](tonemapping-definitions-tonemappingtype.md "tonemapping.schema.json#/toneMappingSettings/properties/type")

### type Type

`string` ([ToneMappingType](tonemapping-definitions-tonemappingtype.md))

### type Constraints

**enum**: the value of this property must be equal to one of the following values:

| Value        | Explanation |
| :----------- | :---------- |
| `"None"`     |             |
| `"ACES"`     |             |
| `"Reinhard"` |             |
| `"Lottes"`   |             |
| `"Neutral"`  |             |

### type Default Value

The default value is:

```json
"ACES"
```

## lottesSettings

Lottes tone mapping curve settings

`lottesSettings`

* is optional

* Type: unknown ([LottesSettings](tonemapping-definitions-lottessettings.md))

* cannot be null

* defined in: [MSGAssets-tonemapping](tonemapping-definitions-lottessettings.md "tonemapping.schema.json#/toneMappingSettings/properties/lottesSettings")

### lottesSettings Type

unknown ([LottesSettings](tonemapping-definitions-lottessettings.md))

## gamma

Gamma used to convert from linear to Rec709 color space

`gamma`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-tonemapping](tonemapping-tonemappingsettings-properties-gamma.md "tonemapping.schema.json#/toneMappingSettings/properties/gamma")

### gamma Type

`number`

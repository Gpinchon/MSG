# ColorGradingSettings Schema

```txt
tonemapping.schema.json#/colorGradingSettings
```



| Abstract            | Extensible | Status         | Identifiable | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                                            |
| :------------------ | :--------- | :------------- | :----------- | :---------------- | :-------------------- | :------------------ | :------------------------------------------------------------------------------------ |
| Can be instantiated | No         | Unknown status | No           | Forbidden         | Allowed               | none                | [tonemapping.schema.json\*](json/defs/tonemapping.schema.json "open original schema") |

## colorGradingSettings Type

`object` ([ColorGradingSettings](tonemapping-colorgradingsettings.md))

# colorGradingSettings Properties

| Property                      | Type          | Required | Nullable       | Defined by                                                                                                                                                   |
| :---------------------------- | :------------ | :------- | :------------- | :----------------------------------------------------------------------------------------------------------------------------------------------------------- |
| [autoExposure](#autoexposure) | Not specified | Optional | cannot be null | [MSGAssets-tonemapping](tonemapping-colorgradingsettings-properties-autoexposure.md "tonemapping.schema.json#/colorGradingSettings/properties/autoExposure") |
| [exposure](#exposure)         | `number`      | Optional | cannot be null | [MSGAssets-tonemapping](tonemapping-colorgradingsettings-properties-exposure.md "tonemapping.schema.json#/colorGradingSettings/properties/exposure")         |
| [saturation](#saturation)     | `number`      | Optional | cannot be null | [MSGAssets-tonemapping](tonemapping-colorgradingsettings-properties-saturation.md "tonemapping.schema.json#/colorGradingSettings/properties/saturation")     |
| [contrast](#contrast)         | `number`      | Optional | cannot be null | [MSGAssets-tonemapping](tonemapping-colorgradingsettings-properties-contrast.md "tonemapping.schema.json#/colorGradingSettings/properties/contrast")         |
| [hueShift](#hueshift)         | `number`      | Optional | cannot be null | [MSGAssets-tonemapping](tonemapping-colorgradingsettings-properties-hueshift.md "tonemapping.schema.json#/colorGradingSettings/properties/hueShift")         |

## autoExposure



`autoExposure`

* is optional

* Type: unknown

* cannot be null

* defined in: [MSGAssets-tonemapping](tonemapping-colorgradingsettings-properties-autoexposure.md "tonemapping.schema.json#/colorGradingSettings/properties/autoExposure")

### autoExposure Type

unknown

## exposure



`exposure`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-tonemapping](tonemapping-colorgradingsettings-properties-exposure.md "tonemapping.schema.json#/colorGradingSettings/properties/exposure")

### exposure Type

`number`

### exposure Default Value

The default value is:

```json
0
```

## saturation



`saturation`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-tonemapping](tonemapping-colorgradingsettings-properties-saturation.md "tonemapping.schema.json#/colorGradingSettings/properties/saturation")

### saturation Type

`number`

### saturation Default Value

The default value is:

```json
0
```

## contrast



`contrast`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-tonemapping](tonemapping-colorgradingsettings-properties-contrast.md "tonemapping.schema.json#/colorGradingSettings/properties/contrast")

### contrast Type

`number`

### contrast Default Value

The default value is:

```json
0
```

## hueShift



`hueShift`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-tonemapping](tonemapping-colorgradingsettings-properties-hueshift.md "tonemapping.schema.json#/colorGradingSettings/properties/hueShift")

### hueShift Type

`number`

### hueShift Default Value

The default value is:

```json
0
```

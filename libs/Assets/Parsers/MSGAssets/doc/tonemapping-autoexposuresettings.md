# AutoExposureSettings Schema

```txt
tonemapping.schema.json#/autoExposureSettings
```



| Abstract            | Extensible | Status         | Identifiable | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                                            |
| :------------------ | :--------- | :------------- | :----------- | :---------------- | :-------------------- | :------------------ | :------------------------------------------------------------------------------------ |
| Can be instantiated | No         | Unknown status | No           | Forbidden         | Allowed               | none                | [tonemapping.schema.json\*](json/defs/tonemapping.schema.json "open original schema") |

## autoExposureSettings Type

`object` ([AutoExposureSettings](tonemapping-autoexposuresettings.md))

# autoExposureSettings Properties

| Property                            | Type      | Required | Nullable       | Defined by                                                                                                                                                         |
| :---------------------------------- | :-------- | :------- | :------------- | :----------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| [enabled](#enabled)                 | `boolean` | Optional | cannot be null | [MSGAssets-tonemapping](tonemapping-autoexposuresettings-properties-enabled.md "tonemapping.schema.json#/autoExposureSettings/properties/enabled")                 |
| [minLuminance](#minluminance)       | `number`  | Optional | cannot be null | [MSGAssets-tonemapping](tonemapping-autoexposuresettings-properties-minluminance.md "tonemapping.schema.json#/autoExposureSettings/properties/minLuminance")       |
| [maxLuminance](#maxluminance)       | `number`  | Optional | cannot be null | [MSGAssets-tonemapping](tonemapping-autoexposuresettings-properties-maxluminance.md "tonemapping.schema.json#/autoExposureSettings/properties/maxLuminance")       |
| [key](#key)                         | `number`  | Optional | cannot be null | [MSGAssets-tonemapping](tonemapping-autoexposuresettings-properties-key.md "tonemapping.schema.json#/autoExposureSettings/properties/key")                         |
| [adaptationSpeed](#adaptationspeed) | `number`  | Optional | cannot be null | [MSGAssets-tonemapping](tonemapping-autoexposuresettings-properties-adaptationspeed.md "tonemapping.schema.json#/autoExposureSettings/properties/adaptationSpeed") |

## enabled



`enabled`

* is optional

* Type: `boolean`

* cannot be null

* defined in: [MSGAssets-tonemapping](tonemapping-autoexposuresettings-properties-enabled.md "tonemapping.schema.json#/autoExposureSettings/properties/enabled")

### enabled Type

`boolean`

### enabled Default Value

The default value is:

```json
true
```

## minLuminance



`minLuminance`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-tonemapping](tonemapping-autoexposuresettings-properties-minluminance.md "tonemapping.schema.json#/autoExposureSettings/properties/minLuminance")

### minLuminance Type

`number`

### minLuminance Default Value

The default value is:

```json
0.0001
```

## maxLuminance



`maxLuminance`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-tonemapping](tonemapping-autoexposuresettings-properties-maxluminance.md "tonemapping.schema.json#/autoExposureSettings/properties/maxLuminance")

### maxLuminance Type

`number`

### maxLuminance Default Value

The default value is:

```json
1000
```

## key



`key`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-tonemapping](tonemapping-autoexposuresettings-properties-key.md "tonemapping.schema.json#/autoExposureSettings/properties/key")

### key Type

`number`

### key Default Value

The default value is:

```json
0.5
```

## adaptationSpeed



`adaptationSpeed`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-tonemapping](tonemapping-autoexposuresettings-properties-adaptationspeed.md "tonemapping.schema.json#/autoExposureSettings/properties/adaptationSpeed")

### adaptationSpeed Type

`number`

### adaptationSpeed Default Value

The default value is:

```json
1
```

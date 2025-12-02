# LottesSettings Schema

```txt
tonemapping.schema.json#/toneMappingSettings/properties/lottesSettings
```

Lottes tone mapping curve settings

| Abstract            | Extensible | Status         | Identifiable | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                                            |
| :------------------ | :--------- | :------------- | :----------- | :---------------- | :-------------------- | :------------------ | :------------------------------------------------------------------------------------ |
| Can be instantiated | No         | Unknown status | No           | Forbidden         | Allowed               | none                | [tonemapping.schema.json\*](json/defs/tonemapping.schema.json "open original schema") |

## lottesSettings Type

unknown ([LottesSettings](tonemapping-definitions-lottessettings.md))

# lottesSettings Properties

| Property              | Type     | Required | Nullable       | Defined by                                                                                                                                                       |
| :-------------------- | :------- | :------- | :------------- | :--------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| [hdrMax](#hdrmax)     | `number` | Optional | cannot be null | [MSGAssets-tonemapping](tonemapping-definitions-lottessettings-properties-hdrmax.md "tonemapping.schema.json#/definitions/lottesSettings/properties/hdrMax")     |
| [contrast](#contrast) | `number` | Optional | cannot be null | [MSGAssets-tonemapping](tonemapping-definitions-lottessettings-properties-contrast.md "tonemapping.schema.json#/definitions/lottesSettings/properties/contrast") |
| [shoulder](#shoulder) | `number` | Optional | cannot be null | [MSGAssets-tonemapping](tonemapping-definitions-lottessettings-properties-shoulder.md "tonemapping.schema.json#/definitions/lottesSettings/properties/shoulder") |
| [midIn](#midin)       | `number` | Optional | cannot be null | [MSGAssets-tonemapping](tonemapping-definitions-lottessettings-properties-midin.md "tonemapping.schema.json#/definitions/lottesSettings/properties/midIn")       |
| [midOut](#midout)     | `number` | Optional | cannot be null | [MSGAssets-tonemapping](tonemapping-definitions-lottessettings-properties-midout.md "tonemapping.schema.json#/definitions/lottesSettings/properties/midOut")     |

## hdrMax



`hdrMax`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-tonemapping](tonemapping-definitions-lottessettings-properties-hdrmax.md "tonemapping.schema.json#/definitions/lottesSettings/properties/hdrMax")

### hdrMax Type

`number`

### hdrMax Default Value

The default value is:

```json
8
```

## contrast



`contrast`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-tonemapping](tonemapping-definitions-lottessettings-properties-contrast.md "tonemapping.schema.json#/definitions/lottesSettings/properties/contrast")

### contrast Type

`number`

### contrast Default Value

The default value is:

```json
0.977
```

## shoulder



`shoulder`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-tonemapping](tonemapping-definitions-lottessettings-properties-shoulder.md "tonemapping.schema.json#/definitions/lottesSettings/properties/shoulder")

### shoulder Type

`number`

### shoulder Default Value

The default value is:

```json
1.6
```

## midIn



`midIn`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-tonemapping](tonemapping-definitions-lottessettings-properties-midin.md "tonemapping.schema.json#/definitions/lottesSettings/properties/midIn")

### midIn Type

`number`

### midIn Default Value

The default value is:

```json
0.18
```

## midOut



`midOut`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-tonemapping](tonemapping-definitions-lottessettings-properties-midout.md "tonemapping.schema.json#/definitions/lottesSettings/properties/midOut")

### midOut Type

`number`

### midOut Default Value

The default value is:

```json
0.267
```

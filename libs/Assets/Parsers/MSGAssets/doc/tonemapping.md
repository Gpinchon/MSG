# MSGAssets-tonemapping Schema

```txt
tonemapping.schema.json
```



| Abstract               | Extensible | Status         | Identifiable            | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                                          |
| :--------------------- | :--------- | :------------- | :---------------------- | :---------------- | :-------------------- | :------------------ | :---------------------------------------------------------------------------------- |
| Cannot be instantiated | Yes        | Unknown status | Unknown identifiability | Forbidden         | Allowed               | none                | [tonemapping.schema.json](json/defs/tonemapping.schema.json "open original schema") |

## MSGAssets-tonemapping Type

`object` ([MSGAssets-tonemapping](tonemapping.md))

# MSGAssets-tonemapping Definitions

## Definitions group toneMappingType

Reference this group by using

```json
{"$ref":"tonemapping.schema.json#/definitions/toneMappingType"}
```

| Property | Type | Required | Nullable | Defined by |
| :------- | :--- | :------- | :------- | :--------- |

## Definitions group lottesSettings

Reference this group by using

```json
{"$ref":"tonemapping.schema.json#/definitions/lottesSettings"}
```

| Property              | Type     | Required | Nullable       | Defined by                                                                                                                                                       |
| :-------------------- | :------- | :------- | :------------- | :--------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| [hdrMax](#hdrmax)     | `number` | Optional | cannot be null | [MSGAssets-tonemapping](tonemapping-definitions-lottessettings-properties-hdrmax.md "tonemapping.schema.json#/definitions/lottesSettings/properties/hdrMax")     |
| [contrast](#contrast) | `number` | Optional | cannot be null | [MSGAssets-tonemapping](tonemapping-definitions-lottessettings-properties-contrast.md "tonemapping.schema.json#/definitions/lottesSettings/properties/contrast") |
| [shoulder](#shoulder) | `number` | Optional | cannot be null | [MSGAssets-tonemapping](tonemapping-definitions-lottessettings-properties-shoulder.md "tonemapping.schema.json#/definitions/lottesSettings/properties/shoulder") |
| [midIn](#midin)       | `number` | Optional | cannot be null | [MSGAssets-tonemapping](tonemapping-definitions-lottessettings-properties-midin.md "tonemapping.schema.json#/definitions/lottesSettings/properties/midIn")       |
| [midOut](#midout)     | `number` | Optional | cannot be null | [MSGAssets-tonemapping](tonemapping-definitions-lottessettings-properties-midout.md "tonemapping.schema.json#/definitions/lottesSettings/properties/midOut")     |

### hdrMax



`hdrMax`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-tonemapping](tonemapping-definitions-lottessettings-properties-hdrmax.md "tonemapping.schema.json#/definitions/lottesSettings/properties/hdrMax")

#### hdrMax Type

`number`

#### hdrMax Default Value

The default value is:

```json
8
```

### contrast



`contrast`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-tonemapping](tonemapping-definitions-lottessettings-properties-contrast.md "tonemapping.schema.json#/definitions/lottesSettings/properties/contrast")

#### contrast Type

`number`

#### contrast Default Value

The default value is:

```json
0.977
```

### shoulder



`shoulder`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-tonemapping](tonemapping-definitions-lottessettings-properties-shoulder.md "tonemapping.schema.json#/definitions/lottesSettings/properties/shoulder")

#### shoulder Type

`number`

#### shoulder Default Value

The default value is:

```json
1.6
```

### midIn



`midIn`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-tonemapping](tonemapping-definitions-lottessettings-properties-midin.md "tonemapping.schema.json#/definitions/lottesSettings/properties/midIn")

#### midIn Type

`number`

#### midIn Default Value

The default value is:

```json
0.18
```

### midOut



`midOut`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-tonemapping](tonemapping-definitions-lottessettings-properties-midout.md "tonemapping.schema.json#/definitions/lottesSettings/properties/midOut")

#### midOut Type

`number`

#### midOut Default Value

The default value is:

```json
0.267
```

## Definitions group bloomSettings

Reference this group by using

```json
{"$ref":"tonemapping.schema.json#/definitions/bloomSettings"}
```

| Property                | Type     | Required | Nullable       | Defined by                                                                                                                                                       |
| :---------------------- | :------- | :------- | :------------- | :--------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| [intensity](#intensity) | `number` | Optional | cannot be null | [MSGAssets-tonemapping](tonemapping-definitions-bloomsettings-properties-intensity.md "tonemapping.schema.json#/definitions/bloomSettings/properties/intensity") |
| [threshold](#threshold) | `number` | Optional | cannot be null | [MSGAssets-tonemapping](tonemapping-definitions-bloomsettings-properties-threshold.md "tonemapping.schema.json#/definitions/bloomSettings/properties/threshold") |
| [smoothing](#smoothing) | `number` | Optional | cannot be null | [MSGAssets-tonemapping](tonemapping-definitions-bloomsettings-properties-smoothing.md "tonemapping.schema.json#/definitions/bloomSettings/properties/smoothing") |
| [size](#size)           | `number` | Optional | cannot be null | [MSGAssets-tonemapping](tonemapping-definitions-bloomsettings-properties-size.md "tonemapping.schema.json#/definitions/bloomSettings/properties/size")           |
| [tint](#tint)           | `array`  | Optional | cannot be null | [MSGAssets-tonemapping](vec-definitions-vec3.md "tonemapping.schema.json#/definitions/bloomSettings/properties/tint")                                            |

### intensity



`intensity`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-tonemapping](tonemapping-definitions-bloomsettings-properties-intensity.md "tonemapping.schema.json#/definitions/bloomSettings/properties/intensity")

#### intensity Type

`number`

#### intensity Default Value

The default value is:

```json
1
```

### threshold



`threshold`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-tonemapping](tonemapping-definitions-bloomsettings-properties-threshold.md "tonemapping.schema.json#/definitions/bloomSettings/properties/threshold")

#### threshold Type

`number`

#### threshold Default Value

The default value is:

```json
1
```

### smoothing



`smoothing`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-tonemapping](tonemapping-definitions-bloomsettings-properties-smoothing.md "tonemapping.schema.json#/definitions/bloomSettings/properties/smoothing")

#### smoothing Type

`number`

#### smoothing Default Value

The default value is:

```json
10
```

### size



`size`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-tonemapping](tonemapping-definitions-bloomsettings-properties-size.md "tonemapping.schema.json#/definitions/bloomSettings/properties/size")

#### size Type

`number`

#### size Default Value

The default value is:

```json
25
```

### tint



`tint`

* is optional

* Type: `number[]`

* cannot be null

* defined in: [MSGAssets-tonemapping](vec-definitions-vec3.md "tonemapping.schema.json#/definitions/bloomSettings/properties/tint")

#### tint Type

`number[]`

#### tint Constraints

**maximum number of items**: the maximum number of items for this array is: `3`

**minimum number of items**: the minimum number of items for this array is: `3`

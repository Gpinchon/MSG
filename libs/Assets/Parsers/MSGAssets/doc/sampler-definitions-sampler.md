# Untitled object in MSGAssets-sampler Schema

```txt
sampler.schema.json#/definitions/sampler
```



| Abstract            | Extensible | Status         | Identifiable | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                                    |
| :------------------ | :--------- | :------------- | :----------- | :---------------- | :-------------------- | :------------------ | :---------------------------------------------------------------------------- |
| Can be instantiated | No         | Unknown status | No           | Forbidden         | Allowed               | none                | [sampler.schema.json\*](json/defs/sampler.schema.json "open original schema") |

## sampler Type

`object` ([Details](sampler-definitions-sampler.md))

# sampler Properties

| Property                        | Type     | Required | Nullable       | Defined by                                                                                                                                       |
| :------------------------------ | :------- | :------- | :------------- | :----------------------------------------------------------------------------------------------------------------------------------------------- |
| [magFilter](#magfilter)         | `string` | Optional | cannot be null | [MSGAssets-sampler](sampler-definitions-samplerfilter.md "sampler.schema.json#/definitions/sampler/properties/magFilter")                        |
| [minFilter](#minfilter)         | `string` | Optional | cannot be null | [MSGAssets-sampler](sampler-definitions-samplerfilter.md "sampler.schema.json#/definitions/sampler/properties/minFilter")                        |
| [wrapModes](#wrapmodes)         | `array`  | Optional | cannot be null | [MSGAssets-sampler](sampler-definitions-sampler-properties-wrapmodes.md "sampler.schema.json#/definitions/sampler/properties/wrapModes")         |
| [compareMode](#comparemode)     | `string` | Optional | cannot be null | [MSGAssets-sampler](sampler-definitions-samplercomparemode.md "sampler.schema.json#/definitions/sampler/properties/compareMode")                 |
| [compareFunc](#comparefunc)     | `string` | Optional | cannot be null | [MSGAssets-sampler](sampler-definitions-samplercomparefunc.md "sampler.schema.json#/definitions/sampler/properties/compareFunc")                 |
| [minLOD](#minlod)               | `number` | Optional | cannot be null | [MSGAssets-sampler](sampler-definitions-sampler-properties-minlod.md "sampler.schema.json#/definitions/sampler/properties/minLOD")               |
| [maxLOD](#maxlod)               | `number` | Optional | cannot be null | [MSGAssets-sampler](sampler-definitions-sampler-properties-maxlod.md "sampler.schema.json#/definitions/sampler/properties/maxLOD")               |
| [lodBias](#lodbias)             | `number` | Optional | cannot be null | [MSGAssets-sampler](sampler-definitions-sampler-properties-lodbias.md "sampler.schema.json#/definitions/sampler/properties/lodBias")             |
| [maxAnisotropy](#maxanisotropy) | `number` | Optional | cannot be null | [MSGAssets-sampler](sampler-definitions-sampler-properties-maxanisotropy.md "sampler.schema.json#/definitions/sampler/properties/maxAnisotropy") |
| [borderColor](#bordercolor)     | `array`  | Optional | cannot be null | [MSGAssets-sampler](vec-definitions-vec4.md "sampler.schema.json#/definitions/sampler/properties/borderColor")                                   |

## magFilter



`magFilter`

* is optional

* Type: `string`

* cannot be null

* defined in: [MSGAssets-sampler](sampler-definitions-samplerfilter.md "sampler.schema.json#/definitions/sampler/properties/magFilter")

### magFilter Type

`string`

### magFilter Constraints

**enum**: the value of this property must be equal to one of the following values:

| Value                    | Explanation |
| :----------------------- | :---------- |
| `"Nearest"`              |             |
| `"Linear"`               |             |
| `"NearestMipmapLinear"`  |             |
| `"NearestMipmapNearest"` |             |
| `"LinearMipmapLinear"`   |             |
| `"LinearMipmapNearest"`  |             |

### magFilter Default Value

The default value is:

```json
"Linear"
```

## minFilter



`minFilter`

* is optional

* Type: `string`

* cannot be null

* defined in: [MSGAssets-sampler](sampler-definitions-samplerfilter.md "sampler.schema.json#/definitions/sampler/properties/minFilter")

### minFilter Type

`string`

### minFilter Constraints

**enum**: the value of this property must be equal to one of the following values:

| Value                    | Explanation |
| :----------------------- | :---------- |
| `"Nearest"`              |             |
| `"Linear"`               |             |
| `"NearestMipmapLinear"`  |             |
| `"NearestMipmapNearest"` |             |
| `"LinearMipmapLinear"`   |             |
| `"LinearMipmapNearest"`  |             |

### minFilter Default Value

The default value is:

```json
"NearestMipmapLinear"
```

## wrapModes



`wrapModes`

* is optional

* Type: `string[]`

* cannot be null

* defined in: [MSGAssets-sampler](sampler-definitions-sampler-properties-wrapmodes.md "sampler.schema.json#/definitions/sampler/properties/wrapModes")

### wrapModes Type

`string[]`

### wrapModes Constraints

**maximum number of items**: the maximum number of items for this array is: `3`

**minimum number of items**: the minimum number of items for this array is: `3`

### wrapModes Default Value

The default value is:

```json
[
  "Repeat",
  "Repeat",
  "Repeat"
]
```

## compareMode



`compareMode`

* is optional

* Type: `string`

* cannot be null

* defined in: [MSGAssets-sampler](sampler-definitions-samplercomparemode.md "sampler.schema.json#/definitions/sampler/properties/compareMode")

### compareMode Type

`string`

### compareMode Constraints

**enum**: the value of this property must be equal to one of the following values:

| Value                   | Explanation |
| :---------------------- | :---------- |
| `"None"`                |             |
| `"CompareRefToTexture"` |             |

### compareMode Default Value

The default value is:

```json
"None"
```

## compareFunc



`compareFunc`

* is optional

* Type: `string`

* cannot be null

* defined in: [MSGAssets-sampler](sampler-definitions-samplercomparefunc.md "sampler.schema.json#/definitions/sampler/properties/compareFunc")

### compareFunc Type

`string`

### compareFunc Constraints

**enum**: the value of this property must be equal to one of the following values:

| Value            | Explanation |
| :--------------- | :---------- |
| `"LessEqual"`    |             |
| `"GreaterEqual"` |             |
| `"Less"`         |             |
| `"Greater"`      |             |
| `"Equal"`        |             |
| `"NotEqual"`     |             |
| `"Always"`       |             |
| `"Never"`        |             |

### compareFunc Default Value

The default value is:

```json
"Always"
```

## minLOD



`minLOD`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-sampler](sampler-definitions-sampler-properties-minlod.md "sampler.schema.json#/definitions/sampler/properties/minLOD")

### minLOD Type

`number`

### minLOD Default Value

The default value is:

```json
-1000
```

## maxLOD



`maxLOD`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-sampler](sampler-definitions-sampler-properties-maxlod.md "sampler.schema.json#/definitions/sampler/properties/maxLOD")

### maxLOD Type

`number`

### maxLOD Default Value

The default value is:

```json
1000
```

## lodBias



`lodBias`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-sampler](sampler-definitions-sampler-properties-lodbias.md "sampler.schema.json#/definitions/sampler/properties/lodBias")

### lodBias Type

`number`

### lodBias Default Value

The default value is:

```json
0
```

## maxAnisotropy



`maxAnisotropy`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-sampler](sampler-definitions-sampler-properties-maxanisotropy.md "sampler.schema.json#/definitions/sampler/properties/maxAnisotropy")

### maxAnisotropy Type

`number`

### maxAnisotropy Default Value

The default value is:

```json
16
```

## borderColor

a vector of 4 floats

`borderColor`

* is optional

* Type: `number[]`

* cannot be null

* defined in: [MSGAssets-sampler](vec-definitions-vec4.md "sampler.schema.json#/definitions/sampler/properties/borderColor")

### borderColor Type

`number[]`

### borderColor Constraints

**maximum number of items**: the maximum number of items for this array is: `4`

**minimum number of items**: the minimum number of items for this array is: `4`

### borderColor Default Value

The default value is:

```json
[
  0,
  0,
  0,
  1
]
```

# ShadowSettings Schema

```txt
light.schema.json#/definitions/shadowSettings
```



| Abstract            | Extensible | Status         | Identifiable | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                                |
| :------------------ | :--------- | :------------- | :----------- | :---------------- | :-------------------- | :------------------ | :------------------------------------------------------------------------ |
| Can be instantiated | No         | Unknown status | No           | Forbidden         | Allowed               | none                | [light.schema.json\*](json/defs/light.schema.json "open original schema") |

## shadowSettings Type

`object` ([ShadowSettings](light-definitions-shadowsettings.md))

# shadowSettings Properties

| Property                            | Type      | Required | Nullable       | Defined by                                                                                                                                                   |
| :---------------------------------- | :-------- | :------- | :------------- | :----------------------------------------------------------------------------------------------------------------------------------------------------------- |
| [castShadow](#castshadow)           | `boolean` | Optional | cannot be null | [MSGAssets-light](light-definitions-shadowsettings-properties-castshadow.md "light.schema.json#/definitions/shadowSettings/properties/castShadow")           |
| [shadowPrecision](#shadowprecision) | `string`  | Optional | cannot be null | [MSGAssets-light](light-definitions-shadowsettings-properties-shadowprecision.md "light.schema.json#/definitions/shadowSettings/properties/shadowPrecision") |
| [bias](#bias)                       | `number`  | Optional | cannot be null | [MSGAssets-light](light-definitions-shadowsettings-properties-bias.md "light.schema.json#/definitions/shadowSettings/properties/bias")                       |
| [normalBias](#normalbias)           | `number`  | Optional | cannot be null | [MSGAssets-light](light-definitions-shadowsettings-properties-normalbias.md "light.schema.json#/definitions/shadowSettings/properties/normalBias")           |
| [blurRadius](#blurradius)           | `number`  | Optional | cannot be null | [MSGAssets-light](light-definitions-shadowsettings-properties-blurradius.md "light.schema.json#/definitions/shadowSettings/properties/blurRadius")           |
| [resolution](#resolution)           | `number`  | Optional | cannot be null | [MSGAssets-light](light-definitions-shadowsettings-properties-resolution.md "light.schema.json#/definitions/shadowSettings/properties/resolution")           |
| [cascadeCount](#cascadecount)       | `number`  | Optional | cannot be null | [MSGAssets-light](light-definitions-shadowsettings-properties-cascadecount.md "light.schema.json#/definitions/shadowSettings/properties/cascadeCount")       |

## castShadow

if true this light will cast a dynamic shadow

`castShadow`

* is optional

* Type: `boolean`

* cannot be null

* defined in: [MSGAssets-light](light-definitions-shadowsettings-properties-castshadow.md "light.schema.json#/definitions/shadowSettings/properties/castShadow")

### castShadow Type

`boolean`

### castShadow Default Value

The default value is:

```json
false
```

## shadowPrecision

allows the renderer to lower shadow buffer precision to save space and performance

`shadowPrecision`

* is optional

* Type: `string`

* cannot be null

* defined in: [MSGAssets-light](light-definitions-shadowsettings-properties-shadowprecision.md "light.schema.json#/definitions/shadowSettings/properties/shadowPrecision")

### shadowPrecision Type

`string`

### shadowPrecision Constraints

**enum**: the value of this property must be equal to one of the following values:

| Value      | Explanation |
| :--------- | :---------- |
| `"High"`   |             |
| `"Medium"` |             |
| `"Low"`    |             |

### shadowPrecision Default Value

The default value is:

```json
"High"
```

## bias

the base amount of bias to apply to shadow maps

`bias`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-light](light-definitions-shadowsettings-properties-bias.md "light.schema.json#/definitions/shadowSettings/properties/bias")

### bias Type

`number`

### bias Default Value

The default value is:

```json
0.0025
```

## normalBias

the amount of extra bias to be applied to 90 degrees normals in texels

`normalBias`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-light](light-definitions-shadowsettings-properties-normalbias.md "light.schema.json#/definitions/shadowSettings/properties/normalBias")

### normalBias Type

`number`

### normalBias Constraints

**minimum**: the value of this number must greater than or equal to: `0`

### normalBias Default Value

The default value is:

```json
0.1
```

## blurRadius

blur radius in texels

`blurRadius`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-light](light-definitions-shadowsettings-properties-blurradius.md "light.schema.json#/definitions/shadowSettings/properties/blurRadius")

### blurRadius Type

`number`

### blurRadius Constraints

**minimum**: the value of this number must greater than or equal to: `0`

### blurRadius Default Value

The default value is:

```json
1
```

## resolution

the shadow map resolution

`resolution`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-light](light-definitions-shadowsettings-properties-resolution.md "light.schema.json#/definitions/shadowSettings/properties/resolution")

### resolution Type

`number`

### resolution Constraints

**minimum**: the value of this number must greater than or equal to: `128`

### resolution Default Value

The default value is:

```json
512
```

## cascadeCount

1 means no cascades

`cascadeCount`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-light](light-definitions-shadowsettings-properties-cascadecount.md "light.schema.json#/definitions/shadowSettings/properties/cascadeCount")

### cascadeCount Type

`number`

### cascadeCount Default Value

The default value is:

```json
1
```

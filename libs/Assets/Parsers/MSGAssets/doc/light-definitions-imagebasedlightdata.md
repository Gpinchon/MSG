# ImageBasedLightData Schema

```txt
light.schema.json#/definitions/IBLData
```



| Abstract            | Extensible | Status         | Identifiable | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                                |
| :------------------ | :--------- | :------------- | :----------- | :---------------- | :-------------------- | :------------------ | :------------------------------------------------------------------------ |
| Can be instantiated | No         | Unknown status | No           | Forbidden         | Allowed               | none                | [light.schema.json\*](json/defs/light.schema.json "open original schema") |

## IBLData Type

unknown ([ImageBasedLightData](light-definitions-imagebasedlightdata.md))

# IBLData Properties

| Property                                          | Type      | Required | Nullable       | Defined by                                                                                                                                                               |
| :------------------------------------------------ | :-------- | :------- | :------------- | :----------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| [halfSize](#halfsize)                             | `array`   | Optional | cannot be null | [MSGAssets-light](vec-definitions-vec3.md "light.schema.json#/definitions/IBLData/properties/halfSize")                                                                  |
| [boxProjection](#boxprojection)                   | `boolean` | Optional | cannot be null | [MSGAssets-light](light-definitions-imagebasedlightdata-properties-boxprojection.md "light.schema.json#/definitions/IBLData/properties/boxProjection")                   |
| [specular](#specular)                             | `number`  | Optional | cannot be null | [MSGAssets-light](light-definitions-imagebasedlightdata-properties-specular.md "light.schema.json#/definitions/IBLData/properties/specular")                             |
| [specularSampler](#specularsampler)               | `number`  | Optional | cannot be null | [MSGAssets-light](light-definitions-imagebasedlightdata-properties-specularsampler.md "light.schema.json#/definitions/IBLData/properties/specularSampler")               |
| [irradianceCoefficients](#irradiancecoefficients) | `array`   | Optional | cannot be null | [MSGAssets-light](light-definitions-imagebasedlightdata-properties-irradiancecoefficients.md "light.schema.json#/definitions/IBLData/properties/irradianceCoefficients") |

## halfSize

mandatory if type is Directional or IBL; the size of the light's AABB

`halfSize`

* is optional

* Type: `number[]`

* cannot be null

* defined in: [MSGAssets-light](vec-definitions-vec3.md "light.schema.json#/definitions/IBLData/properties/halfSize")

### halfSize Type

`number[]`

### halfSize Constraints

**maximum number of items**: the maximum number of items for this array is: `3`

**minimum number of items**: the minimum number of items for this array is: `3`

## boxProjection

if set to true, the light sampling will be offset using box projection

`boxProjection`

* is optional

* Type: `boolean`

* cannot be null

* defined in: [MSGAssets-light](light-definitions-imagebasedlightdata-properties-boxprojection.md "light.schema.json#/definitions/IBLData/properties/boxProjection")

### boxProjection Type

`boolean`

### boxProjection Default Value

The default value is:

```json
true
```

## specular

an index into the textures array to use for specular/reflection, if the texture is 2D, a cubemap will be generated

`specular`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-light](light-definitions-imagebasedlightdata-properties-specular.md "light.schema.json#/definitions/IBLData/properties/specular")

### specular Type

`number`

### specular Constraints

**minimum**: the value of this number must greater than or equal to: `0`

## specularSampler

an index into the samplers array to use for specular/reflection

`specularSampler`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-light](light-definitions-imagebasedlightdata-properties-specularsampler.md "light.schema.json#/definitions/IBLData/properties/specularSampler")

### specularSampler Type

`number`

### specularSampler Constraints

**minimum**: the value of this number must greater than or equal to: `0`

## irradianceCoefficients

the irradiance coefficients generated using spherical harmonics to get the IBL diffuse color, if absent, generated on the fly using specular texture

`irradianceCoefficients`

* is optional

* Type: `number[][]`

* cannot be null

* defined in: [MSGAssets-light](light-definitions-imagebasedlightdata-properties-irradiancecoefficients.md "light.schema.json#/definitions/IBLData/properties/irradianceCoefficients")

### irradianceCoefficients Type

`number[][]`

### irradianceCoefficients Constraints

**maximum number of items**: the maximum number of items for this array is: `16`

**minimum number of items**: the minimum number of items for this array is: `16`

# Camera Schema

```txt
camera.schema.json#/definitions/camera
```

a camera to see the scene through

| Abstract            | Extensible | Status         | Identifiable | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                                  |
| :------------------ | :--------- | :------------- | :----------- | :---------------- | :-------------------- | :------------------ | :-------------------------------------------------------------------------- |
| Can be instantiated | No         | Unknown status | No           | Forbidden         | Allowed               | none                | [camera.schema.json\*](json/defs/camera.schema.json "open original schema") |

## camera Type

`object` ([Camera](camera-definitions-camera.md))

# camera Properties

| Property                      | Type          | Required | Nullable       | Defined by                                                                                                                                |
| :---------------------------- | :------------ | :------- | :------------- | :---------------------------------------------------------------------------------------------------------------------------------------- |
| [copyFrom](#copyfrom)         | `string`      | Optional | cannot be null | [MSGAssets-camera](camera-definitions-camera-properties-fromentity.md "camera.schema.json#/definitions/camera/properties/copyFrom")       |
| [projection](#projection)     | `object`      | Optional | cannot be null | [MSGAssets-camera](camera-definitions-cameraprojection.md "camera.schema.json#/definitions/camera/properties/projection")                 |
| [colorGrading](#colorgrading) | Not specified | Optional | cannot be null | [MSGAssets-camera](camera-definitions-camera-properties-colorgrading.md "camera.schema.json#/definitions/camera/properties/colorGrading") |
| [toneMapping](#tonemapping)   | Not specified | Optional | cannot be null | [MSGAssets-camera](camera-definitions-camera-properties-tonemapping.md "camera.schema.json#/definitions/camera/properties/toneMapping")   |
| [bloom](#bloom)               | `object`      | Optional | cannot be null | [MSGAssets-camera](tonemapping-definitions-bloomsettings.md "camera.schema.json#/definitions/camera/properties/bloom")                    |

## copyFrom

the name of the entity to copy the component from

`copyFrom`

* is optional

* Type: `string` ([FromEntity](camera-definitions-camera-properties-fromentity.md))

* cannot be null

* defined in: [MSGAssets-camera](camera-definitions-camera-properties-fromentity.md "camera.schema.json#/definitions/camera/properties/copyFrom")

### copyFrom Type

`string` ([FromEntity](camera-definitions-camera-properties-fromentity.md))

## projection



`projection`

* is optional

* Type: `object` ([CameraProjection](camera-definitions-cameraprojection.md))

* cannot be null

* defined in: [MSGAssets-camera](camera-definitions-cameraprojection.md "camera.schema.json#/definitions/camera/properties/projection")

### projection Type

`object` ([CameraProjection](camera-definitions-cameraprojection.md))

## colorGrading



`colorGrading`

* is optional

* Type: unknown

* cannot be null

* defined in: [MSGAssets-camera](camera-definitions-camera-properties-colorgrading.md "camera.schema.json#/definitions/camera/properties/colorGrading")

### colorGrading Type

unknown

## toneMapping



`toneMapping`

* is optional

* Type: unknown

* cannot be null

* defined in: [MSGAssets-camera](camera-definitions-camera-properties-tonemapping.md "camera.schema.json#/definitions/camera/properties/toneMapping")

### toneMapping Type

unknown

## bloom



`bloom`

* is optional

* Type: `object` ([BloomSettings](tonemapping-definitions-bloomsettings.md))

* cannot be null

* defined in: [MSGAssets-camera](tonemapping-definitions-bloomsettings.md "camera.schema.json#/definitions/camera/properties/bloom")

### bloom Type

`object` ([BloomSettings](tonemapping-definitions-bloomsettings.md))

### bloom Default Value

The default value is:

```json
[
  1,
  1,
  1
]
```

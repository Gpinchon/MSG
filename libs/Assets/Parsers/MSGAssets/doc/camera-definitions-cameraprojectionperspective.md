# CameraProjectionPerspective Schema

```txt
camera.schema.json#/definitions/cameraProjectionPerspective
```



| Abstract            | Extensible | Status         | Identifiable | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                                  |
| :------------------ | :--------- | :------------- | :----------- | :---------------- | :-------------------- | :------------------ | :-------------------------------------------------------------------------- |
| Can be instantiated | No         | Unknown status | No           | Forbidden         | Allowed               | none                | [camera.schema.json\*](json/defs/camera.schema.json "open original schema") |

## cameraProjectionPerspective Type

`object` ([CameraProjectionPerspective](camera-definitions-cameraprojectionperspective.md))

# cameraProjectionPerspective Properties

| Property                    | Type     | Required | Nullable       | Defined by                                                                                                                                                                        |
| :-------------------------- | :------- | :------- | :------------- | :-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| [fov](#fov)                 | `number` | Optional | cannot be null | [MSGAssets-camera](camera-definitions-cameraprojectionperspective-properties-fov.md "camera.schema.json#/definitions/cameraProjectionPerspective/properties/fov")                 |
| [aspectRatio](#aspectratio) | `number` | Optional | cannot be null | [MSGAssets-camera](camera-definitions-cameraprojectionperspective-properties-aspectratio.md "camera.schema.json#/definitions/cameraProjectionPerspective/properties/aspectRatio") |
| [znear](#znear)             | `number` | Optional | cannot be null | [MSGAssets-camera](camera-definitions-cameraprojectionperspective-properties-znear.md "camera.schema.json#/definitions/cameraProjectionPerspective/properties/znear")             |
| [zfar](#zfar)               | `number` | Optional | cannot be null | [MSGAssets-camera](camera-definitions-cameraprojectionperspective-properties-zfar.md "camera.schema.json#/definitions/cameraProjectionPerspective/properties/zfar")               |

## fov

the field of view of the camera

`fov`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-camera](camera-definitions-cameraprojectionperspective-properties-fov.md "camera.schema.json#/definitions/cameraProjectionPerspective/properties/fov")

### fov Type

`number`

### fov Constraints

**maximum**: the value of this number must smaller than or equal to: `179`

**minimum**: the value of this number must greater than or equal to: `0.1`

### fov Default Value

The default value is:

```json
45
```

## aspectRatio

the ratio between viewport width/height

`aspectRatio`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-camera](camera-definitions-cameraprojectionperspective-properties-aspectratio.md "camera.schema.json#/definitions/cameraProjectionPerspective/properties/aspectRatio")

### aspectRatio Type

`number`

### aspectRatio Constraints

**maximum**: the value of this number must smaller than or equal to: `1`

**minimum**: the value of this number must greater than or equal to: `0.1`

### aspectRatio Default Value

The default value is:

```json
"16/9"
```

## znear

the closest clipping plane

`znear`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-camera](camera-definitions-cameraprojectionperspective-properties-znear.md "camera.schema.json#/definitions/cameraProjectionPerspective/properties/znear")

### znear Type

`number`

### znear Constraints

**minimum**: the value of this number must greater than or equal to: `0.00001`

### znear Default Value

The default value is:

```json
0.1
```

## zfar

the farthest clipping plane

`zfar`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-camera](camera-definitions-cameraprojectionperspective-properties-zfar.md "camera.schema.json#/definitions/cameraProjectionPerspective/properties/zfar")

### zfar Type

`number`

### zfar Constraints

**minimum**: the value of this number must greater than or equal to: `0.00001`

### zfar Default Value

The default value is:

```json
0.1
```

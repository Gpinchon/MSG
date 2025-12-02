# CameraProjectionPerspectiveInfinite Schema

```txt
camera.schema.json#/definitions/cameraProjectionPerspectiveInfinite
```



| Abstract            | Extensible | Status         | Identifiable | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                                  |
| :------------------ | :--------- | :------------- | :----------- | :---------------- | :-------------------- | :------------------ | :-------------------------------------------------------------------------- |
| Can be instantiated | No         | Unknown status | No           | Forbidden         | Allowed               | none                | [camera.schema.json\*](json/defs/camera.schema.json "open original schema") |

## cameraProjectionPerspectiveInfinite Type

`object` ([CameraProjectionPerspectiveInfinite](camera-definitions-cameraprojectionperspectiveinfinite.md))

# cameraProjectionPerspectiveInfinite Properties

| Property                    | Type     | Required | Nullable       | Defined by                                                                                                                                                                                        |
| :-------------------------- | :------- | :------- | :------------- | :------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| [fov](#fov)                 | `number` | Optional | cannot be null | [MSGAssets-camera](camera-definitions-cameraprojectionperspectiveinfinite-properties-fov.md "camera.schema.json#/definitions/cameraProjectionPerspectiveInfinite/properties/fov")                 |
| [aspectRatio](#aspectratio) | `number` | Optional | cannot be null | [MSGAssets-camera](camera-definitions-cameraprojectionperspectiveinfinite-properties-aspectratio.md "camera.schema.json#/definitions/cameraProjectionPerspectiveInfinite/properties/aspectRatio") |
| [znear](#znear)             | `number` | Optional | cannot be null | [MSGAssets-camera](camera-definitions-cameraprojectionperspectiveinfinite-properties-znear.md "camera.schema.json#/definitions/cameraProjectionPerspectiveInfinite/properties/znear")             |

## fov

the field of view in degrees

`fov`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-camera](camera-definitions-cameraprojectionperspectiveinfinite-properties-fov.md "camera.schema.json#/definitions/cameraProjectionPerspectiveInfinite/properties/fov")

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

* defined in: [MSGAssets-camera](camera-definitions-cameraprojectionperspectiveinfinite-properties-aspectratio.md "camera.schema.json#/definitions/cameraProjectionPerspectiveInfinite/properties/aspectRatio")

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

* defined in: [MSGAssets-camera](camera-definitions-cameraprojectionperspectiveinfinite-properties-znear.md "camera.schema.json#/definitions/cameraProjectionPerspectiveInfinite/properties/znear")

### znear Type

`number`

### znear Constraints

**minimum**: the value of this number must greater than or equal to: `0.00001`

### znear Default Value

The default value is:

```json
0.1
```

# CameraProjection Schema

```txt
camera.schema.json#/definitions/cameraProjection
```



| Abstract            | Extensible | Status         | Identifiable | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                                  |
| :------------------ | :--------- | :------------- | :----------- | :---------------- | :-------------------- | :------------------ | :-------------------------------------------------------------------------- |
| Can be instantiated | No         | Unknown status | No           | Forbidden         | Allowed               | none                | [camera.schema.json\*](json/defs/camera.schema.json "open original schema") |

## cameraProjection Type

`object` ([CameraProjection](camera-definitions-cameraprojection.md))

# cameraProjection Properties

| Property      | Type     | Required | Nullable       | Defined by                                                                                                              |
| :------------ | :------- | :------- | :------------- | :---------------------------------------------------------------------------------------------------------------------- |
| [type](#type) | `string` | Required | cannot be null | [MSGAssets-camera](camera-definitions-cameratype.md "camera.schema.json#/definitions/cameraProjection/properties/type") |
| [data](#data) | Merged   | Optional | cannot be null | [MSGAssets-camera](camera-definitions-lightdata.md "camera.schema.json#/definitions/cameraProjection/properties/data")  |

## type



`type`

* is required

* Type: `string`

* cannot be null

* defined in: [MSGAssets-camera](camera-definitions-cameratype.md "camera.schema.json#/definitions/cameraProjection/properties/type")

### type Type

`string`

### type Constraints

**enum**: the value of this property must be equal to one of the following values:

| Value                   | Explanation |
| :---------------------- | :---------- |
| `"PerspectiveInfinite"` |             |
| `"Perspective"`         |             |
| `"Orthographic"`        |             |

## data



`data`

* is optional

* Type: `object` ([LightData](camera-definitions-lightdata.md))

* cannot be null

* defined in: [MSGAssets-camera](camera-definitions-lightdata.md "camera.schema.json#/definitions/cameraProjection/properties/data")

### data Type

`object` ([LightData](camera-definitions-lightdata.md))

one (and only one) of

* [CameraProjectionPerspectiveInfinite](camera-definitions-cameraprojectionperspectiveinfinite.md "check type definition")

* [CameraProjectionPerspective](camera-definitions-cameraprojectionperspective.md "check type definition")

* [CameraProjectionOrthographic](camera-definitions-cameraprojectionorthographic.md "check type definition")

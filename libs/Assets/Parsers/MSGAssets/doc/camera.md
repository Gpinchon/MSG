# MSGAssets-camera Schema

```txt
camera.schema.json
```



| Abstract               | Extensible | Status         | Identifiable            | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                                |
| :--------------------- | :--------- | :------------- | :---------------------- | :---------------- | :-------------------- | :------------------ | :------------------------------------------------------------------------ |
| Cannot be instantiated | Yes        | Unknown status | Unknown identifiability | Forbidden         | Allowed               | none                | [camera.schema.json](json/defs/camera.schema.json "open original schema") |

## MSGAssets-camera Type

`object` ([MSGAssets-camera](camera.md))

# MSGAssets-camera Definitions

## Definitions group cameraType

Reference this group by using

```json
{"$ref":"camera.schema.json#/definitions/cameraType"}
```

| Property | Type | Required | Nullable | Defined by |
| :------- | :--- | :------- | :------- | :--------- |

## Definitions group cameraProjectionPerspectiveInfinite

Reference this group by using

```json
{"$ref":"camera.schema.json#/definitions/cameraProjectionPerspectiveInfinite"}
```

| Property                    | Type     | Required | Nullable       | Defined by                                                                                                                                                                                        |
| :-------------------------- | :------- | :------- | :------------- | :------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| [fov](#fov)                 | `number` | Optional | cannot be null | [MSGAssets-camera](camera-definitions-cameraprojectionperspectiveinfinite-properties-fov.md "camera.schema.json#/definitions/cameraProjectionPerspectiveInfinite/properties/fov")                 |
| [aspectRatio](#aspectratio) | `number` | Optional | cannot be null | [MSGAssets-camera](camera-definitions-cameraprojectionperspectiveinfinite-properties-aspectratio.md "camera.schema.json#/definitions/cameraProjectionPerspectiveInfinite/properties/aspectRatio") |
| [znear](#znear)             | `number` | Optional | cannot be null | [MSGAssets-camera](camera-definitions-cameraprojectionperspectiveinfinite-properties-znear.md "camera.schema.json#/definitions/cameraProjectionPerspectiveInfinite/properties/znear")             |

### fov

the field of view in degrees

`fov`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-camera](camera-definitions-cameraprojectionperspectiveinfinite-properties-fov.md "camera.schema.json#/definitions/cameraProjectionPerspectiveInfinite/properties/fov")

#### fov Type

`number`

#### fov Constraints

**maximum**: the value of this number must smaller than or equal to: `179`

**minimum**: the value of this number must greater than or equal to: `0.1`

#### fov Default Value

The default value is:

```json
45
```

### aspectRatio

the ratio between viewport width/height

`aspectRatio`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-camera](camera-definitions-cameraprojectionperspectiveinfinite-properties-aspectratio.md "camera.schema.json#/definitions/cameraProjectionPerspectiveInfinite/properties/aspectRatio")

#### aspectRatio Type

`number`

#### aspectRatio Constraints

**maximum**: the value of this number must smaller than or equal to: `1`

**minimum**: the value of this number must greater than or equal to: `0.1`

#### aspectRatio Default Value

The default value is:

```json
"16/9"
```

### znear

the closest clipping plane

`znear`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-camera](camera-definitions-cameraprojectionperspectiveinfinite-properties-znear.md "camera.schema.json#/definitions/cameraProjectionPerspectiveInfinite/properties/znear")

#### znear Type

`number`

#### znear Constraints

**minimum**: the value of this number must greater than or equal to: `0.00001`

#### znear Default Value

The default value is:

```json
0.1
```

## Definitions group cameraProjectionPerspective

Reference this group by using

```json
{"$ref":"camera.schema.json#/definitions/cameraProjectionPerspective"}
```

| Property                      | Type     | Required | Nullable       | Defined by                                                                                                                                                                        |
| :---------------------------- | :------- | :------- | :------------- | :-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| [fov](#fov-1)                 | `number` | Optional | cannot be null | [MSGAssets-camera](camera-definitions-cameraprojectionperspective-properties-fov.md "camera.schema.json#/definitions/cameraProjectionPerspective/properties/fov")                 |
| [aspectRatio](#aspectratio-1) | `number` | Optional | cannot be null | [MSGAssets-camera](camera-definitions-cameraprojectionperspective-properties-aspectratio.md "camera.schema.json#/definitions/cameraProjectionPerspective/properties/aspectRatio") |
| [znear](#znear-1)             | `number` | Optional | cannot be null | [MSGAssets-camera](camera-definitions-cameraprojectionperspective-properties-znear.md "camera.schema.json#/definitions/cameraProjectionPerspective/properties/znear")             |
| [zfar](#zfar)                 | `number` | Optional | cannot be null | [MSGAssets-camera](camera-definitions-cameraprojectionperspective-properties-zfar.md "camera.schema.json#/definitions/cameraProjectionPerspective/properties/zfar")               |

### fov

the field of view of the camera

`fov`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-camera](camera-definitions-cameraprojectionperspective-properties-fov.md "camera.schema.json#/definitions/cameraProjectionPerspective/properties/fov")

#### fov Type

`number`

#### fov Constraints

**maximum**: the value of this number must smaller than or equal to: `179`

**minimum**: the value of this number must greater than or equal to: `0.1`

#### fov Default Value

The default value is:

```json
45
```

### aspectRatio

the ratio between viewport width/height

`aspectRatio`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-camera](camera-definitions-cameraprojectionperspective-properties-aspectratio.md "camera.schema.json#/definitions/cameraProjectionPerspective/properties/aspectRatio")

#### aspectRatio Type

`number`

#### aspectRatio Constraints

**maximum**: the value of this number must smaller than or equal to: `1`

**minimum**: the value of this number must greater than or equal to: `0.1`

#### aspectRatio Default Value

The default value is:

```json
"16/9"
```

### znear

the closest clipping plane

`znear`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-camera](camera-definitions-cameraprojectionperspective-properties-znear.md "camera.schema.json#/definitions/cameraProjectionPerspective/properties/znear")

#### znear Type

`number`

#### znear Constraints

**minimum**: the value of this number must greater than or equal to: `0.00001`

#### znear Default Value

The default value is:

```json
0.1
```

### zfar

the farthest clipping plane

`zfar`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-camera](camera-definitions-cameraprojectionperspective-properties-zfar.md "camera.schema.json#/definitions/cameraProjectionPerspective/properties/zfar")

#### zfar Type

`number`

#### zfar Constraints

**minimum**: the value of this number must greater than or equal to: `0.00001`

#### zfar Default Value

The default value is:

```json
0.1
```

## Definitions group cameraProjectionOrthographic

Reference this group by using

```json
{"$ref":"camera.schema.json#/definitions/cameraProjectionOrthographic"}
```

| Property          | Type     | Required | Nullable       | Defined by                                                                                                                                                                |
| :---------------- | :------- | :------- | :------------- | :------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| [left](#left)     | `number` | Optional | cannot be null | [MSGAssets-camera](camera-definitions-cameraprojectionorthographic-properties-left.md "camera.schema.json#/definitions/cameraProjectionOrthographic/properties/left")     |
| [right](#right)   | `number` | Optional | cannot be null | [MSGAssets-camera](camera-definitions-cameraprojectionorthographic-properties-right.md "camera.schema.json#/definitions/cameraProjectionOrthographic/properties/right")   |
| [bottom](#bottom) | `number` | Optional | cannot be null | [MSGAssets-camera](camera-definitions-cameraprojectionorthographic-properties-bottom.md "camera.schema.json#/definitions/cameraProjectionOrthographic/properties/bottom") |
| [top](#top)       | `number` | Optional | cannot be null | [MSGAssets-camera](camera-definitions-cameraprojectionorthographic-properties-top.md "camera.schema.json#/definitions/cameraProjectionOrthographic/properties/top")       |
| [znear](#znear-2) | `number` | Optional | cannot be null | [MSGAssets-camera](camera-definitions-cameraprojectionorthographic-properties-znear.md "camera.schema.json#/definitions/cameraProjectionOrthographic/properties/znear")   |
| [zfar](#zfar-1)   | `number` | Optional | cannot be null | [MSGAssets-camera](camera-definitions-cameraprojectionorthographic-properties-zfar.md "camera.schema.json#/definitions/cameraProjectionOrthographic/properties/zfar")     |

### left



`left`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-camera](camera-definitions-cameraprojectionorthographic-properties-left.md "camera.schema.json#/definitions/cameraProjectionOrthographic/properties/left")

#### left Type

`number`

#### left Default Value

The default value is:

```json
-50
```

### right



`right`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-camera](camera-definitions-cameraprojectionorthographic-properties-right.md "camera.schema.json#/definitions/cameraProjectionOrthographic/properties/right")

#### right Type

`number`

#### right Default Value

The default value is:

```json
50
```

### bottom



`bottom`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-camera](camera-definitions-cameraprojectionorthographic-properties-bottom.md "camera.schema.json#/definitions/cameraProjectionOrthographic/properties/bottom")

#### bottom Type

`number`

#### bottom Default Value

The default value is:

```json
-50
```

### top



`top`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-camera](camera-definitions-cameraprojectionorthographic-properties-top.md "camera.schema.json#/definitions/cameraProjectionOrthographic/properties/top")

#### top Type

`number`

#### top Default Value

The default value is:

```json
50
```

### znear



`znear`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-camera](camera-definitions-cameraprojectionorthographic-properties-znear.md "camera.schema.json#/definitions/cameraProjectionOrthographic/properties/znear")

#### znear Type

`number`

#### znear Default Value

The default value is:

```json
0.1
```

### zfar



`zfar`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-camera](camera-definitions-cameraprojectionorthographic-properties-zfar.md "camera.schema.json#/definitions/cameraProjectionOrthographic/properties/zfar")

#### zfar Type

`number`

#### zfar Default Value

The default value is:

```json
1000
```

## Definitions group cameraProjectionData

Reference this group by using

```json
{"$ref":"camera.schema.json#/definitions/cameraProjectionData"}
```

| Property | Type | Required | Nullable | Defined by |
| :------- | :--- | :------- | :------- | :--------- |

## Definitions group cameraProjection

Reference this group by using

```json
{"$ref":"camera.schema.json#/definitions/cameraProjection"}
```

| Property      | Type     | Required | Nullable       | Defined by                                                                                                                                    |
| :------------ | :------- | :------- | :------------- | :-------------------------------------------------------------------------------------------------------------------------------------------- |
| [type](#type) | `string` | Required | cannot be null | [MSGAssets-camera](camera-definitions-cameraprojection-properties-type.md "camera.schema.json#/definitions/cameraProjection/properties/type") |
| [data](#data) | Merged   | Optional | cannot be null | [MSGAssets-camera](camera-definitions-lightdata.md "camera.schema.json#/definitions/cameraProjection/properties/data")                        |

### type



`type`

* is required

* Type: `string`

* cannot be null

* defined in: [MSGAssets-camera](camera-definitions-cameraprojection-properties-type.md "camera.schema.json#/definitions/cameraProjection/properties/type")

#### type Type

`string`

#### type Constraints

**enum**: the value of this property must be equal to one of the following values:

| Value                   | Explanation |
| :---------------------- | :---------- |
| `"PerspectiveInfinite"` |             |
| `"Perspective"`         |             |
| `"Orthographic"`        |             |

### data



`data`

* is optional

* Type: `object` ([LightData](camera-definitions-lightdata.md))

* cannot be null

* defined in: [MSGAssets-camera](camera-definitions-lightdata.md "camera.schema.json#/definitions/cameraProjection/properties/data")

#### data Type

`object` ([LightData](camera-definitions-lightdata.md))

one (and only one) of

* [CameraProjectionPerspectiveInfinite](camera-definitions-cameraprojectionperspectiveinfinite.md "check type definition")

* [CameraProjectionPerspective](camera-definitions-cameraprojectionperspective.md "check type definition")

* [CameraProjectionOrthographic](camera-definitions-cameraprojectionorthographic.md "check type definition")

## Definitions group cameraSettings

Reference this group by using

```json
{"$ref":"camera.schema.json#/definitions/cameraSettings"}
```

| Property | Type | Required | Nullable | Defined by |
| :------- | :--- | :------- | :------- | :--------- |

## Definitions group camera

Reference this group by using

```json
{"$ref":"camera.schema.json#/definitions/camera"}
```

| Property                      | Type          | Required | Nullable       | Defined by                                                                                                                                |
| :---------------------------- | :------------ | :------- | :------------- | :---------------------------------------------------------------------------------------------------------------------------------------- |
| [copyFrom](#copyfrom)         | `string`      | Optional | cannot be null | [MSGAssets-camera](camera-definitions-camera-properties-fromentity.md "camera.schema.json#/definitions/camera/properties/copyFrom")       |
| [projection](#projection)     | `object`      | Optional | cannot be null | [MSGAssets-camera](camera-definitions-cameraprojection.md "camera.schema.json#/definitions/camera/properties/projection")                 |
| [colorGrading](#colorgrading) | Not specified | Optional | cannot be null | [MSGAssets-camera](camera-definitions-camera-properties-colorgrading.md "camera.schema.json#/definitions/camera/properties/colorGrading") |
| [toneMapping](#tonemapping)   | Not specified | Optional | cannot be null | [MSGAssets-camera](camera-definitions-camera-properties-tonemapping.md "camera.schema.json#/definitions/camera/properties/toneMapping")   |
| [bloom](#bloom)               | `object`      | Optional | cannot be null | [MSGAssets-camera](tonemapping-definitions-bloomsettings.md "camera.schema.json#/definitions/camera/properties/bloom")                    |

### copyFrom

the name of the entity to copy the component from

`copyFrom`

* is optional

* Type: `string` ([FromEntity](camera-definitions-camera-properties-fromentity.md))

* cannot be null

* defined in: [MSGAssets-camera](camera-definitions-camera-properties-fromentity.md "camera.schema.json#/definitions/camera/properties/copyFrom")

#### copyFrom Type

`string` ([FromEntity](camera-definitions-camera-properties-fromentity.md))

### projection



`projection`

* is optional

* Type: `object` ([CameraProjection](camera-definitions-cameraprojection.md))

* cannot be null

* defined in: [MSGAssets-camera](camera-definitions-cameraprojection.md "camera.schema.json#/definitions/camera/properties/projection")

#### projection Type

`object` ([CameraProjection](camera-definitions-cameraprojection.md))

### colorGrading



`colorGrading`

* is optional

* Type: unknown

* cannot be null

* defined in: [MSGAssets-camera](camera-definitions-camera-properties-colorgrading.md "camera.schema.json#/definitions/camera/properties/colorGrading")

#### colorGrading Type

unknown

### toneMapping



`toneMapping`

* is optional

* Type: unknown

* cannot be null

* defined in: [MSGAssets-camera](camera-definitions-camera-properties-tonemapping.md "camera.schema.json#/definitions/camera/properties/toneMapping")

#### toneMapping Type

unknown

### bloom



`bloom`

* is optional

* Type: `object` ([BloomSettings](tonemapping-definitions-bloomsettings.md))

* cannot be null

* defined in: [MSGAssets-camera](tonemapping-definitions-bloomsettings.md "camera.schema.json#/definitions/camera/properties/bloom")

#### bloom Type

`object` ([BloomSettings](tonemapping-definitions-bloomsettings.md))

#### bloom Default Value

The default value is:

```json
[
  1,
  1,
  1
]
```

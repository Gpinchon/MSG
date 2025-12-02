# CameraProjectionOrthographic Schema

```txt
camera.schema.json#/definitions/cameraProjectionOrthographic
```



| Abstract            | Extensible | Status         | Identifiable | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                                  |
| :------------------ | :--------- | :------------- | :----------- | :---------------- | :-------------------- | :------------------ | :-------------------------------------------------------------------------- |
| Can be instantiated | No         | Unknown status | No           | Forbidden         | Allowed               | none                | [camera.schema.json\*](json/defs/camera.schema.json "open original schema") |

## cameraProjectionOrthographic Type

`object` ([CameraProjectionOrthographic](camera-definitions-cameraprojectionorthographic.md))

# cameraProjectionOrthographic Properties

| Property          | Type     | Required | Nullable       | Defined by                                                                                                                                                                |
| :---------------- | :------- | :------- | :------------- | :------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| [left](#left)     | `number` | Optional | cannot be null | [MSGAssets-camera](camera-definitions-cameraprojectionorthographic-properties-left.md "camera.schema.json#/definitions/cameraProjectionOrthographic/properties/left")     |
| [right](#right)   | `number` | Optional | cannot be null | [MSGAssets-camera](camera-definitions-cameraprojectionorthographic-properties-right.md "camera.schema.json#/definitions/cameraProjectionOrthographic/properties/right")   |
| [bottom](#bottom) | `number` | Optional | cannot be null | [MSGAssets-camera](camera-definitions-cameraprojectionorthographic-properties-bottom.md "camera.schema.json#/definitions/cameraProjectionOrthographic/properties/bottom") |
| [top](#top)       | `number` | Optional | cannot be null | [MSGAssets-camera](camera-definitions-cameraprojectionorthographic-properties-top.md "camera.schema.json#/definitions/cameraProjectionOrthographic/properties/top")       |
| [znear](#znear)   | `number` | Optional | cannot be null | [MSGAssets-camera](camera-definitions-cameraprojectionorthographic-properties-znear.md "camera.schema.json#/definitions/cameraProjectionOrthographic/properties/znear")   |
| [zfar](#zfar)     | `number` | Optional | cannot be null | [MSGAssets-camera](camera-definitions-cameraprojectionorthographic-properties-zfar.md "camera.schema.json#/definitions/cameraProjectionOrthographic/properties/zfar")     |

## left



`left`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-camera](camera-definitions-cameraprojectionorthographic-properties-left.md "camera.schema.json#/definitions/cameraProjectionOrthographic/properties/left")

### left Type

`number`

### left Default Value

The default value is:

```json
-50
```

## right



`right`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-camera](camera-definitions-cameraprojectionorthographic-properties-right.md "camera.schema.json#/definitions/cameraProjectionOrthographic/properties/right")

### right Type

`number`

### right Default Value

The default value is:

```json
50
```

## bottom



`bottom`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-camera](camera-definitions-cameraprojectionorthographic-properties-bottom.md "camera.schema.json#/definitions/cameraProjectionOrthographic/properties/bottom")

### bottom Type

`number`

### bottom Default Value

The default value is:

```json
-50
```

## top



`top`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-camera](camera-definitions-cameraprojectionorthographic-properties-top.md "camera.schema.json#/definitions/cameraProjectionOrthographic/properties/top")

### top Type

`number`

### top Default Value

The default value is:

```json
50
```

## znear



`znear`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-camera](camera-definitions-cameraprojectionorthographic-properties-znear.md "camera.schema.json#/definitions/cameraProjectionOrthographic/properties/znear")

### znear Type

`number`

### znear Default Value

The default value is:

```json
0.1
```

## zfar



`zfar`

* is optional

* Type: `number`

* cannot be null

* defined in: [MSGAssets-camera](camera-definitions-cameraprojectionorthographic-properties-zfar.md "camera.schema.json#/definitions/cameraProjectionOrthographic/properties/zfar")

### zfar Type

`number`

### zfar Default Value

The default value is:

```json
1000
```

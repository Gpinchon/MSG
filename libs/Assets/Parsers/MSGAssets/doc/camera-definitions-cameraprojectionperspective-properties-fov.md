# Untitled number in MSGAssets-camera Schema

```txt
camera.schema.json#/definitions/cameraProjectionPerspective/properties/fov
```

the field of view of the camera

| Abstract            | Extensible | Status         | Identifiable            | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                                  |
| :------------------ | :--------- | :------------- | :---------------------- | :---------------- | :-------------------- | :------------------ | :-------------------------------------------------------------------------- |
| Can be instantiated | No         | Unknown status | Unknown identifiability | Forbidden         | Allowed               | none                | [camera.schema.json\*](json/defs/camera.schema.json "open original schema") |

## fov Type

`number`

## fov Constraints

**maximum**: the value of this number must smaller than or equal to: `179`

**minimum**: the value of this number must greater than or equal to: `0.1`

## fov Default Value

The default value is:

```json
45
```

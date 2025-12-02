# Untitled number in MSGAssets-camera Schema

```txt
camera.schema.json#/definitions/cameraProjectionPerspective/properties/zfar
```

the farthest clipping plane

| Abstract            | Extensible | Status         | Identifiable            | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                                  |
| :------------------ | :--------- | :------------- | :---------------------- | :---------------- | :-------------------- | :------------------ | :-------------------------------------------------------------------------- |
| Can be instantiated | No         | Unknown status | Unknown identifiability | Forbidden         | Allowed               | none                | [camera.schema.json\*](json/defs/camera.schema.json "open original schema") |

## zfar Type

`number`

## zfar Constraints

**minimum**: the value of this number must greater than or equal to: `0.00001`

## zfar Default Value

The default value is:

```json
0.1
```

# Untitled number in MSGAssets-camera Schema

```txt
camera.schema.json#/definitions/cameraProjectionPerspective/properties/aspectRatio
```

the ratio between viewport width/height

| Abstract            | Extensible | Status         | Identifiable            | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                                  |
| :------------------ | :--------- | :------------- | :---------------------- | :---------------- | :-------------------- | :------------------ | :-------------------------------------------------------------------------- |
| Can be instantiated | No         | Unknown status | Unknown identifiability | Forbidden         | Allowed               | none                | [camera.schema.json\*](json/defs/camera.schema.json "open original schema") |

## aspectRatio Type

`number`

## aspectRatio Constraints

**maximum**: the value of this number must smaller than or equal to: `1`

**minimum**: the value of this number must greater than or equal to: `0.1`

## aspectRatio Default Value

The default value is:

```json
"16/9"
```

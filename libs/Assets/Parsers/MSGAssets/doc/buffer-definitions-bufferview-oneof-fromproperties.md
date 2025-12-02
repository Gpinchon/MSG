# FromProperties Schema

```txt
buffer.schema.json#/definitions/bufferView/oneOf/0
```



| Abstract            | Extensible | Status         | Identifiable | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                                  |
| :------------------ | :--------- | :------------- | :----------- | :---------------- | :-------------------- | :------------------ | :-------------------------------------------------------------------------- |
| Can be instantiated | No         | Unknown status | No           | Forbidden         | Allowed               | none                | [buffer.schema.json\*](json/defs/buffer.schema.json "open original schema") |

## 0 Type

unknown ([FromProperties](buffer-definitions-bufferview-oneof-fromproperties.md))

# 0 Properties

| Property          | Type     | Required | Nullable       | Defined by                                                                                                                                                         |
| :---------------- | :------- | :------- | :------------- | :----------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| [name](#name)     | `string` | Required | cannot be null | [MSGAssets-buffer](buffer-definitions-bufferview-oneof-fromproperties-properties-name.md "buffer.schema.json#/definitions/bufferView/oneOf/0/properties/name")     |
| [size](#size)     | `number` | Required | cannot be null | [MSGAssets-buffer](buffer-definitions-bufferview-oneof-fromproperties-properties-size.md "buffer.schema.json#/definitions/bufferView/oneOf/0/properties/size")     |
| [offset](#offset) | `number` | Required | cannot be null | [MSGAssets-buffer](buffer-definitions-bufferview-oneof-fromproperties-properties-offset.md "buffer.schema.json#/definitions/bufferView/oneOf/0/properties/offset") |
| [buffer](#buffer) | `number` | Required | cannot be null | [MSGAssets-buffer](buffer-definitions-bufferview-oneof-fromproperties-properties-buffer.md "buffer.schema.json#/definitions/bufferView/oneOf/0/properties/buffer") |

## name

the object's name, important because it can be used to reference it

`name`

* is required

* Type: `string`

* cannot be null

* defined in: [MSGAssets-buffer](buffer-definitions-bufferview-oneof-fromproperties-properties-name.md "buffer.schema.json#/definitions/bufferView/oneOf/0/properties/name")

### name Type

`string`

## size

the size of this buffer view in bytes, only necessary if this bufferView uses a buffer a its source

`size`

* is required

* Type: `number`

* cannot be null

* defined in: [MSGAssets-buffer](buffer-definitions-bufferview-oneof-fromproperties-properties-size.md "buffer.schema.json#/definitions/bufferView/oneOf/0/properties/size")

### size Type

`number`

### size Constraints

**minimum**: the value of this number must greater than or equal to: `0`

## offset

the offset of this buffer view inside the buffer in bytes, only necessary if this bufferView uses a buffer a its source

`offset`

* is required

* Type: `number`

* cannot be null

* defined in: [MSGAssets-buffer](buffer-definitions-bufferview-oneof-fromproperties-properties-offset.md "buffer.schema.json#/definitions/bufferView/oneOf/0/properties/offset")

### offset Type

`number`

### offset Constraints

**minimum**: the value of this number must greater than or equal to: `0`

## buffer

the index of the buffer into the buffers list

`buffer`

* is required

* Type: `number`

* cannot be null

* defined in: [MSGAssets-buffer](buffer-definitions-bufferview-oneof-fromproperties-properties-buffer.md "buffer.schema.json#/definitions/bufferView/oneOf/0/properties/buffer")

### buffer Type

`number`

### buffer Constraints

**minimum**: the value of this number must greater than or equal to: `0`

# Untitled number in MSGAssets-buffer Schema

```txt
buffer.schema.json#/definitions/bufferView/oneOf/0/properties/offset
```

the offset of this buffer view inside the buffer in bytes, only necessary if this bufferView uses a buffer a its source

| Abstract            | Extensible | Status         | Identifiable            | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                                  |
| :------------------ | :--------- | :------------- | :---------------------- | :---------------- | :-------------------- | :------------------ | :-------------------------------------------------------------------------- |
| Can be instantiated | No         | Unknown status | Unknown identifiability | Forbidden         | Allowed               | none                | [buffer.schema.json\*](json/defs/buffer.schema.json "open original schema") |

## offset Type

`number`

## offset Constraints

**minimum**: the value of this number must greater than or equal to: `0`

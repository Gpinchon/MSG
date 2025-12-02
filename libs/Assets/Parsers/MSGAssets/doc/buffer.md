# MSGAssets-buffer Schema

```txt
buffer.schema.json
```



| Abstract               | Extensible | Status         | Identifiable            | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                                |
| :--------------------- | :--------- | :------------- | :---------------------- | :---------------- | :-------------------- | :------------------ | :------------------------------------------------------------------------ |
| Cannot be instantiated | Yes        | Unknown status | Unknown identifiability | Forbidden         | Allowed               | none                | [buffer.schema.json](json/defs/buffer.schema.json "open original schema") |

## MSGAssets-buffer Type

`object` ([MSGAssets-buffer](buffer.md))

# MSGAssets-buffer Definitions

## Definitions group buffer

Reference this group by using

```json
{"$ref":"buffer.schema.json#/definitions/buffer"}
```

| Property      | Type     | Required | Nullable       | Defined by                                                                                                                |
| :------------ | :------- | :------- | :------------- | :------------------------------------------------------------------------------------------------------------------------ |
| [name](#name) | `string` | Optional | cannot be null | [MSGAssets-buffer](buffer-definitions-buffer-properties-name.md "buffer.schema.json#/definitions/buffer/properties/name") |
| [uri](#uri)   | `string` | Required | cannot be null | [MSGAssets-buffer](buffer-definitions-buffer-properties-uri.md "buffer.schema.json#/definitions/buffer/properties/uri")   |

### name

the object's name, important because it can be used to reference it

`name`

* is optional

* Type: `string`

* cannot be null

* defined in: [MSGAssets-buffer](buffer-definitions-buffer-properties-name.md "buffer.schema.json#/definitions/buffer/properties/name")

#### name Type

`string`

### uri

links to an object loaded from an external resource. To load from the externals list, use the "external" scheme then the external's name followed by the object name as a query, example: "external:ExternalName?ObjectName"

`uri`

* is required

* Type: `string` ([URI](buffer-definitions-buffer-properties-uri.md))

* cannot be null

* defined in: [MSGAssets-buffer](buffer-definitions-buffer-properties-uri.md "buffer.schema.json#/definitions/buffer/properties/uri")

#### uri Type

`string` ([URI](buffer-definitions-buffer-properties-uri.md))

#### uri Constraints

**URI**: the string must be a URI, according to [RFC 3986](https://tools.ietf.org/html/rfc3986 "check the specification")

## Definitions group bufferView

Reference this group by using

```json
{"$ref":"buffer.schema.json#/definitions/bufferView"}
```

| Property | Type | Required | Nullable | Defined by |
| :------- | :--- | :------- | :------- | :--------- |

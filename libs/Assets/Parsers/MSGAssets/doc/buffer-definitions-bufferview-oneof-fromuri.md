# FromURI Schema

```txt
buffer.schema.json#/definitions/bufferView/oneOf/1
```



| Abstract            | Extensible | Status         | Identifiable | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                                  |
| :------------------ | :--------- | :------------- | :----------- | :---------------- | :-------------------- | :------------------ | :-------------------------------------------------------------------------- |
| Can be instantiated | No         | Unknown status | No           | Forbidden         | Allowed               | none                | [buffer.schema.json\*](json/defs/buffer.schema.json "open original schema") |

## 1 Type

unknown ([FromURI](buffer-definitions-bufferview-oneof-fromuri.md))

# 1 Properties

| Property      | Type     | Required | Nullable       | Defined by                                                                                                                        |
| :------------ | :------- | :------- | :------------- | :-------------------------------------------------------------------------------------------------------------------------------- |
| [name](#name) | `string` | Optional | cannot be null | [MSGAssets-external](external-definitions-fromuri-properties-name.md "external.schema.json#/definitions/fromURI/properties/name") |
| [uri](#uri)   | `string` | Required | cannot be null | [MSGAssets-external](external-definitions-fromuri-properties-uri.md "external.schema.json#/definitions/fromURI/properties/uri")   |

## name

the object's name, important because it can be used to reference it

`name`

* is optional

* Type: `string`

* cannot be null

* defined in: [MSGAssets-external](external-definitions-fromuri-properties-name.md "external.schema.json#/definitions/fromURI/properties/name")

### name Type

`string`

## uri

links to an object loaded from an external resource. To load from the externals list, use the "external" scheme then the external's name followed by the object name as a query, example: "external:ExternalName?ObjectName"

`uri`

* is required

* Type: `string` ([URI](external-definitions-fromuri-properties-uri.md))

* cannot be null

* defined in: [MSGAssets-external](external-definitions-fromuri-properties-uri.md "external.schema.json#/definitions/fromURI/properties/uri")

### uri Type

`string` ([URI](external-definitions-fromuri-properties-uri.md))

### uri Constraints

**URI**: the string must be a URI, according to [RFC 3986](https://tools.ietf.org/html/rfc3986 "check the specification")

# FromURI Schema

```txt
mesh.schema.json#/definitions/primitive/oneOf/1
```



| Abstract            | Extensible | Status         | Identifiable | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                              |
| :------------------ | :--------- | :------------- | :----------- | :---------------- | :-------------------- | :------------------ | :---------------------------------------------------------------------- |
| Can be instantiated | No         | Unknown status | No           | Forbidden         | Allowed               | none                | [mesh.schema.json\*](json/defs/mesh.schema.json "open original schema") |

## 1 Type

unknown ([FromURI](mesh-definitions-meshprimitive-oneof-fromuri.md))

# 1 Properties

| Property      | Type     | Required | Nullable       | Defined by                                                                                                                                          |
| :------------ | :------- | :------- | :------------- | :-------------------------------------------------------------------------------------------------------------------------------------------------- |
| [name](#name) | `string` | Optional | cannot be null | [MSGAssets-mesh](mesh-definitions-meshprimitive-oneof-fromuri-properties-name.md "mesh.schema.json#/definitions/primitive/oneOf/1/properties/name") |
| [uri](#uri)   | `string` | Optional | cannot be null | [MSGAssets-mesh](mesh-definitions-meshprimitive-oneof-fromuri-properties-uri.md "mesh.schema.json#/definitions/primitive/oneOf/1/properties/uri")   |

## name

this object's name, important because it allows for it to be referenced in another asset file

`name`

* is optional

* Type: `string`

* cannot be null

* defined in: [MSGAssets-mesh](mesh-definitions-meshprimitive-oneof-fromuri-properties-name.md "mesh.schema.json#/definitions/primitive/oneOf/1/properties/name")

### name Type

`string`

## uri

links to an object loaded from an external resource. To load from the externals list, use the "external" scheme then the external's name followed by the object name as a query, example: "external:ExternalName?ObjectName"

`uri`

* is optional

* Type: `string` ([URI](mesh-definitions-meshprimitive-oneof-fromuri-properties-uri.md))

* cannot be null

* defined in: [MSGAssets-mesh](mesh-definitions-meshprimitive-oneof-fromuri-properties-uri.md "mesh.schema.json#/definitions/primitive/oneOf/1/properties/uri")

### uri Type

`string` ([URI](mesh-definitions-meshprimitive-oneof-fromuri-properties-uri.md))

### uri Constraints

**URI**: the string must be a URI, according to [RFC 3986](https://tools.ietf.org/html/rfc3986 "check the specification")

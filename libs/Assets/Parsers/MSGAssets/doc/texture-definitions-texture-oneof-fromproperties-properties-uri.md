# URI Schema

```txt
texture.schema.json#/definitions/texture/oneOf/0/properties/uri
```

links to an object loaded from an external resource. To load from the externals list, use the "external" scheme then the external's name followed by the object name as a query, example: "external:ExternalName?ObjectName"

| Abstract            | Extensible | Status         | Identifiable            | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                                    |
| :------------------ | :--------- | :------------- | :---------------------- | :---------------- | :-------------------- | :------------------ | :---------------------------------------------------------------------------- |
| Can be instantiated | No         | Unknown status | Unknown identifiability | Forbidden         | Allowed               | none                | [texture.schema.json\*](json/defs/texture.schema.json "open original schema") |

## uri Type

`string` ([URI](texture-definitions-texture-oneof-fromproperties-properties-uri.md))

## uri Constraints

**URI**: the string must be a URI, according to [RFC 3986](https://tools.ietf.org/html/rfc3986 "check the specification")

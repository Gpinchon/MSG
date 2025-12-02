# Material Schema

```txt
material.schema.json#/definitions/material
```



| Abstract            | Extensible | Status         | Identifiable | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                                      |
| :------------------ | :--------- | :------------- | :----------- | :---------------- | :-------------------- | :------------------ | :------------------------------------------------------------------------------ |
| Can be instantiated | No         | Unknown status | No           | Forbidden         | Allowed               | none                | [material.schema.json\*](json/defs/material.schema.json "open original schema") |

## material Type

`object` ([Material](material-definitions-material.md))

# material Properties

| Property                                                    | Type     | Required | Nullable       | Defined by                                                                                                                                                            |
| :---------------------------------------------------------- | :------- | :------- | :------------- | :-------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| [uri](#uri)                                                 | `string` | Optional | cannot be null | [MSGAssets-material](external-definitions-uri.md "material.schema.json#/definitions/material/properties/uri")                                                         |
| [name](#name)                                               | `string` | Optional | cannot be null | [MSGAssets-material](material-definitions-material-properties-name.md "material.schema.json#/definitions/material/properties/name")                                   |
| [baseExtension](#baseextension)                             | `object` | Optional | cannot be null | [MSGAssets-material](material-definitions-materialbaseextension.md "material.schema.json#/definitions/material/properties/baseExtension")                             |
| [metallicRoughnessExtension](#metallicroughnessextension)   | `object` | Optional | cannot be null | [MSGAssets-material](material-definitions-materialmetallicroughnessextension.md "material.schema.json#/definitions/material/properties/metallicRoughnessExtension")   |
| [specularGlossinessExtension](#specularglossinessextension) | `object` | Optional | cannot be null | [MSGAssets-material](material-definitions-materialspecularglossinessextension.md "material.schema.json#/definitions/material/properties/specularGlossinessExtension") |

## uri

links to an object loaded from an external resource. To load from the externals list, use the "external" scheme then the external's name followed by the object name as a query

`uri`

* is optional

* Type: `string` ([URI](external-definitions-uri.md))

* cannot be null

* defined in: [MSGAssets-material](external-definitions-uri.md "material.schema.json#/definitions/material/properties/uri")

### uri Type

`string` ([URI](external-definitions-uri.md))

### uri Constraints

**URI**: the string must be a URI, according to [RFC 3986](https://tools.ietf.org/html/rfc3986 "check the specification")

### uri Examples

```json
"external:ExternalName?ObjectName"
```

```json
"file:path/to/file/filename.fileExtension"
```

## name

this material's name, required if uri is not present

`name`

* is optional

* Type: `string`

* cannot be null

* defined in: [MSGAssets-material](material-definitions-material-properties-name.md "material.schema.json#/definitions/material/properties/name")

### name Type

`string`

## baseExtension

this material's base extension

`baseExtension`

* is optional

* Type: `object` ([MaterialBaseExtension](material-definitions-materialbaseextension.md))

* cannot be null

* defined in: [MSGAssets-material](material-definitions-materialbaseextension.md "material.schema.json#/definitions/material/properties/baseExtension")

### baseExtension Type

`object` ([MaterialBaseExtension](material-definitions-materialbaseextension.md))

## metallicRoughnessExtension

this allows for the metallic roughness workflow

`metallicRoughnessExtension`

* is optional

* Type: `object` ([MaterialMetallicRoughnessExtension](material-definitions-materialmetallicroughnessextension.md))

* cannot be null

* defined in: [MSGAssets-material](material-definitions-materialmetallicroughnessextension.md "material.schema.json#/definitions/material/properties/metallicRoughnessExtension")

### metallicRoughnessExtension Type

`object` ([MaterialMetallicRoughnessExtension](material-definitions-materialmetallicroughnessextension.md))

## specularGlossinessExtension

this allows for the specular glossiness workflow, this extension takes priority over metallic roughness if both are present

`specularGlossinessExtension`

* is optional

* Type: `object` ([MaterialSpecularGlossinessExtension](material-definitions-materialspecularglossinessextension.md))

* cannot be null

* defined in: [MSGAssets-material](material-definitions-materialspecularglossinessextension.md "material.schema.json#/definitions/material/properties/specularGlossinessExtension")

### specularGlossinessExtension Type

`object` ([MaterialSpecularGlossinessExtension](material-definitions-materialspecularglossinessextension.md))

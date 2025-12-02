# FromProperties Schema

```txt
material.schema.json#/definitions/material/oneOf/0
```



| Abstract            | Extensible | Status         | Identifiable | Custom Properties | Additional Properties | Access Restrictions | Defined In                                                                      |
| :------------------ | :--------- | :------------- | :----------- | :---------------- | :-------------------- | :------------------ | :------------------------------------------------------------------------------ |
| Can be instantiated | No         | Unknown status | No           | Forbidden         | Allowed               | none                | [material.schema.json\*](json/defs/material.schema.json "open original schema") |

## 0 Type

unknown ([FromProperties](material-definitions-material-oneof-fromproperties.md))

# 0 Properties

| Property                                                    | Type     | Required | Nullable       | Defined by                                                                                                                                                                    |
| :---------------------------------------------------------- | :------- | :------- | :------------- | :---------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| [name](#name)                                               | `string` | Required | cannot be null | [MSGAssets-material](material-definitions-material-oneof-fromproperties-properties-name.md "material.schema.json#/definitions/material/oneOf/0/properties/name")              |
| [baseExtension](#baseextension)                             | `object` | Optional | cannot be null | [MSGAssets-material](material-definitions-materialbaseextension.md "material.schema.json#/definitions/material/oneOf/0/properties/baseExtension")                             |
| [metallicRoughnessExtension](#metallicroughnessextension)   | `object` | Optional | cannot be null | [MSGAssets-material](material-definitions-materialmetallicroughnessextension.md "material.schema.json#/definitions/material/oneOf/0/properties/metallicRoughnessExtension")   |
| [specularGlossinessExtension](#specularglossinessextension) | `object` | Optional | cannot be null | [MSGAssets-material](material-definitions-materialspecularglossinessextension.md "material.schema.json#/definitions/material/oneOf/0/properties/specularGlossinessExtension") |

## name

this material's name

`name`

* is required

* Type: `string`

* cannot be null

* defined in: [MSGAssets-material](material-definitions-material-oneof-fromproperties-properties-name.md "material.schema.json#/definitions/material/oneOf/0/properties/name")

### name Type

`string`

## baseExtension

this material's base extension

`baseExtension`

* is optional

* Type: `object` ([MaterialBaseExtension](material-definitions-materialbaseextension.md))

* cannot be null

* defined in: [MSGAssets-material](material-definitions-materialbaseextension.md "material.schema.json#/definitions/material/oneOf/0/properties/baseExtension")

### baseExtension Type

`object` ([MaterialBaseExtension](material-definitions-materialbaseextension.md))

## metallicRoughnessExtension

this allows for the metallic roughness workflow

`metallicRoughnessExtension`

* is optional

* Type: `object` ([MaterialMetallicRoughnessExtension](material-definitions-materialmetallicroughnessextension.md))

* cannot be null

* defined in: [MSGAssets-material](material-definitions-materialmetallicroughnessextension.md "material.schema.json#/definitions/material/oneOf/0/properties/metallicRoughnessExtension")

### metallicRoughnessExtension Type

`object` ([MaterialMetallicRoughnessExtension](material-definitions-materialmetallicroughnessextension.md))

## specularGlossinessExtension

this allows for the specular glossiness workflow, this extension takes priority over metallic roughness if both are present

`specularGlossinessExtension`

* is optional

* Type: `object` ([MaterialSpecularGlossinessExtension](material-definitions-materialspecularglossinessextension.md))

* cannot be null

* defined in: [MSGAssets-material](material-definitions-materialspecularglossinessextension.md "material.schema.json#/definitions/material/oneOf/0/properties/specularGlossinessExtension")

### specularGlossinessExtension Type

`object` ([MaterialSpecularGlossinessExtension](material-definitions-materialspecularglossinessextension.md))

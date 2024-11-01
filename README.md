<h1 align="center">
  <img src="https://user-images.githubusercontent.com/2679513/131189167-18ea5fe1-c578-47f6-9785-3748178e4312.png" width="150px"/><br/>
  Speckle | Unreal
</h1>

<p align="center"><a href="https://twitter.com/SpeckleSystems"><img src="https://img.shields.io/twitter/follow/SpeckleSystems?style=social" alt="Twitter Follow"></a> <a href="https://speckle.community"><img src="https://img.shields.io/discourse/users?server=https%3A%2F%2Fspeckle.community&amp;style=flat-square&amp;logo=discourse&amp;logoColor=white" alt="Community forum users"></a> <a href="https://speckle.systems"><img src="https://img.shields.io/badge/https://-speckle.systems-royalblue?style=flat-square" alt="website"></a> <a href="https://speckle.guide/dev/"><img src="https://img.shields.io/badge/docs-speckle.guide-orange?style=flat-square&amp;logo=read-the-docs&amp;logoColor=white" alt="docs"></a></p>

> Speckle is the first AEC data hub that connects with your favorite AEC tools. Speckle exists to overcome the challenges of working in a fragmented industry where communication, creative workflows, and the exchange of data are often hindered by siloed software and processes. It is here to make the industry better.

<h3 align="center">
    Speckle Connector for Unreal Engine
</h3>

> [!WARNING]
> This is a legacy repo! A new next generation connector will be coming soon. In the meantime, check out our active next generation repos here 👇<br/>
> [`speckle-sharp-connectors`](https://github.com/specklesystems/speckle-sharp-connectors): our .NET next generation connectors and desktop UI<br/>
> [`speckle-sharp-sdk`](https://github.com/specklesystems/speckle-sharp-sdk): our .NET SDK, Tests, and Objects

## Notice

We officially support Unreal Engine 4.27, 5.0-5.4

**Features**:
- [Receiving Speckle geometry as Actors in editor and standalone runtime](https://speckle.systems/tutorials/getting-started-with-speckle-for-unreal/).
- [Material override/substitiution](https://speckle.guide/user/unreal.html#material-converter).
- [Blueprint nodes for receiving and converting objects](https://speckle.guide/user/unreal.html#usage-blueprint).
- [Blueprint nodes for fetching stream/branch/commit/user details](https://speckle.systems/tutorials/unreal-engine-blueprint-nodes-fetch-stream-branch-commit-info-and-more/).
- [Ability to write/extend custom conversion functions in C++/BP](https://speckle.systems/tutorials/unreal-developing-custom-conversion-logic/).

**Supported Conversions**:

 Speckle Type |  | Native Type |
| ---: | :---: | :--- |
| [`Objects.Geometry.Mesh`](https://github.com/specklesystems/speckle-sharp/blob/main/Objects/Objects/Geometry/Mesh.cs) | → | [Static Mesh Component](https://docs.unrealengine.com/4.27/en-US/API/Runtime/Engine/Components/UStaticMeshComponent/) /<br/> [Procedural Mesh Component](https://docs.unrealengine.com/4.27/en-US/API/Plugins/ProceduralMeshComponent/UProceduralMeshComponent/) |
| [`Objects.Geometry.PointCloud`](https://github.com/specklesystems/speckle-sharp/blob/main/Objects/Objects/Geometry/Pointcloud.cs) | → | [LiDAR Point Cloud](https://docs.unrealengine.com/4.27/en-US/WorkingWithContent/LidarPointCloudPlugin/LidarPointCloudPluginReference/) |
| [`Objects.Other.Instance`](https://github.com/specklesystems/speckle-sharp/blob/main/Objects/Objects/Other/Instance.cs) | → | Actor with Transform |
| [`Objects.BuiltElements.View`](https://github.com/specklesystems/speckle-sharp/blob/main/Objects/Objects/BuiltElements/View.cs) | → | [Camera](https://docs.unrealengine.com/4.27/en-US/InteractiveExperiences/Framework/Camera/) |
| [`Speckle.Core.Models.Collection`](https://github.com/specklesystems/speckle-sharp/blob/main/Core/Core/Models/Collection.cs) | → | Empty Actor |

**Supported platforms**: Windows, Linux, MacOS
*Other platforms may work, but currently untested*.


**Limitations**:
- Currently no support for sending Unreal geometry to Speckle.
- Does not use the .NET or Python SDK, or the Speckle Kit based workflow, all code is in native C++/BP.
- Does not use our shared [Desktop UI 2](https://speckle.guide/user/ui2.html), only a simple editor UI is currently provided.
- Does not fetch [accounts from Speckle Manager](https://speckle.guide/user/manager.html#logging-in-adding-accounts), you must generate a [personal access token (auth token)](https://speckle.guide/dev/tokens.html#personal-access-tokens) to receive non-public streams.

## How to install

**Speckle for Unreal Engine** can be installed through the [Unreal Engine Marketplace](https://www.unrealengine.com/marketplace/en-US/product/speckle-for-unreal-engine).

---

Alternatively, developers may prefer to install manually.
1. Git clone [this repo](https://github.com/specklesystems/speckle-unreal) (or download and extract the [archive zip](https://github.com/specklesystems/speckle-unreal/archive/refs/heads/main.zip)) into your project's `Plugins` directory (created as needed)
2. Open/Restart your Unreal project. This will build the plugin for your environment.

See our [docs](https://speckle.guide/user/unreal.html) for usage instructions.

If you encounter build issues, try building your project from VS/Rider directly. Look at the `Saved\Logs\` files for error messages,
and don't hesitate to reach out on our [community forums](https://speckle.community) for help!

For contributing to any of our code repositories, please make sure you read the [contribution guidelines](https://github.com/specklesystems/speckle-sharp/blob/main/.github/CONTRIBUTING.md) for an overview of the best practices we try to follow.

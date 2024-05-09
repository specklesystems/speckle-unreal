# Speckle for Unreal Engine

[![Twitter Follow](https://img.shields.io/twitter/follow/SpeckleSystems?style=social)](https://twitter.com/SpeckleSystems) [![Community forum users](https://img.shields.io/discourse/users?server=https%3A%2F%2Fspeckle.community&style=flat-square&logo=discourse&logoColor=white)](https://speckle.community) [![website](https://img.shields.io/badge/https://-speckle.systems-royalblue?style=flat-square)](https://speckle.systems) [![docs](https://img.shields.io/badge/docs-speckle.guide-orange?style=flat-square&logo=read-the-docs&logoColor=white)](https://speckle.guide/user/unreal.html)

Speckle makes integrating Unreal Engine with your 3D workflows even easier. </br>
This plugin connects Unreal Engine to Speckle, allowing you to receive your versioned 3D data from Speckle inside Unreal Engine,
enabling interoperable collaboration between Unreal and other Speckle connectors (Revit, Rhino, Blender, Sketchup, Unity, AutoCAD, [and more!](https://speckle.systems/features/connectors/))


https://user-images.githubusercontent.com/45512892/187969471-3f548b17-3388-48ee-a07c-bd3a0ecf5149.mp4


Checkout our dedicated [Tutorials and Docs](https://speckle.systems/tag/unreal/).

If you are enjoying using Speckle, don't forget to ⭐ our [GitHub repositories](https://github.com/specklesystems),
and [join our community forum](https://speckle.community/) where you can post any questions, suggestions, and discuss exciting projects!

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

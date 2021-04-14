# speckle-unreal

[![Version](https://img.shields.io/badge/Version-v0.1.0-orange)](https://github.com/specklesystems/speckle-unreal) [![PRs Welcome](https://img.shields.io/badge/PRs-welcome-brightgreen)](http://makeapullrequest.com)

Plugin for Unreal Engine 4 to import objects from Speckle v2.


## NOTICE

* Tested on Windows, Unreal Engine v4.26 and Visual Studio Community 2019
* Only displays meshes. Breps are converted using their display values.
* Does not use the Speckle Kit workflow as conversions all happen in C++. 

## How To Install


1. Clone the repository or download it as a zip file.
2. Navigate to `SpeckleUnrealProject` > `Plugins` and copy the `SpeckleUnreal` folder
3. Paste the folder into your Unreal project under `YourUnrealProjectFolder` > `Plugins` (Create a `Plugins` folder if you don't already have one).
4. Reopen your project.

We will eventually look to distributing the plugin officially on the Unreal Engine Marketplace but for now you'll need to install the plugin manually like this.

## Credits
Based off the original Unreal integration for Speckle v1 by Mark and Jak which can be found here: [https://github.com/mobiusnode/SpeckleUnreal](https://github.com/mobiusnode/SpeckleUnreal).

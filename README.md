# SpeckleUnreal

[![Version](https://img.shields.io/badge/Version-v0.1.0-orange)](https://github.com/mobiusnode/SpeckleUnreal) [![PRs Welcome](https://img.shields.io/badge/PRs-welcome-brightgreen)](http://makeapullrequest.com)

Our Team is developing a Speckle plugin and interoperability transport schema for UE4. Our goal is to enable Revit/Dynamo and Rhino/Grasshopper to send + receive geometry to UE4 for visualization. Our current priority is to establish and release a data sender (TO UE4). We’re also working on receiver methods, however our initial focus  is on Rhino/Grasshopper to UE4 translation and the attachment of UE4-specific metadata to the core JSON ‘blobs’ in transport.

In this repository you will find the source code, assets and project settings of the SpeckleUnreal plugin for Unreal Engine app development (Unreal Engine 4.25.1 or newer recommended).

## NOTICE

* Tested on Windows and MacOS.
* Only displays meshes. Breps are converted using their display values.
* Does not use the Speckle Kit workflow as conversions all happen in C++. 

## How To Install


1. Clone the repository or download it as a zip file.
2. Navigate to `SpeckleUnrealProject` > `Plugins` and copy the `SpeckleUnreal` folder
3. Paste the folder into your Unreal project under `YourUnrealProjectFolder` > `Plugins` (Create a `Plugins` folder if you don't already have one).
4. Reopen your project.

We will eventually look to distributing the plugin officially on the Unreal Engine Marketplace but for now you'll need to install the plugin manually like this.

---

## Roadmap

> Roadmap is subject to change. Last reviewed 10th of July 2020.

| Version | Defining Feature                  						  				         |
| ------- | -------------------------------------------------------------------------------- |
| ~0.1~   | ~First prototype release as Unreal Engine plugin~   								 			         |
| 0.2   | New component workflow and custom materials assigned via inspector~		 |
| 0.3   | Spawn geometry in transform heirarchy based on layer data		    	         |
| 0.4   | User login API, get Stream API and no dependency on a local install of Speckle |
| 0.5   | Rendering Rule API                      |
| 0.6   | Support Lines, Points, Numbers and Text|
| 0.7     | Local caching of Speckle streams			                                     |
| 0.8     | Implement Sender API    			                                 |
| 1.0     | Production ready (out of preview)      			                                 |

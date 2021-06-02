# TODO

## Refurbish
- Yeet the levelloader system all the way to the depths where it belongs
- ~~Remove the script system/node editor~~
  - ~~It didn't live up to any expectations and didn't work all that well, better to remove it completely and build a new system from scratch~~
- ~~Make FiskJSON even remotely readable~~
- Remove or isolate Moonview Mountain from the engine
- Figure out preinstalled depencies/SDK's and list them on the main readme file
- Replace levelloader with something competent
  - Inengine editor?
- Flip all chiralities and axles to match the default in blender
- Switch from VS filters to folders
- ~~SetErrorTexture() move call to better place than modelloader, cause uhm wtf~~
- ~~Remove Skybox class, make it a assetType instead~~
- Rename All projects and folders to english

## Fixes
- Make a levelInstance with the metadata for levelloading (current index, current step etc) so the asset doesn't include any instance specific data and can be reused without issues 
- use fixed dt physx stepping in GameState::Update
- Don't set skybox in forward renderer every frame

## Improve
- Rework asset loading to allow easier asset loading
  - Massloading a folder
  - Asset packing into single Data file
  - Quickloading by packing *_DESC structs along with pre-parsed data
- Remove direct loads and asset handling with an asset manager and asset-handles
  - ~~Textures~~
  - ~~Models~~
  - ~~Shaders~~
  - ~~Cube Textures~~
  - ~~Animations~~
  - ~~Data/JSON~~
  - ~~Fonts~~
  - ~~Levels~~
  - ~~Navmeshes~~
  - Sounds
  - Sprites
- Centralize and standardize Entities so we can reliably select/view entity data
- Editor tools
  - Play/Pause
  - Freecam
- Add VR Support
  - using [OpenVR](https://github.com/ValveSoftware/openvr)
- Make tests for tools being used
- migrate to cmake instead of raw vs files
- ~~clean up matrix and vector operators to reduce copy operations~~
- Quickly and quitely hide and murder the pch hack
- Add animations back to modelviewer, maybe its own editor?
- Write a new script system + editor
- Update imgui version

# TODO

## Refurbish
- Yeet the levelloader system all the way to the depths where it belongs
- Remove the script system/node editor
  - It didn't live up to any expectations and didn't work all that well, better to remove it completely and build a new system from scratch
- ~~Make FiskJSON even remotely readable~~
- Remove or isolate Moonview Mountain from the engine
- Figure out preinstalled depencies/SDK's and list them on the main readme file
- Replace levelloader with something competent
  - Inengine editor?
- Flip all chiralities and axles to match the default in blender
- Switch from VS filters to folders

## Improve
- Rework asset loading to allow easier asset loading
  - Massloading a folder
  - Asset packing into single Data file
  - Quickloading by packing *_DESC structs along with pre-parsed data
- Remove direct loads and asset handling with an asset manager and asset-handles
- Centralize and standardize Entities so we can reliably select/view entity data
- Editor tools
  - Play/Pause
  - Freecam
- Add VR Support
  - using [OpenVR](https://github.com/ValveSoftware/openvr)
- Make tests for tools being used

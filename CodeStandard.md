# 1 Files
### 1.1 Cpp file extensions
Use only `.h` for header files and `.cpp` for code files
### 1.2 File names
Files should be named human readable and in PascalCase i.e `LockedResource.h`
### 1.3 Folder names
Folders should always be single word and all lowercase i.e `engine/graphics/`
# 2 Header guards
Use `#ifndef ... #endif` blocks as header guards
```cpp
#ifndef <key> 
#define <key>
...
#endif
```
The include guard key should be of the form `Folder/Project, Folders, Filename split into words, the letter H` joined by underscores and fully capitalized

So the file `engine/graphics/RenderStateManager.h` would have the key `ENGINE_GRAPHICS_RENDER_STATE_MANAGER_H`
# 3 Include
### 3.1 Never include relative
For headers that are a part of the project always include with the full path from the source folder
```cpp
#include "engine/graphics/..."
```
### 3.2 Include order
Files are generally included in order of most specific to least specific
#### 3.2.1 Block
Group includes into blocks where possible and separate with a one empty row
#### 3.2.2 Sort
Sort all includes in a block aphabetically
#### 3.2.3 Include self
Always include a `.cpp`'s corresponding `.h` at the very top of a file in its own include block
#### 3.2.4 Include project files
Split up all project includes into blocks by the last folder in their path

That is: 
`engine/graphics/Camera.h`
`engine/GameEngine.h`
`tools/MathVector.h`
`engine/SettingsManager.h`
`engine/graphics/Texture.h`
`tools/Event.h`
`engine/graphics/GBuffer.h`
`tools/Literals.h`

Should be arranged into blocks like this:
```cpp
#include "engine/graphics/Camera.h"
#include "engine/graphics/GBuffer.h"
#include "engine/graphics/Texture.h"

#include "engine/GameEngine.h"
#include "engine/SettingsManager.h"

#include "tools/Event.h"
#include "tools/Literals.h"
#include "tools/MathVector.h"
```
#### 3.2.4 External files
Always include external files with `<>`

The two final include blocks are `std` files and `platform` files

```cpp
#include <string>
#include <typeinfo>
#include <vector>

#include <d3d11.h>
```
##### 3.2.4.1 Windows
Always define `NOMINMAX` right before including `Windows.h`
```cpp
#define NOMINMAX
#include <Windows.h>
```

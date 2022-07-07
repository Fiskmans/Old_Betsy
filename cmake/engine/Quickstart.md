### Core Start flow
* Create a class that inherits from `engine::Game` 
* Make an instance of it
* Initialize the game engine with it
* Run the game
```cpp
//YourGame.h
class YourGame : public engine::Game
{
  ...
}
```
```cpp
//main.cpp
int main()
{
  YourGame yourGame;

  engine::GameEngine::GetInstance().Init(yourGame);
  engine::GameEngine::GetInstance().RunGame();
}
```


### Logger
You may want to setup the logger before anything else though, otherwise the game will halt on any message
```cpp
int main()
{
  logger::SetFilter(logger::Type::All & ~logger::Type::AnyVerbose);        // Set the logger filter to accept any message that isn't verbose
  logger::SetHalting(logger::Type::AnyError | logger::Type::SystemCrash);  // Halt execution on any message that is an error or an crash

  logger::SetColor(logger::Type::AnyError,        FOREGROUND_RED |                                      FOREGROUND_INTENSITY); // Color all errors   RED
  logger::SetColor(logger::Type::AnyWarning,      FOREGROUND_RED | FOREGROUND_GREEN |                   FOREGROUND_INTENSITY); // Color all warnings YELLOW
  logger::SetColor(logger::Type::AnyInfo,         FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY); // Color all info     WHITE
  logger::SetColor(logger::Type::SystemNetwork,                                       FOREGROUND_BLUE | FOREGROUND_INTENSITY); // Color all network  BLUE  
  ...
}
```

### Name thread
It's generally a good idea to name the main thread as soon as possible to ease debugging
```cpp
int main()
{
  tools::NameThread("main_thread");
  ...
}
```

### Log library versions
#### It's recomended to log the versions of all libraries

I tried so hard to make this automagically collect all versions of any linked in library, but alas you'll have to do it manually.

Because the construction of globals can be deferred until just before the first peice of code from that translation unit is executed (Even if it has side effects that extend beyond the translation unit... like a global registration) the best that could be done is to add a registration function to all the libraries but you'd still have to manually call all of them. At that point you might aswell just structure the code nice and call it a day instead.
```cpp
int main()
{
  ...
  tools::SubsystemManager::GetInstance().AddSubsystemVersion("engine",   VERSIONSTRING(ENGINE));
  tools::SubsystemManager::GetInstance().AddSubsystemVersion("imgui",    VERSIONSTRING(IMGUI));
  tools::SubsystemManager::GetInstance().AddSubsystemVersion("launcher", VERSIONSTRING(LAUNCHER));
  tools::SubsystemManager::GetInstance().AddSubsystemVersion("logger",   VERSIONSTRING(LOGGER));
  tools::SubsystemManager::GetInstance().AddSubsystemVersion("tools",    VERSIONSTRING(TOOLS));

  tools::SubsystemManager::GetInstance().LogAllVersions();
  ...
}
```

### Finally
After the game shuts down you'll want to shut down the logger to make sure all the log files get flushed properly
```cpp
int main()
{
  ...
  logger::Shutdown();
}
```

### Result
Your main file should look something like this in the end
```cpp
// YourGame.h
class YourGame : public engine::Game
{
  ...
}
```

```cpp
//main.cpp
#include "YourGame.h"

int main()
{
  tools::NameThread("main_thread");

  logger::SetFilter(logger::Type::All & ~logger::Type::AnyVerbose);
  logger::SetHalting(logger::Type::AnyError | logger::Type::SystemCrash);

  logger::SetColor(logger::Type::AnyError,        FOREGROUND_RED |                                      FOREGROUND_INTENSITY);
  logger::SetColor(logger::Type::AnyWarning,      FOREGROUND_RED | FOREGROUND_GREEN |                   FOREGROUND_INTENSITY);
  logger::SetColor(logger::Type::AnyInfo,         FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
  logger::SetColor(logger::Type::SystemNetwork,                                       FOREGROUND_BLUE | FOREGROUND_INTENSITY);

  tools::SubsystemManager::GetInstance().AddSubsystemVersion("engine",   VERSIONSTRING(ENGINE));
  tools::SubsystemManager::GetInstance().AddSubsystemVersion("imgui",    VERSIONSTRING(IMGUI));
  tools::SubsystemManager::GetInstance().AddSubsystemVersion("launcher", VERSIONSTRING(LAUNCHER));
  tools::SubsystemManager::GetInstance().AddSubsystemVersion("logger",   VERSIONSTRING(LOGGER));
  tools::SubsystemManager::GetInstance().AddSubsystemVersion("tools",    VERSIONSTRING(TOOLS));

  tools::SubsystemManager::GetInstance().LogAllVersions();


  YourGame yourGame;

  engine::GameEngine::GetInstance().Init(game);
  engine::GameEngine::GetInstance().RunGame();

  logger::Shutdown();
}
```
But i can't stop you from doing what you want with it, go crazy, add some ascii art or something, make it your own

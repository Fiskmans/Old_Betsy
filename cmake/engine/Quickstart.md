### The core start loop looks something like this
```cpp
class YourGame : public engine::Game
{
  ...
}

int main()
{
  YourGame yourGame;

  engine::GameEngine::GetInstance().Init(yourGame);
  engine::GameEngine::GetInstance().RunGame();
}
```


### You may want to setup the logger before anything else though, otherwise the game will halt on any message
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

### It's generally a good idea to name the main thread as soon as possible to ease debugging
```cpp
int main()
{
  tools::NameThread("main_thread");
  ...
}
```

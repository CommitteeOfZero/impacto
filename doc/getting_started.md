# Getting started

## Game profiles

To handle the complexity of the full range of MAGES. engine titles (originally developed on different engine branches with many per-game hardcoded customizations) within a single codebase and binary, impacto uses a data-driven, modular design.

On application load, a set of scripts we call *profile* is executed, generating a (declarative) configuration specifying the chosen game's featureset (such as implemented VM instructions or the 3D scene in Robotics;Notes), parameters (like dimensions, names etc. ), asset location/use and "game objects" (menus/UI, database of 3D models in Robotics;Notes, built from primitives). `/profiles/` contains these scripts. `/src/profile/` has the host code to execute them and manually deserialize the resulting configuration into global state.

## Configuration Files

**`baseconfig.lua`** defines available games and their profile locations. For example:
```lua
root.BaseConfig.GameDefinitions = {
  chlcc = {
    GameProfile = root.BaseConfig.RootProfilesDir .. "/chlcc/game.lua",
    Patch = {
      English = root.BaseConfig.RootPatchesDir .. "/english/profiles/chlcc/patch.lua",
    }
  },
  -- ... other games
};
```

**`userconfig.lua`** contains runtime settings including the active game. Set `ActiveGame` to specify which game to launch:
```lua
root.UserConfig = {
  GameSettings = {
    chlcc = { Language = "Japanese", UsePatch = false },
    cclcc = { Language = "English", UsePatch = true},
    -- ... settings for other games
  },
  ActiveGame = "cclcc"
};
```

In a release environment, these files will be placed in a platform specific preferences directory. 

For development purposes it will fallback to looking for these files in the same directory as the impacto executable.

## Preparing files
The following directories must be in the same directory as the impacto executable:

* games - directory that contains game specific Impacto asset files
* shaders - directory that contains shader files

The following directories must be specified in `baseconfig.lua` and populated:
* gamedata - directory containing game resource files, nested under their respective profile names
* profiles - directory that contains profile definition files
* patches - optional directory containing overlay resources and profile files to patch over existing game profile

These folders can be found in the repository root. When using the *install* build step they are automatically copied to your specified install location.

For the list of required game resource files refer to the `vfs.lua` file located in desired game profile directory. The resource files should be placed in `/gamedata/<profile_name>/` directory.

## Launching a Game

You can specify the game to launch using any of these methods (in priority order):

1. **Command-line argument** (Useful for development and debugging):
   ```
   impacto -g chlcc
   impacto --game cclcc
   ```

2. **Set `ActiveGame` in `userconfig.lua`**:
   Uncomment the line `ActiveGame = "cclcc"` to set the default game for your development session.

3. **Fallback: `args.txt`** (for platforms without command-line support):
   Create a file named `args.txt` next to the executable with the game name as content. This is useful for platforms without arguments support.

## Command-line Arguments

Useful flags for development and debugging, these settings will take precedent over settings specified in lua when applicable:

```
impacto -g chlcc                           # Specify active game/profile
impacto -p                                 # Enables patch override
impacto -l English                         # Specify language override
impacto -ll Debug                          # Set log level (Fatal, Warning, Info, Debug)
impacto -lc General                        # Enable specific log channel (repeat flag to specify multiple)
impacto -lf log.txt                        # Specify Log File Path
impacto -bc ./custom_base_config.lua       # Use custom base config path
impacto -uc ./custom_user_config.lua       # Use custom user config path
```
Available Log Channels:
- None
- General
- IO
- Render
- ModelLoad
- GL
- Renderable3D
- TextureLoad
- Scene
- VM
- Expr
- VMStub
- Audio
- Profile
- Video
- All

Available Log Levels:
- Off
- Fatal
- Error
- Warning
- Info
- Debug
- Trace
- Max

Example combining multiple arguments:
```
impacto.exe -g chlcc -ll Debug -lc General -lc Vm
```

The following profiles are currently available:

* **chlcc** - Chaos;Head Love Chu Chu
* **cclcc** - Chaos;Child Love Chu Chu
* **darling** - Steins;Gate My Darling's Embrace
* **dash** - Robotics;Notes DaSH
* **mo6tw** - Memories Off 6 ~T-Wave~
* **rne** - Robotics;Notes Elite (PS Vita)
* **characterviewer** - A viewer for 2D characters
* **modelviewer** - A viewer for 3D models from Robotics;Notes Elite
* **modelviewer-dash** - A viewer for 3D models from Robotics;Notes DaSH

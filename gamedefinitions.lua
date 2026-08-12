root.GameDefinitions = {
  chlcc = {
    GameProfile = root.BasePaths.RootProfilesDir .. "/chlcc/game.lua",
    Patch = {
      English = root.BasePaths.RootPatchesDir .. "/english/profiles/chlcc/patch.lua",
    }
  },
  cclcc = {
    GameProfile = root.BasePaths.RootProfilesDir .. "/cclcc/game.lua",
    Patch = {
      English = root.BasePaths.RootPatchesDir .. "/english/profiles/cclcc/patch.lua",
    }
  },
  ["cclcc-switch"] = {
      GameProfile = root.BasePaths.RootProfilesDir .. "/cclcc-switch/game.lua",
    },
  mo6tw = {
    Hidden = true,
    GameProfile = root.BasePaths.RootProfilesDir .. "/mo6tw/game.lua",
  },
  darling = {
    Hidden = true,
    GameProfile = root.BasePaths.RootProfilesDir .. "/darling/game.lua",
  },
  dash = {
    Hidden = true,
    GameProfile = root.BasePaths.RootProfilesDir .. "/dash/game.lua",
  },
  rne = {
    Hidden = true,
    GameProfile = root.BasePaths.RootProfilesDir .. "/rne/game.lua",
  },
  characterviewer = {
    Hidden = true,
    GameProfile = root.BasePaths.RootProfilesDir .. "/characterviewer/game.lua",
  },
  modelviewer = {
    Hidden = true,
    GameProfile = root.BasePaths.RootProfilesDir .. "/modelviewer/game.lua",
  },
  ["modelviewer-dash"] = {
    Hidden = true,
    GameProfile = root.BasePaths.RootProfilesDir .. "/modelviewer-dash/game.lua",
  },
};
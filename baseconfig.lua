-- @: Directory of impacto executable
-- 
root.BaseConfig = {
  RootGamedataDir = "./gamedata",
  RootProfilesDir = "./profiles",
  RootPatchesDir = "./patches",
  RootSavesDir = "./saves",  
};

root.BaseConfig.GameDefinitions = {
  chlcc = {
    GameProfile = root.BaseConfig.RootProfilesDir .. "/chlcc/game.lua",
    Patch = {
      English = root.BaseConfig.RootPatchesDir .. "/english/profiles/chlcc/patch.lua",
    }
  },
  cclcc = {
    GameProfile = root.BaseConfig.RootProfilesDir .. "/cclcc/game.lua",
    Patch = {
      English = root.BaseConfig.RootPatchesDir .. "/english/profiles/cclcc/patch.lua",
    }
  },
  ["cclcc-switch"] = {
    GameProfile = root.BaseConfig.RootProfilesDir .. "/cclcc-switch/game.lua",
  },
  mo6tw = {
    GameProfile = root.BaseConfig.RootProfilesDir .. "/profiles/mo6tw/game.lua",
  },
  darling = {
      GameProfile = root.BaseConfig.RootProfilesDir .. "/profiles/darling/game.lua",
  },
  dash = {
      GameProfile = root.BaseConfig.RootProfilesDir .. "/profiles/dash/game.lua",
  },
  rne = {
      GameProfile = root.BaseConfig.RootProfilesDir .. "/profiles/rne/game.lua",
  },
  characterviewer = {
      GameProfile = root.BaseConfig.RootProfilesDir .. "/profiles/characterviewer/game.lua",
  },
  modelviewer = {
      GameProfile = root.BaseConfig.RootProfilesDir .. "/profiles/modelviewer/game.lua",
  },
  ["modelviewer-dash"] = {
      GameProfile = root.BaseConfig.RootProfilesDir .. "/profiles/modelviewer-dash/game.lua",
  },
};
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
  mo6tw = {
    GameProfile = root.BaseConfig.RootProfilesDir .. "/profiles/mo6tw/game.lua",
    Patch = {
      English = root.BaseConfig.RootPatchesDir .. "/english/profiles/mo6tw/patch.lua",
    }
  }
};
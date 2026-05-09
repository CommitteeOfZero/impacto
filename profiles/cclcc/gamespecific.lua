root.GameSpecific = {
  Type = GameSpecificType.CCLCC,
  UIButtonGuideSprites = {},
  UIButtonGuideEndDisp = {
    X = 0, Y = 988, Width = 60, Height = 54
  }
}

include(root.BaseConfig.RootProfilesDir .. '/cclcc/mapsystem.lua');
include(root.BaseConfig.RootProfilesDir .. '/cclcc/yesnotrigger.lua');
include(root.BaseConfig.RootProfilesDir .. '/cclcc/hud/delusiontrigger.lua');

for i = 0, 1 do
    local index = i + 1;
    root.Sprites["UIButtonGuide" .. i] = {
        Sheet = "Data",
        Bounds = { X = 0, Y = 1121 + 56 * i, Width = 1920, Height = 54 },
    };
    root.GameSpecific.UIButtonGuideSprites[index] = "UIButtonGuide" .. i;
end
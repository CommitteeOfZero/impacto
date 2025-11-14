root.DelusionTrigger = {
  BackgroundSprite = "BackgroundSprite",
  BackgroundSpriteMask = "BackgroundSpriteMask",
  ScreenMask = "ScreenMask",

  DelusionTextGlyphs = {},
};

root.Sprites["BackgroundSprite"] = {
  Sheet = "Menu",
  Bounds = { X = 0, Y = 0, Width = 2048, Height = 1024 }
}
root.Sprites["ScreenMask"] = {
  Sheet = "DelusionUnderlayer",
  Bounds = { X = 0, Y = 0, Width = 1600, Height = 720 }
}
root.Sprites["BackgroundSpriteMask"] = {
  Sheet = "DelusionMask",
  Bounds = { X = 0, Y = 0, Width = 1024, Height = 1024 }
}

local textCharacterCount = {
  13,	13,	12,	17, 16,	17,	13,	13,
  14,	19,	16,	16, 16,	12,	10,	15,
  12,	20,	15,	14, 15,
}

for i = 0, 21 - 1 do
  local currentLine = {}
  for j = 0, textCharacterCount[i + 1] - 1 do
      local name = "DelusionTextGlyph" .. i .. "_" .. j
      root.Sprites[name] = {
          Sheet = "DelusionText",
          Bounds = {
              X = j * 64,
              Y = i * 64,
              Width = 64,
              Height = 64
          },
      };
      currentLine[j + 1] = name;
  end
  root.DelusionTrigger.DelusionTextGlyphs[i + 1] = currentLine
end

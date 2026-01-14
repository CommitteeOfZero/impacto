root.GameSpecific = {
  Type = GameSpecificType.CHLCC,

  MonitorScanline = "MonitorScanline",

  ButterflySprites = {},
  ButterflyFlapFrameDuration = 4/60,
  ButterflyFrameCount = 8,
  ButterflyFadeDuration = 64 / 60,

  BubbleSpriteSmall = "BubbleSpriteSmall",
  BubbleSpriteBig = "BubbleSpriteBig",
  BubbleFadeDuration = 64/60,

  EyecatchStar = "EyecatchStar",

  MenuSelectPromptDuration =  110 / 60,
  MenuSelectPromptInterval = 5 / 60,
  MenuTransitionDuration = 64 / 60,
  ShowPageAnimationStartTime = 16 / 60,
  ShowPageAnimationDuration = (48 - 16) / 60,
};

root.Sprites["MonitorScanline"] = {
  Sheet = "Data",
  Bounds = { X = 1, Y = 369, Width = 640, Height = 360 },
};

root.Sprites["EyecatchStar"] = {
  Sheet = "Data",
  Bounds = { X = 1, Y = 917, Width = 106, Height = 106 },
};

for i = 0, 5 do
  local name = "ButterflySprites" .. i;
  root.Sprites[name] = {
      Sheet = "Data",
      Bounds = {
          X = i * 100 + 1,
          Y = 817,
          Width = 98,
          Height = 98
      },
  };
  root.GameSpecific.ButterflySprites[i + 1] = name
end

root.Sprites["BubbleSpriteSmall"] = {
    Sheet = "Data",
    Bounds = {
        X = 129,
        Y = 981,
        Width = 22,
        Height = 22
    },
};

root.Sprites["BubbleSpriteBig"] = {
    Sheet = "Data",
    Bounds = {
        X = 179,
        Y = 967,
        Width = 50,
        Height = 50
    },
};

include('chlcc/hud/delusiontrigger.lua');
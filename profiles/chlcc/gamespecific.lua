root.GameSpecific = {
  Type = GameSpecificType.CHLCC,

  MonitorScanline = "MonitorScanline",

  ButterflySprites = {},
  ButterflyFlapFrameDuration = 4/60,
  ButterflyFrameCount = 8,
  ButterflyFadeDuration = 64 / 60,

  EyecatchStar = "EyecatchStar",
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

include('chlcc/hud/delusiontrigger.lua');
root.DelusionTrigger = {
  BackgroundSprite = "BackgroundSprite",
  BackgroundSpriteMask = "BackgroundSpriteMask",
  ScreenMask = "ScreenMask",
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
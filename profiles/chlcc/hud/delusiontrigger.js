root.DelusionTrigger = {
  DrawType: DrawComponentType.SystemMenu,
  Type: DelusionTriggerType.CHLCC,
  BackgroundSprite: "BackgroundSprite",
  BackgroundSpriteMask: "BackgroundSpriteMask",
  ScreenMask: "ScreenMask",
  FadeInDuration: 4,
  FadeOutDuration: 4,
  TextFadeInDuration: 40 / 60,
  TextFadeOutDuration: 28 / 60
};

root.Sprites["BackgroundSprite"] = {
  Sheet: "Menu",
  Bounds: { X: 0, Y: 0, Width: 2048, Height: 1024 }
}
root.Sprites["ScreenMask"] = {
  Sheet: "DelusionUnderlayer",
  Bounds: { X: 0, Y: 0, Width: 2048, Height: 1024 }
}
root.Sprites["BackgroundSpriteMask"] = {
  Sheet: "DelusionMask",
  Bounds: { X: 0, Y: 0, Width: 1024, Height: 1024 }
}
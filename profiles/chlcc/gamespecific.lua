root.GameSpecific = {
  Type = GameSpecificType.CHLCC,
  MonitorScanline = "MonitorScanline",
}

root.Sprites["MonitorScanline"] = {
  Sheet = "Data",
  Bounds = { X = 1, Y = 369, Width = 640, Height = 360 },
};

include('chlcc/hud/delusiontrigger.lua');
root.OptionsMenu = {
    Type = OptionsMenuType.MO8,
    DrawType = DrawComponentType.SystemMenu,
    BackgroundSprite = "OptionsBackground",
    SliderTrackSprite = "OptionsSliderTrack",
    SliderFillSprite = "OptionsSliderFill",
    SliderThumbSprite = "OptionsSliderThumb",
    NextButtonSprite = "OptionsNextButton",
    NextButtonHighlightedSprite = "OptionsNextButtonHighlighted",
    NextButtonPosition = { X = 1586, Y = 972 },
    BackButtonSprite = "OptionsBackButton",
    BackButtonHighlightedSprite = "OptionsBackButtonHighlighted",
    BackButtonPosition = { X = 30, Y = 972 },
    BackButtonPositionVoice = { X = 808, Y = 972 },
    FadeInDuration = 0.2,
    FadeOutDuration = 0.2
};

root.Sprites["OptionsBackground"] = {
    Sheet = "Options",
    Bounds = { X = 0, Y = 0, Width = 1920, Height = 1080 }
};

root.Sprites["OptionsSliderTrack"] = {
    Sheet = "Options",
    Bounds = { X = 1, Y = 921, Width = 480, Height = 8 }
};

root.Sprites["OptionsSliderFill"] = {
    Sheet = "Options",
    Bounds = { X = 1, Y = 931, Width = 480, Height = 8 }
};

root.Sprites["OptionsSliderThumb"] = {
    Sheet = "Options",
    Bounds = { X = 963, Y = 807, Width = 42, Height = 42 }
};

root.Sprites["OptionsBackButton"] = {
    Sheet = "Data",
    Bounds = { X = 0, Y = 722, Width = 304, Height = 80 }
};

root.Sprites["OptionsBackButtonHighlighted"] = {
    Sheet = "Data",
    Bounds = { X = 0, Y = 898, Width = 304, Height = 80 }
};

root.Sprites["OptionsNextButton"] = {
    Sheet = "Data",
    Bounds = { X = 0, Y = 810, Width = 304, Height = 80 }
};

root.Sprites["OptionsNextButtonHighlighted"] = {
    Sheet = "Data",
    Bounds = { X = 0, Y = 634, Width = 304, Height = 80 }
};

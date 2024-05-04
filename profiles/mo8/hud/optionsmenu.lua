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
    ButtonHighlight = "OptionsButtonHighlight",
    PageLabelPosition = { X = 336, Y = 62 },
    ListStartingPosition = { X = 66, Y = 164 },
    ListPadding = { X = 0, Y = 152 },
    OptionGroupItemsOffset = { X = 538, Y = 10 },
    OptionGroupSliderOffset = { X = 777, Y = 16 },

    TextPageLabel = "OptionsTextPageLabel",
    TextSpeedOptionsLabel = "OptionsTextSpeedLabel",
    TextSpeedOptionsLabelH = "OptionsTextSpeedLabelH",
    TextSpeedOptionsNum = 4,
    TextSpeedOptionsSprites = {},
    TextSpeedOptionsHSprites = {},

    AutoModeOptionsLabel = "OptionsAutoModeLabel",
    AutoModeOptionsLabelH = "OptionsAutoModeLabelH",
    AutoModeOptionsNum = 3,
    AutoModeOptionsSprites = {},
    AutoModeOptionsHSprites = {},

    SkipModeOptionsLabel = "OptionsSkipModeLabel",
    SkipModeOptionsLabelH = "OptionsSkipModeLabelH",
    SkipModeOptionsNum = 2,
    SkipModeOptionsSprites = {},
    SkipModeOptionsHSprites = {},

    SoundModeOptionsNum = 2,
    SoundPageLabel = "OptionsSoundPageLabel",

    VoiceSyncOptionsLabel = "OptionsVoiceSyncLabel",
    VoiceSyncOptionsLabelH = "OptionsVoiceSyncLabelH",

    VoiceSkipOptionsLabel = "OptionsVoiceSkipLabel",
    VoiceSkipOptionsLabelH = "OptionsVoiceSkipLabelH",

    VoiceHighlightOptionsLabel = "OptionsVoiceHighlightLabel",
    VoiceHighlightOptionsLabelH = "OptionsVoiceHighlightLabelH",

    SoundModeOptionsSprites = {},
    SoundModeOptionsHSprites = {},

    BgmVolumeLabel = "OptionsBgmVolumeLabel",
    BgmVolumeLabelH = "OptionsBgmVolumeLabelH",
    VoiceVolumeLabel = "OptionsVoiceVolumeLabel",
    VoiceVolumeLabelH = "OptionsVoiceVolumeLabelH",
    SeVolumeLabel = "OptionsSeVolumeLabel",
    SeVolumeLabelH = "OptionsSeVolumeLabelH",
    SystemSeVolumeLabel = "OptionsSystemSeVolumeLabel",
    SystemSeVolumeLabelH = "OptionsSystemSeVolumeLabelH",
    CharacterVoiceVolumeLabel = "OptionsCharacterVoiceVolumeLabel",
    CharacterVoiceVolumeLabelH = "OptionsCharacterVoiceVolumeLabelH",

    OtherPageLabel = "OptionsOtherPageLabel",

    QuickSaveOptionsLabel = "OptionsQuickSaveOptionsLabel",
    QuickSaveOptionsLabelH = "OptionsQuickSaveOptionsLabelH",
    QuickSaveOptionsNum = 3,
    QuickSaveOptionsSprites = {},
    QuickSaveOptionsHSprites = {},

    FadeInDuration = 0.33,
    FadeOutDuration = 0.33
};

root.Sprites["OptionsBackground"] = {
    Sheet = "Options",
    Bounds = { X = 0, Y = 0, Width = 1920, Height = 1080 }
};

root.Sprites["OptionsSliderTrack"] = {
    Sheet = "Options",
    Bounds = { X = 61, Y = 4004, Width = 870, Height = 68 }
};

root.Sprites["OptionsSliderFill"] = {
    Sheet = "Options",
    Bounds = { X = 1061, Y = 4004, Width = 870, Height = 68 }
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

root.Sprites["OptionsButtonHighlight"] = {
    Sheet = "Options",
    Bounds = { X = 2406, Y = 3256, Width = 304, Height = 80 }
};

root.Sprites["OptionsTextPageLabel"] = {
    Sheet = "Options",
    Bounds = { X = 1928, Y = 1008, Width = 192, Height = 42 }
};

root.Sprites["OptionsSoundPageLabel"] = {
    Sheet = "Options",
    Bounds = { X = 2128, Y = 1008, Width = 192, Height = 42 }
};

root.Sprites["OptionsOtherPageLabel"] = {
    Sheet = "Options",
    Bounds = { X = 2328, Y = 1008, Width = 192, Height = 42 }
};

-- Text speed

root.Sprites["OptionsTextSpeedLabel"] = {
    Sheet = "Options",
    Bounds = { X = 0, Y = 1628, Width = 1786, Height = 100 }
};
root.Sprites["OptionsTextSpeedLabelH"] = {
    Sheet = "Options",
    Bounds = { X = 0, Y = 1520, Width = 1786, Height = 100 }
};

root.Sprites["OptionsTextSpeedImmediate"] = {
    Sheet = "Options",
    Bounds = {
        X = 2718,
        Y = 3256,
        Width = 304,
        Height = 80
    }
};
root.OptionsMenu.TextSpeedOptionsSprites[#root.OptionsMenu.TextSpeedOptionsSprites + 1] = "OptionsTextSpeedImmediate";
root.Sprites["OptionsTextSpeedImmediateH"] = {
    Sheet = "Options",
    Bounds = {
        X = 2540,
        Y = 528,
        Width = 304,
        Height = 80
    }
};
root.OptionsMenu.TextSpeedOptionsHSprites[#root.OptionsMenu.TextSpeedOptionsHSprites + 1] = "OptionsTextSpeedImmediateH";

root.Sprites["OptionsTextSpeedSlow"] = {
    Sheet = "Options",
    Bounds = {
        X = 2406,
        Y = 2992,
        Width = 304,
        Height = 80
    }
};
root.OptionsMenu.TextSpeedOptionsSprites[#root.OptionsMenu.TextSpeedOptionsSprites + 1] = "OptionsTextSpeedSlow";
root.Sprites["OptionsTextSpeedSlowH"] = {
    Sheet = "Options",
    Bounds = {
        X = 2406,
        Y = 2904,
        Width = 304,
        Height = 80
    }
};
root.OptionsMenu.TextSpeedOptionsHSprites[#root.OptionsMenu.TextSpeedOptionsHSprites + 1] = "OptionsTextSpeedSlowH";

root.Sprites["OptionsTextSpeedMedium"] = {
    Sheet = "Options",
    Bounds = {
        X = 2406,
        Y = 3080,
        Width = 304,
        Height = 80
    }
};
root.OptionsMenu.TextSpeedOptionsSprites[#root.OptionsMenu.TextSpeedOptionsSprites + 1] = "OptionsTextSpeedMedium";
root.Sprites["OptionsTextSpeedMediumH"] = {
    Sheet = "Options",
    Bounds = {
        X = 2540,
        Y = 616,
        Width = 304,
        Height = 80
    }
};
root.OptionsMenu.TextSpeedOptionsHSprites[#root.OptionsMenu.TextSpeedOptionsHSprites + 1] = "OptionsTextSpeedMediumH";

root.Sprites["OptionsTextSpeedFast"] = {
    Sheet = "Options",
    Bounds = {
        X = 2540,
        Y = 704,
        Width = 304,
        Height = 80
    }
};
root.OptionsMenu.TextSpeedOptionsSprites[#root.OptionsMenu.TextSpeedOptionsSprites + 1] = "OptionsTextSpeedFast";
root.Sprites["OptionsTextSpeedFastH"] = {
    Sheet = "Options",
    Bounds = {
        X = 2406,
        Y = 3168,
        Width = 304,
        Height = 80
    }
};
root.OptionsMenu.TextSpeedOptionsHSprites[#root.OptionsMenu.TextSpeedOptionsHSprites + 1] = "OptionsTextSpeedFastH";

-- Auto mode

root.Sprites["OptionsAutoModeLabel"] = {
    Sheet = "Options",
    Bounds = { X = 0, Y = 1844, Width = 1786, Height = 100 }
};
root.Sprites["OptionsAutoModeLabelH"] = {
    Sheet = "Options",
    Bounds = { X = 0, Y = 1736, Width = 1786, Height = 100 }
};

root.Sprites["OptionsAutoModeLong"] = {
    Sheet = "Options",
    Bounds = {
        X = 2718,
        Y = 2904,
        Width = 304,
        Height = 80
    }
};
root.OptionsMenu.AutoModeOptionsSprites[#root.OptionsMenu.AutoModeOptionsSprites + 1] = "OptionsAutoModeLong";
root.Sprites["OptionsAutoModeLongH"] = {
    Sheet = "Options",
    Bounds = {
        X = 2718,
        Y = 2992,
        Width = 304,
        Height = 80
    }
};
root.OptionsMenu.AutoModeOptionsHSprites[#root.OptionsMenu.AutoModeOptionsHSprites + 1] = "OptionsAutoModeLongH";

root.OptionsMenu.AutoModeOptionsSprites[#root.OptionsMenu.AutoModeOptionsSprites + 1] = "OptionsTextSpeedMedium";
root.OptionsMenu.AutoModeOptionsHSprites[#root.OptionsMenu.AutoModeOptionsHSprites + 1] = "OptionsTextSpeedMediumH";

root.Sprites["OptionsAutoModeFast"] = {
    Sheet = "Options",
    Bounds = {
        X = 2718,
        Y = 3080,
        Width = 304,
        Height = 80
    }
};
root.OptionsMenu.AutoModeOptionsSprites[#root.OptionsMenu.AutoModeOptionsSprites + 1] = "OptionsAutoModeFast";
root.Sprites["OptionsAutoModeFastH"] = {
    Sheet = "Options",
    Bounds = {
        X = 2718,
        Y = 3168,
        Width = 304,
        Height = 80
    }
};
root.OptionsMenu.AutoModeOptionsHSprites[#root.OptionsMenu.AutoModeOptionsHSprites + 1] = "OptionsAutoModeFastH";

-- Skip mode

root.Sprites["OptionsSkipModeLabel"] = {
    Sheet = "Options",
    Bounds = { X = 1794, Y = 1520, Width = 1786, Height = 100 }
};
root.Sprites["OptionsSkipModeLabelH"] = {
    Sheet = "Options",
    Bounds = { X = 0, Y = 1952, Width = 1786, Height = 100 }
};

root.Sprites["OptionsSkipModeEverything"] = {
    Sheet = "Options",
    Bounds = {
        X = 2718,
        Y = 2552,
        Width = 304,
        Height = 80
    }
};
root.OptionsMenu.SkipModeOptionsSprites[#root.OptionsMenu.SkipModeOptionsSprites + 1] = "OptionsSkipModeEverything";
root.Sprites["OptionsSkipModeEverythingH"] = {
    Sheet = "Options",
    Bounds = {
        X = 2718,
        Y = 2640,
        Width = 304,
        Height = 80
    }
};
root.OptionsMenu.SkipModeOptionsHSprites[#root.OptionsMenu.SkipModeOptionsHSprites + 1] = "OptionsSkipModeEverythingH";

root.Sprites["OptionsSkipModeReadOnly"] = {
    Sheet = "Options",
    Bounds = {
        X = 2718,
        Y = 2728,
        Width = 304,
        Height = 80
    }
};
root.OptionsMenu.SkipModeOptionsSprites[#root.OptionsMenu.SkipModeOptionsSprites + 1] = "OptionsSkipModeReadOnly";
root.Sprites["OptionsSkipModeReadOnlyH"] = {
    Sheet = "Options",
    Bounds = {
        X = 2718,
        Y = 2816,
        Width = 304,
        Height = 80
    }
};
root.OptionsMenu.SkipModeOptionsHSprites[#root.OptionsMenu.SkipModeOptionsHSprites + 1] = "OptionsSkipModeReadOnlyH";

-- Voice sync

root.Sprites["OptionsVoiceSyncLabel"] = {
    Sheet = "Options",
    Bounds = { X = 0, Y = 2816, Width = 1786, Height = 100 }
};
root.Sprites["OptionsVoiceSyncLabelH"] = {
    Sheet = "Options",
    Bounds = { X = 0, Y = 2924, Width = 1786, Height = 100 }
};

-- Voice skip

root.Sprites["OptionsVoiceSkipLabel"] = {
    Sheet = "Options",
    Bounds = { X = 1794, Y = 1304, Width = 1786, Height = 100 }
};
root.Sprites["OptionsVoiceSkipLabelH"] = {
    Sheet = "Options",
    Bounds = { X = 1794, Y = 1412, Width = 1786, Height = 100 }
};

-- Voice highlight

root.Sprites["OptionsVoiceHighlightLabel"] = {
    Sheet = "Options",
    Bounds = { X = 1794, Y = 1088, Width = 1786, Height = 100 }
};
root.Sprites["OptionsVoiceHighlightLabelH"] = {
    Sheet = "Options",
    Bounds = { X = 1794, Y = 1196, Width = 1786, Height = 100 }
};

root.Sprites["OptionsSoundOn"] = {
    Sheet = "Options",
    Bounds = {
        X = 2540,
        Y = 968,
        Width = 304,
        Height = 80
    }
};
root.OptionsMenu.SoundModeOptionsSprites[#root.OptionsMenu.SoundModeOptionsSprites + 1] = "OptionsSoundOn";
root.Sprites["OptionsSoundOnH"] = {
    Sheet = "Options",
    Bounds = {
        X = 2612,
        Y = 3960,
        Width = 304,
        Height = 80
    }
};
root.OptionsMenu.SoundModeOptionsHSprites[#root.OptionsMenu.SoundModeOptionsHSprites + 1] = "OptionsSoundOnH";

root.Sprites["OptionsSoundOff"] = {
    Sheet = "Options",
    Bounds = {
        X = 2540,
        Y = 792,
        Width = 304,
        Height = 80
    }
};
root.OptionsMenu.SoundModeOptionsSprites[#root.OptionsMenu.SoundModeOptionsSprites + 1] = "OptionsSoundOff";
root.Sprites["OptionsSoundOffH"] = {
    Sheet = "Options",
    Bounds = {
        X = 2540,
        Y = 880,
        Width = 304,
        Height = 80
    }
};
root.OptionsMenu.SoundModeOptionsHSprites[#root.OptionsMenu.SoundModeOptionsHSprites + 1] = "OptionsSoundOffH";

-- Volume options

root.Sprites["OptionsBgmVolumeLabel"] = {
    Sheet = "Options",
    Bounds = { X = 0, Y = 3788, Width = 1786, Height = 100 }
};
root.Sprites["OptionsBgmVolumeLabelH"] = {
    Sheet = "Options",
    Bounds = { X = 0, Y = 3896, Width = 1786, Height = 100 }
};

root.Sprites["OptionsVoiceVolumeLabel"] = {
    Sheet = "Options",
    Bounds = { X = 0, Y = 3572, Width = 1786, Height = 100 }
};
root.Sprites["OptionsVoiceVolumeLabelH"] = {
    Sheet = "Options",
    Bounds = { X = 0, Y = 3680, Width = 1786, Height = 100 }
};

root.Sprites["OptionsSeVolumeLabel"] = {
    Sheet = "Options",
    Bounds = { X = 0, Y = 3356, Width = 1786, Height = 100 }
};
root.Sprites["OptionsSeVolumeLabelH"] = {
    Sheet = "Options",
    Bounds = { X = 0, Y = 3464, Width = 1786, Height = 100 }
};

root.Sprites["OptionsSystemSeVolumeLabel"] = {
    Sheet = "Options",
    Bounds = { X = 0, Y = 3140, Width = 1786, Height = 100 }
};
root.Sprites["OptionsSystemSeVolumeLabelH"] = {
    Sheet = "Options",
    Bounds = { X = 0, Y = 3248, Width = 1786, Height = 100 }
};

root.Sprites["OptionsCharacterVoiceVolumeLabel"] = {
    Sheet = "Options",
    Bounds = { X = 0, Y = 1196, Width = 1786, Height = 100 }
};
root.Sprites["OptionsCharacterVoiceVolumeLabelH"] = {
    Sheet = "Options",
    Bounds = { X = 0, Y = 3032, Width = 1786, Height = 100 }
};

-- Quick save options

root.Sprites["OptionsQuickSaveOptionsLabel"] = {
    Sheet = "Options",
    Bounds = { X = 0, Y = 2708, Width = 1786, Height = 100 }
};
root.Sprites["OptionsQuickSaveOptionsLabelH"] = {
    Sheet = "Options",
    Bounds = { X = 0, Y = 1088, Width = 1786, Height = 100 }
};

root.Sprites["OptionsQuickSaveChoicesOnly"] = {
    Sheet = "Options",
    Bounds = {
        X = 2406,
        Y = 2728,
        Width = 304,
        Height = 80
    }
};
root.OptionsMenu.QuickSaveOptionsSprites[#root.OptionsMenu.QuickSaveOptionsSprites + 1] = "OptionsQuickSaveChoicesOnly";
root.Sprites["OptionsQuickSaveChoicesOnlyH"] = {
    Sheet = "Options",
    Bounds = {
        X = 2406,
        Y = 2816,
        Width = 304,
        Height = 80
    }
};
root.OptionsMenu.QuickSaveOptionsHSprites[#root.OptionsMenu.QuickSaveOptionsHSprites + 1] = "OptionsQuickSaveChoicesOnlyH";

root.Sprites["OptionsQuickSaveScenesOnly"] = {
    Sheet = "Options",
    Bounds = {
        X = 2540,
        Y = 440,
        Width = 304,
        Height = 80
    }
};
root.OptionsMenu.QuickSaveOptionsSprites[#root.OptionsMenu.QuickSaveOptionsSprites + 1] = "OptionsQuickSaveScenesOnly";
root.Sprites["OptionsQuickSaveScenesOnlyH"] = {
    Sheet = "Options",
    Bounds = {
        X = 2718,
        Y = 3344,
        Width = 304,
        Height = 80
    }
};
root.OptionsMenu.QuickSaveOptionsHSprites[#root.OptionsMenu.QuickSaveOptionsHSprites + 1] = "OptionsQuickSaveScenesOnlyH";

root.Sprites["OptionsQuickSaveChoicesScenes"] = {
    Sheet = "Options",
    Bounds = {
        X = 2540,
        Y = 264,
        Width = 304,
        Height = 80
    }
};
root.OptionsMenu.QuickSaveOptionsSprites[#root.OptionsMenu.QuickSaveOptionsSprites + 1] = "OptionsQuickSaveChoicesScenes";
root.Sprites["OptionsQuickSaveChoicesScenesH"] = {
    Sheet = "Options",
    Bounds = {
        X = 2540,
        Y = 352,
        Width = 304,
        Height = 80
    }
};
root.OptionsMenu.QuickSaveOptionsHSprites[#root.OptionsMenu.QuickSaveOptionsHSprites + 1] = "OptionsQuickSaveChoicesScenesH";

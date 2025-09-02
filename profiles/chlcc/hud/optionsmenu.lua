root.OptionsMenu = {
    DrawType = DrawComponentType.SystemMenu,
    Type = OptionsMenuType.CHLCC,

    FadeInDuration = 64 / 60,
    FadeOutDuration = 64 / 60,

    BackgroundColor = 0xa988e5,

    CircleSprite = "CircleConfig",
    CircleStartPosition = { X = 20, Y = 20 },
    CircleOffset = 200,

    ErinSprite = "Erin",
    ErinPosition = { X = 301, Y = 1 },

    BackgroundFilter = "BackgroundFilter",

    InitialRedBarPosition = { X = 0, Y = 538 },
    RightRedBarPosition = { X = 1059, Y = 538 },
    RedBarDivision = 1826,
    RedBarBaseX = 1059,
    RedBarSprite = "RedBar",
    RedBarLabelPosition = { X = 1067, Y = 573 },
    RedBarLabel = "RedBarLabel",

    MenuTitleTextRightPos = { X = 550, Y = 525 },
    MenuTitleTextAngle = 6.02,
    TitleFadeInDuration = 40 / 60,
    TitleFadeOutDuration = 28 / 60,
    MenuTitleText = "MenuTitleTextConfig",

    ButtonPromptSprite = "ButtonPromptConfig",
    ButtonPromptPosition = { X = 685, Y = 651 },

    MinButtonHoldTime = 0.5,
    ButtonHoldFireInterval = 0.07,

    SelectedSprite = "SelectedSprite",
    SelectedLabelSprite = "SelectedLabelSprite",
    SelectedDotSprite = "SelectedDotSprite",
    VoiceMutedSprite = "VoiceMutedSprite",

    BasicSettingsSprite = "BasicSettingsSprite",
    BasicSettingsPos = { X = 0, Y = 46 },
    TextSettingsSprite = "TextSettingsSprite",
    TextSettingsPos = { X = 0, Y = 359 },
    SoundSettingsSprite = "SoundSettingsSprite",
    SoundSettingsPos = { X = 0, Y = 44 },
    VoiceSettingsSprite = "VoiceSettingsSprite",
    VoiceSettingsPos = { X = 0, Y = 47 },

    SliderBarBaseSprite = "SliderBarBaseSprite",
    SliderBarFillSprite = "SliderBarFillSprite",

    SettingInstantSprite = "SettingInstantSprite",
    SettingFastSprite = "SettingFastSprite",
    SettingNormalSprite = "SettingNormalSprite",
    SettingSlowSprite = "SettingSlowSprite",
    SettingShortSprite = "SettingShortSprite",
    SettingLongSprite = "SettingLongSprite",
    SettingDoSprite = "SettingDoSprite",
    SettingDontSprite = "SettingDontSprite",
    SettingYesSprite = "SettingYesSprite",
    SettingNoSprite = "SettingNoSprite",
    SettingReadSprite = "SettingReadSprite",
    SettingAllSprite = "SettingAllSprite",
    SettingOnTriggerSprite = "SettingOnTriggerSprite",
    SettingOnSceneSprite = "SettingOnSceneSprite",
    SettingOnTriggerAndSceneSprite = "SettingOnTriggerAndSceneSprite",
    SettingTypeASprite = "SettingTypeASprite",
    SettingTypeBSprite = "SettingTypeBSprite",
};

root.Sprites["CircleConfig"] = {
    Sheet = "Options",
    Bounds = { X = 1, Y = 917, Width = 106, Height = 106 }
}

root.Sprites["Erin"] = {
    Sheet = "Main",
    Bounds = { X = 641, Y = 1, Width = 978, Height = 798 }
}

root.Sprites["BackgroundFilter"] = {
    Sheet = "Main",
    Bounds = { X = 0, Y = 0, Width = 640, Height = 360 }
}

root.Sprites["RedBar"] = {
    Sheet = "Main",
    Bounds = { X = 767, Y = 913, Width = 1280, Height = 110 }
}

root.Sprites["RedBarLabel"] = {
    Sheet = "Main",
    Bounds = { X = 506, Y = 469, Width = 133, Height = 74 }
}

root.Sprites["MenuTitleTextConfig"] = {
    Sheet = "Options",
    Bounds = { X = 482, Y = 604, Width = 592, Height = 117 }
}

root.Sprites["ButtonPromptConfig"] = {
    Sheet = "Options",
    Bounds = { X = 481, Y = 824, Width = 676, Height = 28 }
}

root.Sprites["SelectedSprite"] = {
    Sheet = "Options",
    -- Yes, the single-pixel-wide overhang is in the binary
    Bounds = { X = 719, Y = 766, Width = 232, Height = 40 }
}

root.Sprites["SelectedLabelSprite"] = {
    Sheet = "Options",
    Bounds = { X = 480, Y = 724, Width = 630, Height = 40 }
}

root.Sprites["SelectedDotSprite"] = {
    Sheet = "Options",
    Bounds = { X = 42, Y = 602, Width = 18, Height = 18 }
}

root.Sprites["VoiceMutedSprite"] = {
    Sheet = "Options",
    Bounds = { X = 1, Y = 602, Width = 39, Height = 39 }
}

root.Sprites["BasicSettingsSprite"] = {
    Sheet = "Options",
    Bounds = { X = 453, Y = 1, Width = 574, Height = 314 }
}

root.Sprites["TextSettingsSprite"] = {
    Sheet = "Options",
    Bounds = { X = 453, Y = 315, Width = 504, Height = 284 }
}

root.Sprites["SoundSettingsSprite"] = {
    Sheet = "Options",
    Bounds = { X = 109, Y = 602, Width = 369, Height = 369 }
}

root.Sprites["VoiceSettingsSprite"] = {
    Sheet = "Options",
    Bounds = { X = 1, Y = 1, Width = 450, Height = 599 }
}

root.Sprites["SliderBarBaseSprite"] = {
    Sheet = "Options",
    Bounds = { X = 480, Y = 766, Width = 237, Height = 27 }
}

root.Sprites["SliderBarFillSprite"] = {
    Sheet = "Options",
    Bounds = { X = 486, Y = 795, Width = 225, Height = 27 }
}

-- Setting sprites
root.Sprites["SettingInstantSprite"] = {
    Sheet = "Options",
    Bounds = { X = 1121, Y = 1, Width = 56, Height = 28 }
}

root.Sprites["SettingFastSprite"] = {
    Sheet = "Options",
    Bounds = { X = 1121, Y = 31, Width = 56, Height = 28 }
}

root.Sprites["SettingNormalSprite"] = {
    Sheet = "Options",
    Bounds = { X = 1121, Y = 61, Width = 56, Height = 28 }
}

root.Sprites["SettingSlowSprite"] = {
    Sheet = "Options",
    Bounds = { X = 1121, Y = 91, Width = 56, Height = 28 }
}

root.Sprites["SettingShortSprite"] = {
    Sheet = "Options",
    Bounds = { X = 1121, Y = 121, Width = 56, Height = 28 }
}

root.Sprites["SettingLongSprite"] = {
    Sheet = "Options",
    Bounds = { X = 1121, Y = 151, Width = 56, Height = 28 }
}

root.Sprites["SettingDoSprite"] = {
    Sheet = "Options",
    Bounds = { X = 1121, Y = 181, Width = 56, Height = 28 }
}

root.Sprites["SettingDontSprite"] = {
    Sheet = "Options",
    Bounds = { X = 1099, Y = 211, Width = 78, Height = 28 }
}

root.Sprites["SettingYesSprite"] = {
    Sheet = "Options",
    Bounds = { X = 1121, Y = 241, Width = 56, Height = 28 }
}

root.Sprites["SettingNoSprite"] = {
    Sheet = "Options",
    Bounds = { X = 1099, Y = 271, Width = 78, Height = 28 }
}

root.Sprites["SettingReadSprite"] = {
    Sheet = "Options",
    Bounds = { X = 1069, Y = 301, Width = 108, Height = 28 }
}

root.Sprites["SettingAllSprite"] = {
    Sheet = "Options",
    Bounds = { X = 1121, Y = 331, Width = 56, Height = 28 }
}

root.Sprites["SettingOnTriggerSprite"] = {
    Sheet = "Options",
    Bounds = { X = 1047, Y = 361, Width = 130, Height = 28 }
}

root.Sprites["SettingOnSceneSprite"] = {
    Sheet = "Options",
    Bounds = { X = 1068, Y = 391, Width = 108, Height = 28 }
}

root.Sprites["SettingOnTriggerAndSceneSprite"] = {
    Sheet = "Options",
    Bounds = { X = 959, Y = 421, Width = 218, Height = 28 }
}

root.Sprites["SettingTypeASprite"] = {
    Sheet = "Options",
    Bounds = { X = 1068, Y = 451, Width = 108, Height = 28 }
}

root.Sprites["SettingTypeBSprite"] = {
    Sheet = "Options",
    Bounds = { X = 1068, Y = 481, Width = 108, Height = 28 }
}

root.OptionsMenu = {
    Type: OptionsMenuType.MO6TW,
    BackgroundSprite: "OptionsBackground",
    SliderTrackSprite: "OptionsSliderTrack",
    SliderFillSprite: "OptionsSliderFill",
    SliderThumbSprite: "OptionsSliderThumb",
    VoiceToggleEnabledSprites: [],
    VoiceToggleDisabledSprites: [],
    VoiceToggleHighlightSprite: "OptionsVoiceToggleHighlight",
    VoiceToggleStart: { X: 80, Y: 465 },
    VoiceTogglePadding: { X: 96, Y: 56 },
    VoiceTogglePerLine: 5,
    FadeInDuration: 0.2,
    FadeOutDuration: 0.2
};

root.Sprites["OptionsBackground"] = {
    Sheet: "Options",
    Bounds: { X: 0, Y: 0, Width: 1280, Height: 720 }
};

root.Sprites["OptionsSliderTrack"] = {
    Sheet: "Options",
    Bounds: { X: 1, Y: 921, Width: 480, Height: 8 }
};

root.Sprites["OptionsSliderFill"] = {
    Sheet: "Options",
    Bounds: { X: 1, Y: 931, Width: 480, Height: 8 }
};

root.Sprites["OptionsSliderThumb"] = {
    Sheet: "Options",
    Bounds: { X: 963, Y: 807, Width: 42, Height: 42 }
};

var enabledFirstX = 486;
var enabledFirstY = 753;
var disabledFirstX = 4;
var disabledFirstY = 753;

for (var i = 1; i < 15; i++) {
    root.Sprites["OptionsVoiceToggleDisabled" + i] = {
        Sheet: "Options",
        Bounds: {
            X: disabledFirstX,
            Y: disabledFirstY,
            Width: 88,
            Height: 48
        }
    };
    root.OptionsMenu.VoiceToggleDisabledSprites.push("OptionsVoiceToggleDisabled" + i);

    root.Sprites["OptionsVoiceToggleEnabled" + i] = {
        Sheet: "Options",
        Bounds: {
            X: enabledFirstX,
            Y: enabledFirstY,
            Width: 88,
            Height: 48
        }
    };
    root.OptionsMenu.VoiceToggleEnabledSprites.push("OptionsVoiceToggleEnabled" + i);

    if (i % 5 == 0) {
        enabledFirstX = 486;
        disabledFirstX = 4;
        enabledFirstY += 56;
        disabledFirstY += 56;
    } else {
        disabledFirstX += 96;
        enabledFirstX += 96;
    }
}

root.Sprites["OptionsVoiceToggleHighlight"] = {
    Sheet: "Options",
    Bounds: { X: 968, Y: 753, Width: 88, Height: 48 }
};

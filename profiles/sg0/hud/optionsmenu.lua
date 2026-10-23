-- STEINS;GATE 0 config (config.dds): BASIC / TEXT / SOUND / VOICE pages.
-- Each row is a label line (with its dotted leader) plus either choice words,
-- drawn at their offset from the label in the sheet, or a MIN-MAX slider.
-- Console-only rows of the sheet (screen size, touchscreen, L/R message send)
-- are left out.
root.SpriteSheets["Config"] = { Path = { Mount = "system", Id = 3 }, DesignWidth = 3072, DesignHeight = 2496 };

local function sprite(name, x, y, w, h)
    root.Sprites[name] = { Sheet = "Config", Bounds = { X = x, Y = y, Width = w, Height = h } };
    return name;
end

local on = sprite("ConfigOn", 2788, 1104, 68, 36);
local off = sprite("ConfigOff", 2936, 1104, 96, 36);
local function onOffRow(page, setting, name, y, x, w)
    -- Every ON / OFF pair sits at x 2788 / 2936 on its own line.
    return {
        Page = page, Setting = setting,
        Label = sprite(name, x, y - 4, w, 44),
        Choices = { sprite(name .. "On", 2788, y, 68, 36), sprite(name .. "Off", 2936, y, 96, 36) },
    };
end
local function sliderRow(page, setting, name, x, y, w, index)
    return { Page = page, Setting = setting, Index = index, Label = sprite(name, x, y, w, 44) };
end

local characters = {};
for i = 0, 15 do
    local col, row = i % 4, math.floor(i / 4);
    local cellY = ({ 2180, 2256, 2336, 2412 })[row + 1];
    characters[i + 1] = sliderRow(3, "CharacterVoice", "ConfigVoiceChar" .. i, 8 + col * 624, cellY, 608, i);
    root.Sprites["ConfigVoiceChar" .. i].Bounds.Height = 64;
end

local rows = {
    -- BASIC
    { Page = 0, Setting = "FullScreen", Label = sprite("ConfigFullScreen", 1944, 556, 568, 44),
      Choices = { sprite("ConfigFullScreenOn", 2532, 556, 272, 40), sprite("ConfigWindow", 2856, 556, 192, 40) } },
    { Page = 0, Setting = "Resolution", Label = sprite("ConfigResolution", 1944, 628, 528, 44),
      Choices = { sprite("ConfigRes1024", 2488, 632, 160, 32), sprite("ConfigRes1280", 2676, 632, 160, 32),
                  sprite("ConfigRes1920", 2868, 632, 180, 32) } },
    onOffRow(0, "TriggerStopSkip", "ConfigSkipRelease", 1104, 1944, 804),
    onOffRow(0, "ShowTipsNotification", "ConfigTipsInfo", 1176, 1940, 808),
    { Page = 0, Setting = "AutoQuickSave", Label = sprite("ConfigAutoQuickSave", 1940, 1244, 472, 48),
      Choices = { sprite("ConfigAqsScene", 2436, 1248, 120, 36), sprite("ConfigAqsTrigger", 2592, 1248, 168, 36),
                  sprite("ConfigAqsAll", 2796, 1248, 76, 36), sprite("ConfigAqsOff", 2936, 1248, 96, 36) } },
    -- TEXT
    sliderRow(1, "TextSpeed", "ConfigMessageSpeed", 1940, 1460, 608),
    sliderRow(1, "AutoSpeed", "ConfigAutoDelay", 1940, 1532, 604),
    { Page = 1, Setting = "SkipRead", Label = sprite("ConfigSkipMode", 1944, 1604, 724, 44),
      Choices = { sprite("ConfigSkipReadOnly", 2696, 1608, 188, 36), sprite("ConfigSkipAll", 2956, 1608, 76, 36) } },
    -- SOUND
    sliderRow(2, "VoiceVolume", "ConfigVoiceVolume", 1940, 1748, 604),
    sliderRow(2, "MusicVolume", "ConfigMusicVolume", 1940, 1820, 604),
    sliderRow(2, "SEVolume", "ConfigSEVolume", 1944, 1892, 600),
    sliderRow(2, "MovieVolume", "ConfigMovieVolume", 1940, 1968, 604),
    onOffRow(2, "SyncVoice", "ConfigSyncVoice", 2040, 1944, 804),
    onOffRow(2, "SkipVoice", "ConfigSkipVoice", 2112, 1944, 804),
};
for _, row in ipairs(characters) do rows[#rows + 1] = row; end

root.OptionsMenu = {
    Type = OptionsMenuType.SG0,
    DrawType = DrawComponentType.SystemMenu,
    -- _Startup_win counts SW_SYSSUBMENUCT to 32 while the config opens.
    CountMax = 32,
    FadeInDuration = 32 / 60,
    FadeOutDuration = 32 / 60,

    BackgroundSprite = sprite("ConfigBackground", 0, 0, 1920, 1080),
    GuideSprite = "ConfigGuide",
    GuidePos = { X = 1000, Y = 1012 },
    VoiceGuideSprite = "ConfigVoiceGuide",
    VoiceGuidePos = { X = 600, Y = 1016 },

    PageButtonSprites = {
        sprite("ConfigPageBasic", 1936, 16, 472, 148), sprite("ConfigPageText", 1936, 196, 472, 148),
        sprite("ConfigPageSound", 1936, 376, 472, 148), sprite("ConfigPageVoice", 2424, 376, 476, 148),
    },
    PageButtonBounds = {
        { X = 172, Y = 56, Width = 472, Height = 148 }, { X = 172, Y = 236, Width = 472, Height = 148 },
        { X = 172, Y = 416, Width = 472, Height = 148 }, { X = 172, Y = 596, Width = 476, Height = 148 },
    },
    PageHeadingSprites = {
        sprite("ConfigHeadingBasic", 1940, 752, 392, 52), sprite("ConfigHeadingText", 1940, 840, 372, 48),
        sprite("ConfigHeadingSound", 1940, 932, 420, 48), sprite("ConfigHeadingVoice", 1940, 1024, 400, 48),
    },
    HeadingPos = { X = 724, Y = 70 },

    ContentX = 720,
    RowsTop = 170,
    RowHeight = 72,
    VoiceRowHeight = 52,
    RowPadding = 8,
    RowWidth = 1150,

    SliderFrameSprite = sprite("ConfigSliderFrame", 2428, 60, 488, 56),
    SliderFillSprite = sprite("ConfigSliderFill", 2436, 132, 476, 44),
    SliderKnobSprite = sprite("ConfigSliderKnob", 2932, 56, 100, 80),
    SliderOffset = { X = 640, Y = -6 },
    SliderFillOffset = { X = 6, Y = 6 },
    SliderKnobOffsetY = -12,
    VoiceSliderScale = 0.66,
    VoiceSliderOffsetY = 12,
    VoiceOffSprite = sprite("ConfigVoiceOff", 2436, 312, 476, 44),

    HighlightColor = 0xFFA040,
    HighlightOpacity = 0.14,
    UnselectedBrightness = 0.45,

    Rows = rows
};

root.Sprites["ConfigGuide"] = { Sheet = "SysTips", Bounds = { X = 980, Y = 1776, Width = 908, Height = 64 } };
root.Sprites["ConfigVoiceGuide"] = { Sheet = "SysTips", Bounds = { X = 596, Y = 1856, Width = 1292, Height = 60 } };

root.SpriteSheets = {
    ["Album"] = {
        Path = { Mount = "system", Id = 0 },
        DesignWidth = 4096,
        DesignHeight = 4096
    },
    ["Backlog"] = {
        Path = { Mount = "system", Id = 1 },
        DesignWidth = 2048,
        DesignHeight = 2048
    },
    ["ClearList"] = {
        Path = { Mount = "system", Id = 3 },
        DesignWidth = 4096,
        DesignHeight = 2324
    },
    ["Config"] = {
        Path = { Mount = "system", Id = 4 },
        DesignWidth = 2048,
        DesignHeight = 2538
    },
    ["ConfigEx"] = {
        Path = { Mount = "system", Id = 5 },
        DesignWidth = 4096,
        DesignHeight = 3272
    },
    ["Data"] = {
        Path = { Mount = "system", Id = 6 },
        DesignWidth = 3072,
        DesignHeight = 1536
    },
    ["Menu"] = {
        Path = { Mount = "system", Id = 8 },
        DesignWidth = 2048,
        DesignHeight = 1024
    },
    ["Font"] = {
        Path = { Mount = "system", Id = 9 },
        DesignWidth = 3072,
        DesignHeight = 5616
    },
    ["LibraryMenu"] = {
        Path = { Mount = "system", Id = 14},
        DesignWidth = 2048,
        DesignHeight = 2048
    },
    ["LoadMenu"] = {
        Path = { Mount = "system", Id = 15},
        DesignWidth = 2048,
        DesignHeight = 2048
    },
    ["MapBG"] = {
        Path = { Mount = "system", Id = 16 },
        DesignWidth = 3378,
        DesignHeight = 1900
    },
    ["MapParts"] = {
        Path = { Mount = "system", Id = 18 },
        DesignWidth = 2048,
        DesignHeight = 3072
    },
    ["MenuBG"] = {
        Path = { Mount = "system", Id = 19 },
        DesignWidth = 3000,
        DesignHeight = 1500
    },
    ["MenuChip"] = {
        Path = { Mount = "system", Id = 20 },
        DesignWidth = 4096,
        DesignHeight = 2048
    },
    ["MesBox"] = {
        Path = { Mount = "system", Id = 21 },
        DesignWidth = 1920,
        DesignHeight = 600
    },
    ["MovMenu"] = {
        Path = { Mount = "system", Id = 22 },
        DesignWidth = 3072,
        DesignHeight = 1228
    },
    ["MusicMenu"] = {
        Path = { Mount = "system", Id = 23 },
        DesignWidth = 3058,
        DesignHeight = 1492
    },
    ["NamePlate"] = {
        Path = { Mount = "system", Id = 24 },
        DesignWidth = 4096,
        DesignHeight = 2048
    },
    ["QuickLoadMenu"] = {
        Path = { Mount = "system", Id = 25},
        DesignWidth = 2048,
        DesignHeight = 2048
    },
    ["SaveMenu"] = {
        Path = { Mount = "system", Id = 26},
        DesignWidth = 2048,
        DesignHeight = 2048
    },
    ["SysMesBox"] = {
        Path = { Mount = "system", Id = 27 },
        DesignWidth = 2048,
        DesignHeight = 1528
    },
    ["Tips"] = {
        Path = { Mount = "system", Id = 28 },
        DesignWidth = 2048,
        DesignHeight = 2048
    },
    ["Title"] = {
        Path = { Mount = "system", Id = 30 },
        DesignWidth = 1920,
        DesignHeight = 2160
    },
    ["TitleChip"] = {
        Path = { Mount = "system", Id = 31 },
        DesignWidth = 2048,
        DesignHeight = 800
    },
    ["YesNoBG0"] = {
        Path = { Mount = "system", Id = 35 },
        DesignWidth = 3840,
        DesignHeight = 2160
    },
    ["YesNoBG1"] = {
        Path = { Mount = "system", Id = 36 },
        DesignWidth = 3840,
        DesignHeight = 2160
    },
    ["YesNoBG2"] = {
        Path = { Mount = "system", Id = 37 },
        DesignWidth = 3840,
        DesignHeight = 2160
    },
    ["YesNoBG3"] = {
        Path = { Mount = "system", Id = 38 },
        DesignWidth = 3840,
        DesignHeight = 2160
    },
    ["YesNoChip"] = {
        Path = { Mount = "system", Id = 39 },
        DesignWidth = 1536,
        DesignHeight = 2400
    },
};

local numberOfPages = root.Language == "Japanese" and 16 or 1

for i = 0, numberOfPages do
    root.SpriteSheets["ManualSheet" .. i] = {
        Path = { Mount = "manual", Id = i },
        DesignWidth = 1920,
        DesignHeight = 1080
    };
end

root.Sprites = {};
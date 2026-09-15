root.Vfs = {
    Mounts = {
        ["bg"] = { root.BasePaths.RootGamedataDir .. "/cclcc-switch/bg_webp.mpk" },
        ["bgm"] = { root.BasePaths.RootGamedataDir .. "/cclcc-switch/bgm_ninopus.mpk" },
        ["chara"] = { root.BasePaths.RootGamedataDir .. "/cclcc-switch/chara_webp.mpk" },
        ["mask"] = { root.BasePaths.RootGamedataDir .. "/cclcc-switch/mask.mpk" },
        ["mes"] = {
            {
                Path = root.BasePaths.RootGamedataDir .. "/cclcc-switch/script/mes00",
                Order = root.BasePaths.RootGamedataDir .. "/cclcc-switch/script.cls",
                Whitelist = ".*mes.*\\.msb",
            },
        },
        ["movie"] = { root.BasePaths.RootGamedataDir .. "/cclcc-switch/movie" },
        ["script"] = {
            {
                Path = root.BasePaths.RootGamedataDir .. "/cclcc-switch/script",
                Order = root.BasePaths.RootGamedataDir .. "/cclcc-switch/script.cls",
                Whitelist = ".*script.*\\.scx",
            },
        },
        -- ["scriptdbg"] = {root.BasePaths.RootGamedataDir .. "/cclcc-switch/scriptdbg"},
        ["se"] = { root.BasePaths.RootGamedataDir .. "/cclcc-switch/se_ninopus.mpk" },
        ["sysse"] = { root.BasePaths.RootGamedataDir .. "/cclcc-switch/sysse_ninopus.mpk" },
        ["system"] = { root.BasePaths.RootGamedataDir .. "/cclcc-switch/system_swi.mpk" },
        ["voice"] = { root.BasePaths.RootGamedataDir .. "/cclcc-switch/voice_ninopus.mpk" }
    }
};

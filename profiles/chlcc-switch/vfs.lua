local data = root.BasePaths.RootGamedataDir .. "/chlcc-switch";
root.Vfs = {
    Mounts = {
        ["bg"] = { data .. "/bg_webp.mpk" },
        ["bgm"] = { data .. "/bgm_ninopus.mpk" },
        ["chara"] = { data .. "/chara_webp.mpk" },
        ["chara2"] = { data .. "/chara2_webp.mpk" },
        ["mask"] = { data .. "/mask.mpk" },
        ["mes"] = {
            {
                Path = data .. "/script/mes00",
                Order = data .. "/script.cls",
                Whitelist = ".*mes.*\\.msb"
            }
        },
        ["movie"] = { data .. "/movie.mlp" },
        ["script"] = {
            {
                Path = data .. "/script",
                Order = data .. "/script.cls",
                Whitelist = ".*script.*\\.scx"
            }
        },
        ["se"] = { data .. "/se_ninopus.mpk" },
        ["sysse"] = { data .. "/sysse_ninopus.mpk" },
        ["system"] = { data .. "/system_swi.mpk" },
        ["voice"] = { data .. "/voice_ninopus.mpk" }
    }
};

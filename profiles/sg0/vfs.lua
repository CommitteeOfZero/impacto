-- STEINS;GATE 0 Steam (Windows) layout: USRDIR/*.mpk plus Bink 2 movies in USRDIR/movie/<resolution>/.
root.Vfs = {
    Mounts = {
        ["script"] = {root.BasePaths.RootGamedataDir .. "/sg0/script.mpk"},
        ["system"] = {root.BasePaths.RootGamedataDir .. "/sg0/system.mpk"},
        ["bgm"] = {root.BasePaths.RootGamedataDir .. "/sg0/bgm.mpk"},
        ["se"] = {root.BasePaths.RootGamedataDir .. "/sg0/se.mpk"},
        ["voice"] = {root.BasePaths.RootGamedataDir .. "/sg0/voice.mpk"},
        ["bg"] = {root.BasePaths.RootGamedataDir .. "/sg0/bg.mpk"},
        ["chara"] = {root.BasePaths.RootGamedataDir .. "/sg0/chara.mpk"},
        ["mask"] = {root.BasePaths.RootGamedataDir .. "/sg0/mask.mpk"},
        ["manual"] = {root.BasePaths.RootGamedataDir .. "/sg0/manual.mpk"},
        ["movie"] = {
            {
                Path = root.BasePaths.RootGamedataDir .. "/sg0/movie/1920x1080",
                Whitelist = ".*\\.bk2",
                -- Movie ids follow plain ASCII order of the file names (uppercase first):
                -- the scripts play 32 = intro and 16 = OP at the title, 4/8-10 = IMV inserts.
                Order = root.BasePaths.RootProfilesDir .. "/sg0/movie.cls",
            },
        }
    }
};

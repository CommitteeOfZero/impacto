-- STEINS;GATE 0 EXTRA screens. Sheet rectangles measured with
-- scripts/sheet_regions.py: exmenu.dds (library chrome), exmenu2.dds (the
-- sound library's player window), album_thum.dds (108 CG thumbnails, 288x162
-- on a 294x168 pitch, 10 per row) and clearlist.dds (Nixie tubes).
root.SpriteSheets["ExMenu"] = { Path = { Mount = "system", Id = 7 }, DesignWidth = 3072, DesignHeight = 3072 };
root.SpriteSheets["ExMenu2"] = { Path = { Mount = "system", Id = 8 }, DesignWidth = 1920, DesignHeight = 984 };
root.SpriteSheets["AlbumThumb"] = { Path = { Mount = "system", Id = 0 }, DesignWidth = 2944, DesignHeight = 1856 };
root.SpriteSheets["ClearList"] = { Path = { Mount = "system", Id = 2 }, DesignWidth = 3072, DesignHeight = 2432 };

local function sprite(name, sheet, x, y, w, h)
    root.Sprites[name] = { Sheet = sheet, Bounds = { X = x, Y = y, Width = w, Height = h } };
    return name;
end

local thumbnails = {};
for i = 0, 107 do
    thumbnails[#thumbnails + 1] = sprite("LibraryCgThumb" .. i, "AlbumThumb",
        294 * (i % 10), 168 * math.floor(i / 10), 288, 162);
end

-- Sound library tracks: _system strings 151.. (title, credit) pairs. Tracks
-- 1-28 are bgm201-228 (loop ids 1, 3, .. 55, no-loop ids 2, 4, .. 56), then
-- bgm229nl, bgm230 and the four vocal songs (song01-04), which are also
-- unlocked by watching the movie they belong to (OP, Ending1-3).
local trackBgm, trackBgmAlt, trackMovie = {}, {}, {};
for t = 0, 27 do
    trackBgm[#trackBgm + 1] = 1 + 2 * t;
    trackBgmAlt[#trackBgmAlt + 1] = 2 + 2 * t;
    trackMovie[#trackMovie + 1] = -1;
end
for _, track in ipairs({ { 57, -1, -1 }, { 58, 59, -1 }, { 69, -1, 0 }, { 70, -1, 1 }, { 71, -1, 2 }, { 72, -1, 3 } }) do
    trackBgm[#trackBgm + 1] = track[1];
    trackBgmAlt[#trackBgmAlt + 1] = track[2];
    trackMovie[#trackMovie + 1] = track[3];
end

root.ExtraMenus = {
    LibraryMenu = {
        Type = LibraryMenuType.SG0,
        DrawType = DrawComponentType.SystemMenu,
        -- _Startup_win counts SW_SYSSUBMENUCT to 32 while a library opens.
        CountMax = 32,

        BackgroundSprite = sprite("LibraryBackground", "ExMenu", 0, 0, 1920, 1080),
        TabSprites = {
            sprite("LibraryTabClearList", "ExMenu", 1928, 0, 416, 64),
            sprite("LibraryTabCg", "ExMenu", 1928, 64, 416, 64),
            sprite("LibraryTabSound", "ExMenu", 1928, 130, 416, 64),
            sprite("LibraryTabMovie", "ExMenu", 1928, 196, 416, 64),
        },
        TabSelectedSprites = {
            sprite("LibraryTabClearListOn", "ExMenu", 2344, 0, 416, 64),
            sprite("LibraryTabCgOn", "ExMenu", 2344, 64, 416, 64),
            sprite("LibraryTabSoundOn", "ExMenu", 2344, 130, 416, 64),
            sprite("LibraryTabMovieOn", "ExMenu", 2344, 196, 416, 64),
        },
        TabPos = { X = 60, Y = 214 },
        TabPitch = 66,
        TitleSprites = {
            sprite("LibraryTitleCg", "ExMenu", 1928, 268, 228, 52),
            sprite("LibraryTitleSound", "ExMenu", 2168, 268, 292, 52),
            sprite("LibraryTitleMovie", "ExMenu", 2476, 268, 280, 52),
        },
        TitlePos = { X = 60, Y = 130 },
        GuideSprites = {
            sprite("LibraryGuideCg", "ExMenu", 1016, 2180, 872, 60),
            sprite("LibraryGuideViewer", "ExMenu", 884, 2256, 1004, 64),
            sprite("LibraryGuideSound", "ExMenu", 748, 2336, 1140, 60),
            sprite("LibraryGuideMovie", "ExMenu", 1016, 2412, 872, 64),
        },
        GuideRight = 1888,
        GuideY = 1008,

        ThumbnailSprites = thumbnails,
        NoDataSprite = sprite("LibraryNoData", "ExMenu", 2772, 172, 296, 172),
        NoDataSelectedSprite = sprite("LibraryNoDataOn", "ExMenu", 2772, 696, 296, 168),
        SelectedFrameSprite = sprite("LibraryThumbFrame", "ExMenu", 2772, 360, 296, 168),
        SelectedGlowSprite = sprite("LibraryThumbGlow", "ExMenu", 2764, 0, 292, 164),
        CgGridPos = { X = 520, Y = 112 },
        CgCellPitch = { X = 334, Y = 202 },
        CgColumns = 4,
        CgRows = 4,
        ThumbnailOffset = { X = 4, Y = 5 },
        ScrollThumbSprite = sprite("LibraryScrollThumb", "ExMenu", 2636, 332, 28, 72),
        ScrollX = 1846,
        ScrollTop = 112,
        ScrollBottom = 890,
        -- EV_SP01-04 have no EV flag (see savedata.lua AlbumEvData).
        AllClearCgs = { 102, 103, 104, 105 },
        EndingFlags = { 801, 802, 803, 804, 805, 806 },
        -- Script BG buffer (bit) the loader thread fills: 4 = BG3, unused on the title.
        CgLoadBuffer = 4,
        CgZoomMax = 2.0,
        CgScrollSpeed = 900,

        PlayerSprite = sprite("LibraryPlayer", "ExMenu2", 4, 4, 1804, 972),
        PlayerPos = { X = 58, Y = 14 },
        ListHeaderSprite = sprite("LibraryListHeader", "ExMenu", 0, 2712, 1348, 44),
        ListRowSprites = {
            sprite("LibraryListRowA", "ExMenu", 0, 2763, 1348, 63),
            sprite("LibraryListRowB", "ExMenu", 0, 2826, 1348, 63),
        },
        ListSelectedSprite = sprite("LibraryListRowOn", "ExMenu", 0, 2645, 1348, 62),
        -- The player window's white list pane (exmenu2 x 422-1765, y 152-919).
        ListBounds = { X = 476, Y = 162, Width = 1344, Height = 768 },
        ListRowHeight = 63,
        ListVisibleRows = 11,
        ListTrackX = 20,
        ListTitleX = 132,
        ListArtistX = 612,
        ListFontSize = 30,
        -- The player's LCD (exmenu2 x 908-1675, y 32-117), right of its icon.
        NowPlayingPos = { X = 1080, Y = 50 },
        NowPlayingFontSize = 28,
        RepeatModeSprites = {
            sprite("LibraryRepeatOne", "ExMenu", 2768, 536, 48, 40),
            sprite("LibraryPlayThrough", "ExMenu", 2768, 574, 48, 40),
            sprite("LibraryRepeatAll", "ExMenu", 2768, 610, 48, 40),
            sprite("LibraryShuffle", "ExMenu", 2768, 646, 48, 40),
        },
        RepeatModePos = { X = 1780, Y = 938 },
        PlayIconSprite = sprite("LibraryPlayIcon", "ExMenu", 2680, 336, 76, 76),
        TrackStringBase = 151,
        LockedTrackString = 148,
        TrackBgmIds = trackBgm,
        TrackBgmAltIds = trackBgmAlt,
        TrackMovieUnlocks = trackMovie,

        -- Opening, Ending1-3, Animelo Summer Live 2015 (movie.cls ids 16, 0, 1,
        -- 2, 31; titles are _system strings 135..).
        MovieThumbnailSprites = {
            sprite("LibraryMovieOp", "ExMenu", 1926, 1800, 480, 272),
            sprite("LibraryMovieEd1", "ExMenu", 2412, 1800, 480, 272),
            sprite("LibraryMovieEd2", "ExMenu", 1926, 2076, 480, 272),
            sprite("LibraryMovieEd3", "ExMenu", 2412, 2076, 480, 272),
            sprite("LibraryMovieAnisama", "ExMenu", 1926, 2352, 480, 272),
        },
        MovieIds = { 16, 0, 1, 2, 31 },
        MovieGridPos = { X = 540, Y = 200 },
        MovieCellPitch = { X = 430, Y = 380 },
        MovieColumns = 3,
        MovieThumbnailScale = 0.8,
        MovieTitleOffsetY = 14,
        MovieStringBase = 135,
        MovieFontSize = 30,
        -- {script id, line id, ...}: the line shown just before each place the
        -- story plays the movie (Assign W61 = movie, then the movie macro).
        MovieUnlockLines = {
            { 19, 153, 72, 183 },
            { 27, 248, 33, 223, 119, 187 },
            { 17, 383, 44, 208, 65, 126, 90, 866, 118, 85 },
            { 21, 201, 45, 451, 109, 185 },
            { 54, 347, 75, 49 },
        },

        TextColor = 0x303030,
        LockedTextColor = 0x8C8C8C,
        SelectedTextColor = 0xFFFFFF,
        -- _system.scx is loaded in script buffer 1 on the title.
        SystemScriptBuffer = 1,
    },

    ClearListMenu = {
        Type = ClearListMenuType.SG0,
        DrawType = DrawComponentType.SystemMenu,
        -- _Startup_win counts SW_SYSSUBMENUCT to 64 while the clear list opens.
        CountMax = 64,
        BackgroundSprite = sprite("ClearListBackground", "ClearList", 0, 0, 1920, 1080),
        -- Left to right: Kagari, Maho, Kurisu, Okabe, Mayuri, Leskinen; each lit
        -- tube is aligned glass-to-glass with the backdrop's tube.
        TubeSprites = {
            sprite("ClearListTube0", "ClearList", 1928, 4, 280, 924),
            sprite("ClearListTube1", "ClearList", 2226, 4, 280, 924),
            sprite("ClearListTube2", "ClearList", 2525, 4, 280, 924),
            sprite("ClearListTube3", "ClearList", 1931, 940, 280, 924),
            sprite("ClearListTube4", "ClearList", 2229, 940, 280, 924),
            sprite("ClearListTube5", "ClearList", 2528, 940, 280, 924),
        },
        TubePositions = {
            { X = 74, Y = 54 }, { X = 372, Y = 54 }, { X = 671, Y = 54 },
            { X = 971, Y = 54 }, { X = 1269, Y = 54 }, { X = 1568, Y = 54 },
        },
        -- Recursive Mother Goose, Twin Automata, Promised Rinascimento,
        -- Milky-way Crossing, Vega and Altair, Gehenna's Stigma.
        LabelSprites = {
            sprite("ClearListLabel0", "ClearList", 8, 2176, 288, 244),
            sprite("ClearListLabel1", "ClearList", 312, 2176, 300, 244),
            sprite("ClearListLabel2", "ClearList", 628, 2176, 268, 244),
            sprite("ClearListLabel3", "ClearList", 916, 2176, 264, 244),
            sprite("ClearListLabel4", "ClearList", 1196, 2176, 272, 244),
            sprite("ClearListLabel5", "ClearList", 1492, 2176, 268, 244),
        },
        -- Over the tube bases, ending above the BACK guide.
        LabelPositions = {
            { X = 68, Y = 772 }, { X = 360, Y = 772 }, { X = 675, Y = 772 },
            { X = 977, Y = 772 }, { X = 1271, Y = 772 }, { X = 1572, Y = 772 },
        },
        -- CLRFLG.scx sets flag 801 + ending: 0 A04 (Promised Rinascimento),
        -- 1 M01 (Twin Automata), 2 K01 (Recursive Mother Goose), 3 B01 (Vega
        -- and Altair), 4 B02 (Gehenna's Stigma), 5 T01 (Milky-way Crossing).
        EndingFlags = { 803, 802, 801, 806, 804, 805 },
        GuideSprite = sprite("ClearListGuide", "ClearList", 2848, 8, 168, 60),
        GuidePos = { X = 1736, Y = 1016 },
    },
};

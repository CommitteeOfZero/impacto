

root.LayerCount = 100;
root.GameFeatures = GameFeature.Sc3VirtualMachine | GameFeature.Renderer2D | GameFeature.Input | GameFeature.Audio | GameFeature.Video;
root.DesignWidth = 1920;
root.DesignHeight = 1080;

root.WindowName = "STEINS;GATE 0";
root.WindowIconPath = "resources/cc/icondata/icon.png";
root.CursorArrowPath = "resources/cc/icondata/cursor_arrow.png";
root.CursorPointerPath = "resources/cc/icondata/cursor_pointer.png";
root.UseMoviePriority = true;
root.UseWaveEffects = true;

root.CharaIsMvl = false;
root.LayFileBigEndian = false;
root.LayFileTexXMultiplier = 1;
root.LayFileTexYMultiplier = 1;

root.ScreenCaptureCount = 2;

root.Vm = {
    StartScript = 6, -- _Startup_win.scx in SG0 Steam script.mpk
    StartScriptBuffer = 0,
    GameInstructionSet = InstructionSet.SG0,
    UseReturnIds = true,

    ScrWorkChaStructSize = 40,
    ScrWorkChaOffsetStructSize = 10,
    ScrWorkBgStructSize = 40,
    ScrWorkBgOffsetStructSize = 10,
    ScrWorkCaptureStructSize = 20,
    ScrWorkCaptureOffsetStructSize = 10,
    ScrWorkCaptureEffectInfoStructSize = 3,
    ScrWorkBgEffStructSize = 30,
    ScrWorkBgEffOffsetStructSize = 20,

    MaxLinkedBgBuffers = 2
};

root.PlatformId = 131072;

include(root.BasePaths.RootProfilesDir .. '/common/scriptinput.lua');
-- SG0 PC keys per the game's own help page (manual.mpk help00):
-- the phone ("RINE trigger", script key 22, unmapped in common) is Z or C,
-- i.e. L2 / R2; F5 is Load and F7 Quick Load (common has them the other way).
root.Input.PADcustomA[23] = root.PADinput.PAD1L2 | root.PADinput.PAD1R2;
root.Input.PADcustomB[23] = root.PADinput.PAD1L2 | root.PADinput.PAD1R2;
root.Input.KBcustom[55] = {KeyboardScanCode._F7};
root.Input.KBcustom[58] = {KeyboardScanCode._F5};
include(root.BasePaths.RootProfilesDir .. '/common/scriptvars.lua');
include(root.BasePaths.RootProfilesDir .. '/sg0/scriptvars.lua');
include(root.BasePaths.RootProfilesDir .. '/sg0/savedata.lua');
include(root.BasePaths.RootProfilesDir .. '/sg0/tipssystem.lua');
include(root.BasePaths.RootProfilesDir .. '/sg0/vfs.lua');
include(root.BasePaths.RootProfilesDir .. '/sg0/sprites.lua');
include(root.BasePaths.RootProfilesDir .. '/common/animation.lua');
include(root.BasePaths.RootProfilesDir .. '/common/achievementnotification.lua');
include(root.BasePaths.RootProfilesDir .. '/sg0/charset.lua');
include(root.BasePaths.RootProfilesDir .. '/sg0/font.lua');
include(root.BasePaths.RootProfilesDir .. '/sg0/dialogue.lua');
include(root.BasePaths.RootProfilesDir .. '/sg0/waveeffects.lua');
include(root.BasePaths.RootProfilesDir .. '/sg0/hud/saveicon.lua');
include(root.BasePaths.RootProfilesDir .. '/sg0/hud/loadingdisplay.lua');
include(root.BasePaths.RootProfilesDir .. '/sg0/hud/datedisplay.lua');
include(root.BasePaths.RootProfilesDir .. '/sg0/hud/titlemenu.lua');
include(root.BasePaths.RootProfilesDir .. '/sg0/hud/savemenu.lua');
include(root.BasePaths.RootProfilesDir .. '/sg0/hud/phone.lua');
include(root.BasePaths.RootProfilesDir .. '/sg0/hud/systemmenu.lua');
include(root.BasePaths.RootProfilesDir .. '/sg0/hud/helpmenu.lua');
include(root.BasePaths.RootProfilesDir .. '/sg0/hud/backlogmenu.lua');
include(root.BasePaths.RootProfilesDir .. '/sg0/hud/sysmesboxdisplay.lua');
include(root.BasePaths.RootProfilesDir .. '/sg0/hud/selectiondisplay.lua');
include(root.BasePaths.RootProfilesDir .. '/sg0/hud/tipsmenu.lua');
include(root.BasePaths.RootProfilesDir .. '/sg0/hud/optionsmenu.lua');
include(root.BasePaths.RootProfilesDir .. '/sg0/hud/tipsnotification.lua');
include(root.BasePaths.RootProfilesDir .. '/sg0/hud/extramenus.lua');

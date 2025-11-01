root.ActiveRenderer = RendererType.OpenGL;

root.LayerCount = 100;
root.GameFeatures = GameFeature.Sc3VirtualMachine | GameFeature.Renderer2D | GameFeature.Input | GameFeature.Audio;
root.DesignWidth = 1280;
root.DesignHeight = 720;

root.WindowName = "STEINS;GATE: Hiyoku Renri no Darling";
root.WindowIconPath = "profiles/darling/icon.png";
root.CursorArrowPath = "profiles/darling/cursor_arrow.png";
root.CursorPointerPath = "profiles/darling/cursor_pointer.png";

root.CharaIsMvl = false;
root.LayFileBigEndian = true;
root.LayFileTexXMultiplier = 2048;
root.LayFileTexYMultiplier = 1024;

root.Vm = {
    StartScript = 2,
    StartScriptBuffer = 0,
    GameInstructionSet = InstructionSet.Darling,
    UseReturnIds = true,

    ScrWorkChaStructSize = 40,
    ScrWorkChaOffsetStructSize = 10,
    ScrWorkBgStructSize = 40,
    ScrWorkBgOffsetStructSize = 10,
    ScrWorkCaptureStructSize = 20,
    ScrWorkCaptureOffsetStructSize = 10,
    ScrWorkBgEffStructSize = 30,
    ScrWorkBgEffOffsetStructSize = 20,

};

include('common/scriptinput.lua');
include('common/scriptvars.lua');
include('darling/config.lua');
include('darling/scriptvars.lua');
include('darling/savedata.lua');
include('darling/tipssystem.lua');
include('darling/vfs.lua');
include('darling/sprites.lua');
include('common/animation.lua');
include('darling/charset.lua');
include('darling/font.lua');
include('darling/dialogue.lua');
include('darling/hud/saveicon.lua');
include('darling/hud/loadingdisplay.lua');
include('darling/hud/datedisplay.lua');
include('darling/hud/titlemenu.lua');
include('darling/hud/backlogmenu.lua');
--include('darling/hud/systemmenu.lua');
include('darling/hud/sysmesboxdisplay.lua');
include('darling/hud/selectiondisplay.lua');
include('darling/hud/tipsmenu.lua');
include('darling/hud/tipsnotification.lua');
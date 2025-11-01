root.ActiveRenderer = RendererType.OpenGL;

root.LayerCount = 100;
root.GameFeatures = GameFeature.Sc3VirtualMachine | GameFeature.Renderer2D | GameFeature.Input | GameFeature.Audio | GameFeature.Video;
root.DesignWidth = 1280;
root.DesignHeight = 720;

root.WindowName = "STEINS;GATE";
root.WindowIconPath = "profiles/sgps3/icon.png";
root.CursorArrowPath = "profiles/sgps3/cursor_arrow.png";
root.CursorPointerPath = "profiles/sgps3/cursor_pointer.png";

root.CharaIsMvl = false;
root.LayFileBigEndian = true;
root.LayFileTexXMultiplier = 2048;
root.LayFileTexYMultiplier = 1024;
root.UseScreenCapEffects = false;

root.Vm = {
    StartScript = 2,
    StartScriptBuffer = 0,
    GameInstructionSet = InstructionSet.SGPS3,
    UseReturnIds = false,

    ScrWorkChaStructSize = 20,
    ScrWorkChaOffsetStructSize = 10,
    ScrWorkBgStructSize = 20,
    ScrWorkBgOffsetStructSize = 10,
    ScrWorkCaptureStructSize = 20,
    ScrWorkCaptureOffsetStructSize = 10,
    ScrWorkBgEffStructSize = 20,
    ScrWorkBgEffOffsetStructSize = 20,
};

include('common/scriptinput.lua');
include('common/scriptvars.lua');
include('sgps3/config.lua');
include('sgps3/scriptvars.lua');
include('sgps3/savedata.lua');
include('sgps3/tipssystem.lua');
include('sgps3/vfs.lua');
include('sgps3/sprites.lua');
include('common/animation.lua');
include('sgps3/charset.lua');
include('sgps3/font.lua');
include('sgps3/dialogue.lua');
include('sgps3/hud/saveicon.lua');
include('sgps3/hud/loadingdisplay.lua');
include('sgps3/hud/datedisplay.lua');
include('sgps3/hud/titlemenu.lua');
--include('sgps3/hud/systemmenu.lua');
include('sgps3/hud/backlogmenu.lua');
include('sgps3/hud/sysmesboxdisplay.lua');
include('sgps3/hud/selectiondisplay.lua');
include('sgps3/hud/tipsmenu.lua');
include('sgps3/hud/tipsnotification.lua');

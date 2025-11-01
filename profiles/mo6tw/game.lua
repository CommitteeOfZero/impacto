root.ActiveRenderer = RendererType.OpenGL;

root.LayerCount = 100;
root.GameFeatures = GameFeature.Sc3VirtualMachine | GameFeature.Renderer2D | GameFeature.Input | GameFeature.Audio | GameFeature.Video;
root.DesignWidth = 1280;
root.DesignHeight = 720;

root.WindowName = "Memories Off 6 ~T-Wave~";
root.WindowIconPath = "profiles/mo6tw/icon.png";
root.CursorArrowPath = "profiles/mo6tw/cursor_arrow.png";
root.CursorPointerPath = "profiles/mo6tw/cursor_pointer.png";

root.CharaIsMvl = false;
root.LayFileBigEndian = true;
root.LayFileTexXMultiplier = 2048;
root.LayFileTexYMultiplier = 1024;
root.UseScreenCapEffects = false;

root.Vm = {
    StartScript=0,
    StartScriptBuffer=0,
    GameInstructionSet=InstructionSet.MO6TW,
    UseReturnIds=false,
    RestartMaskUsesThreadAlpha=true,

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
include('mo6tw/config.lua');
include('mo6tw/scriptinput.lua');
include('mo6tw/scriptvars.lua');
include('mo6tw/savedata.lua');
include('mo6tw/tipssystem.lua');
include('mo6tw/vfs.lua');
include('mo6tw/sprites.lua');
include('common/animation.lua');
include('mo6tw/charset.lua');
include('mo6tw/font.lua');
include('mo6tw/dialogue.lua');
include('mo6tw/hud/saveicon.lua');
include('mo6tw/hud/loadingdisplay.lua');
include('mo6tw/hud/datedisplay.lua');
include('mo6tw/hud/titlemenu.lua');
include('mo6tw/hud/systemmenu.lua');
include('mo6tw/hud/savemenu.lua');
include('mo6tw/hud/sysmesboxdisplay.lua');
include('mo6tw/hud/selectiondisplay.lua');
include('mo6tw/hud/backlogmenu.lua');
include('mo6tw/hud/optionsmenu.lua');
include('mo6tw/hud/tipsmenu.lua');
include('mo6tw/hud/extramenus.lua');
include('mo6tw/hud/tipsnotification.lua');

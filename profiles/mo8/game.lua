root.ActiveRenderer = RendererType.OpenGL;

root.LayerCount = 100;
root.GameFeatures = GameFeature.Sc3VirtualMachine | GameFeature.Renderer2D | GameFeature.Input | GameFeature.Audio | GameFeature.Video | GameFeature.DebugMenu;
root.DesignWidth = 1920;
root.DesignHeight = 1080;

root.WindowName = "Memories Off -Innocent Fille-";
root.WindowIconPath = "profiles/mo8/icon.png";
root.CursorArrowPath = "profiles/mo8/cursor_arrow.png";
root.CursorPointerPath = "profiles/mo8/cursor_pointer.png";

root.CharaIsMvl = true;
root.UseMoviePriority = true;

root.Vm = {
    StartScript = 7,
    StartScriptBuffer = 0,
    GameInstructionSet = InstructionSet.MO8,
    UseReturnIds = true,
    UseMsbStrings = true,

    ScrWorkChaStructSize = 40,
    ScrWorkChaOffsetStructSize = 10,
    ScrWorkBgStructSize = 40,
    ScrWorkBgOffsetStructSize = 10,
    ScrWorkCaptureStructSize = 20,
    ScrWorkCaptureOffsetStructSize = 10,
    ScrWorkBgEffStructSize = 30,
    ScrWorkBgEffOffsetStructSize = 20,

    MaxLinkedBgBuffers = 2
};

include('common/scriptinput.lua');
include('common/scriptvars.lua');
include('mo8/config.lua');
include('mo8/scriptvars.lua');
include('mo8/savedata.lua');
include('mo8/tipssystem.lua');
include('mo8/vfs.lua');
include('mo8/sprites.lua');
include('common/animation.lua');
include('mo8/charset.lua');
include('mo8/font.lua');
--include('mo8/font-lb.lua');
include('mo8/dialogue.lua');
include('mo8/hud/saveicon.lua');
include('mo8/hud/loadingdisplay.lua');
include('mo8/hud/datedisplay.lua');
include('mo8/hud/titlemenu.lua');
include('mo8/hud/systemmenu.lua');
include('mo8/hud/backlogmenu.lua');
include('mo8/hud/optionsmenu.lua');
include('mo8/hud/savemenu.lua');
include('mo8/hud/sysmesboxdisplay.lua');
include('mo8/hud/selectiondisplay.lua');
include('mo8/hud/tipsmenu.lua');
include('mo8/hud/tipsnotification.lua');
root.ActiveRenderer = RendererType.OpenGL;

root.LayerCount = 100;
root.GameFeatures = GameFeature.Sc3VirtualMachine | GameFeature.Renderer2D | GameFeature.Input | GameFeature.Audio |
                        GameFeature.Video | GameFeature.DebugMenu;
root.DesignWidth = 1920;
root.DesignHeight = 1080;

root.WindowName = "CHAOS;CHILD Love Chu☆Chu!!";
root.WindowIconPath = "profiles/cclcc/icon.png";
root.CursorArrowPath = "profiles/cclcc/cursor_arrow.png";
root.CursorPointerPath = "profiles/cclcc/cursor_pointer.png";

root.CharaIsMvl = true;
root.UseMoviePriority = true;
root.UseBgChaEffects = true;
root.UseBgFrameEffects = false;

root.Vm = {
    StartScript = 1,
    StartScriptBuffer = 0,
    GameInstructionSet = InstructionSet.CC,
    UseReturnIds = true,

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

root.PlatformId = 131072;

include('common/scriptinput.lua');
include('common/scriptvars.lua');

include('cclcc/scriptinput.lua');
include('cclcc/scriptvars.lua');
include('cclcc/config.lua');
include('cclcc/sprites.lua');
include('cclcc/savedata.lua');
include('cclcc/tipssystem.lua');
include('cclcc/vfs.lua');
include('cclcc/mapsystem.lua');
include('cclcc/yesnotrigger.lua');
include('common/animation.lua');
include('cclcc/charset.lua');
include('common/charset.lua');
--include('cclcc/font.lua');
--include('cclcc/font-lb.lua');
include('cclcc/font-lb-italic.lua');
include('cclcc/dialogue.lua');
include('cclcc/configsystem.lua');
include('cclcc/bgeff.lua');
include('cclcc/hud/saveicon.lua');
include('cclcc/hud/loadingdisplay.lua');
include('cclcc/hud/datedisplay.lua');
include('cclcc/hud/titlemenu.lua');
include('cclcc/hud/backlogmenu.lua');
include('cclcc/hud/sysmesboxdisplay.lua');
include('cclcc/hud/selectiondisplay.lua');
include('cclcc/hud/optionsmenu.lua');
include('cclcc/hud/tipsmenu.lua');
include('cclcc/hud/extramenus.lua');
include('cclcc/hud/tipsnotification.lua');
include('cclcc/hud/delusiontrigger.lua');
include('cclcc/hud/systemmenu.lua');
include('cclcc/hud/savemenu.lua');
include('cclcc/hud/helpmenu.lua');
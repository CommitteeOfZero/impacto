root.ActiveRenderer = RendererType.OpenGL;

root.LayerCount = 100;
root.GameFeatures = GameFeature.Sc3VirtualMachine | GameFeature.Renderer2D | GameFeature.Scene3D | GameFeature.Input | GameFeature.Audio | GameFeature.Video;
root.DesignWidth = 1920;
root.DesignHeight = 1080;

root.WindowName = "ROBOTICS;NOTES DaSH";
root.WindowIconPath = "profiles/dash/icon.png";
root.CursorArrowPath = "profiles/dash/cursor_arrow.png";
root.CursorPointerPath = "profiles/dash/cursor_pointer.png";

root.Vm = {
    StartScript = 4,
    StartScriptBuffer = 0,
    GameInstructionSet = InstructionSet.Dash,
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

include('common/scriptinput.lua');
include('common/scriptvars.lua');
include('dash/config.lua');
include('dash/scriptinput.lua');
include('dash/scriptvars.lua');
include('dash/savedata.lua');
include('dash/tipssystem.lua');
include('dash/vfs.lua');
include('dash/sprites.lua');
include('common/animation.lua');
include('dash/charset.lua');
--include('dash/font.lua');
include('dash/font-lb.lua');
include('dash/dialogue.lua');
include('dash/hud/saveicon.lua');
include('dash/hud/loadingdisplay.lua');
include('dash/hud/datedisplay.lua');
include('dash/hud/titlemenu.lua');
include('dash/hud/systemmenu.lua');
include('dash/hud/backlogmenu.lua');
include('dash/hud/sysmesboxdisplay.lua');
include('dash/scene3d/scene3d.lua');
include('dash/hud/selectiondisplay.lua');
include('dash/hud/tipsmenu.lua');
include('dash/hud/tipsnotification.lua');
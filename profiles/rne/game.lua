root.ActiveRenderer = RendererType.OpenGL;

root.LayerCount = 100;
root.GameFeatures = GameFeature.Sc3VirtualMachine | GameFeature.Renderer2D | GameFeature.Scene3D | GameFeature.Input | GameFeature.Audio | GameFeature.Video;
root.DesignWidth = 1280;
root.DesignHeight = 720;

root.WindowName = "ROBOTICS;NOTES ELITE";
root.WindowIconPath = "profiles/rne/icon.png";
root.CursorArrowPath = "profiles/rne/cursor_arrow.png";
root.CursorPointerPath = "profiles/rne/cursor_pointer.png";

root.Vm = {
    StartScript = 4,
    StartScriptBuffer = 0,
    GameInstructionSet = InstructionSet.RNE,
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

include('common/scriptinput.lua')
include('common/scriptvars.lua');
include('rne/config.lua');
include('rne/savedata.lua');
include('rne/tipssystem.lua');
include('rne/vfs.lua');
include('rne/sprites.lua');
include('common/animation.lua');
include('rne/charset.lua');
--include('rne/font.lua');
include('rne/font-lb.lua');
include('rne/dialogue.lua');
include('rne/hud/saveicon.lua');
include('rne/hud/loadingdisplay.lua');
include('rne/hud/datedisplay.lua');
include('rne/hud/titlemenu.lua');
include('rne/hud/systemmenu.lua');
include('rne/hud/backlogmenu.lua');
include('rne/hud/sysmesboxdisplay.lua');
include('rne/scene3d/scene3d.lua');
include('rne/hud/selectiondisplay.lua');
include('rne/hud/tipsmenu.lua');
include('rne/hud/tipsnotification.lua');
root.ActiveRenderer = RendererType.OpenGL;

root.LayerCount = 100;
root.GameFeatures = GameFeature.Sc3VirtualMachine | GameFeature.Renderer2D | GameFeature.Input | GameFeature.Audio | GameFeature.Video;
root.DesignWidth = 1920;
root.DesignHeight = 1080;

root.WindowName = "CHAOS;HEAD NOAH";

root.CharaIsMvl = true;

root.Vm = {
    StartScript = 1,
    StartScriptBuffer = 0,
    GameInstructionSet = InstructionSet.CHN,
    UseReturnIds = true,
    UseMsbStrings = true,
    UseSeparateMsbArchive = true,
    ScrWorkChaStructSize = 40,
    ScrWorkBgStructSize = 40,
    ScrWorkCaptureStructSize = 20,
    ScrWorkBgEffStructSize = 30,
    MaxLinkedBgBuffers = 2
};

include('common/scriptinput.lua');
include('common/scriptvars.lua');
include('chn/config.lua');
include('chn/scriptvars.lua');
include('chn/savedata.lua');
include('chn/tipssystem.lua');
include('chn/vfs.lua');
include('chn/sprites.lua');
include('common/animation.lua');
include('chn/charset.lua');
include('chn/font.lua');
--include('chn/font-lb.lua');
include('chn/dialogue.lua');
include('chn/hud/saveicon.lua');
include('chn/hud/loadingdisplay.lua');
include('chn/hud/datedisplay.lua');
include('chn/hud/titlemenu.lua');
--include('chn/hud/systemmenu.lua');
include('chn/hud/backlogmenu.lua');
include('chn/hud/sysmesboxdisplay.lua');
include('chn/hud/selectiondisplay.lua');
include('chn/hud/tipsmenu.lua');
include('chn/hud/tipsnotification.lua');
root.ActiveRenderer = RendererType.OpenGL;

root.LayerCount = 100;
root.GameFeatures = GameFeature.Sc3VirtualMachine | GameFeature.Renderer2D | GameFeature.Input | GameFeature.Audio | GameFeature.Nuklear | GameFeature.Video;
root.DesignWidth = 1920;
root.DesignHeight = 1080;

root.WindowName = "CHAOS;CHILD Love Chu☆Chu!!";

root.CharaIsMvl = true;

root.Vm = {
    StartScript = 1,
    StartScriptBuffer = 0,
    GameInstructionSet = InstructionSet.CC,
    UseReturnIds = true,
    ScrWorkChaStructSize = 40,
    ScrWorkBgStructSize = 40,
    MaxLinkedBgBuffers = 2
};

include('common/scriptinput.lua');
include('common/scriptvars.lua');
include('cclcc/scriptvars.lua');
include('cclcc/savedata.lua');
include('cclcc/tipssystem.lua');
include('cclcc/vfs.lua');
include('cclcc/sprites.lua');
include('common/animation.lua');
include('cclcc/charset.lua');
--include('cclcc/font.lua');
include('cclcc/font-lb.lua');
include('cclcc/dialogue.lua');
include('cclcc/hud/saveicon.lua');
include('cclcc/hud/loadingdisplay.lua');
include('cclcc/hud/datedisplay.lua');
include('cclcc/hud/titlemenu.lua');
--include('cclcc/hud/systemmenu.lua');
include('cclcc/hud/backlogmenu.lua');
include('cclcc/hud/sysmesboxdisplay.lua');
include('cclcc/hud/selectiondisplay.lua');
include('cclcc/hud/optionsmenu.lua');
include('cclcc/hud/tipsmenu.lua');
include('cclcc/hud/extramenus.lua');
include('cclcc/hud/tipsnotification.lua');
include('cclcc/mapsystem.lua');
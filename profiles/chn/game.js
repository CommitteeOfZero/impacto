root.ActiveRenderer = RendererType.OpenGL;

root.LayerCount = 100;
root.GameFeatures = GameFeature.Sc3VirtualMachine | GameFeature.Renderer2D | GameFeature.Input | GameFeature.Audio | GameFeature.Nuklear | GameFeature.Video;
root.DesignWidth = 1920;
root.DesignHeight = 1080;

root.WindowName = "CHAOS;HEAD NOAH";

root.CharaIsMvl = true;

root.Vm = {
    StartScript: 1,
    StartScriptBuffer: 0,
    GameInstructionSet: InstructionSet.CHN,
	UseReturnIds: true,
    UseMsbStrings: true,
    UseSeparateMsbArchive: true,
	ScrWorkChaStructSize: 40,
	ScrWorkBgStructSize: 40,
    MaxLinkedBgBuffers: 2
};

include('common/scriptinput.js');
include('common/scriptvars.js');
include('chn/scriptvars.js');
include('chn/savedata.js');
include('chn/tipssystem.js');
include('chn/vfs.js');
include('chn/sprites.js');
include('common/animation.js');
include('chn/charset.js');
include('chn/font.js');
//include('chn/font-lb.js');
include('chn/dialogue.js');
include('chn/hud/saveicon.js');
include('chn/hud/loadingdisplay.js');
include('chn/hud/datedisplay.js');
include('chn/hud/titlemenu.js');
//include('chn/hud/systemmenu.js');
include('chn/hud/backlogmenu.js');
include('chn/hud/sysmesboxdisplay.js');
include('chn/hud/selectiondisplay.js');
include('chn/hud/tipsmenu.js');
include('chn/hud/tipsnotification.js');
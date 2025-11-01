root.ActiveRenderer = RendererType.OpenGL;

root.LayerCount = 100;
root.GameFeatures = GameFeature.Sc3VirtualMachine | GameFeature.Renderer2D | GameFeature.Input | GameFeature.Audio | GameFeature.Video | GameFeature.DebugMenu;
root.DesignWidth = 1280;
root.DesignHeight = 720;

root.WindowName = "CHAOS;HEAD Love Chu☆Chu!";
root.WindowIconPath = "profiles/chlcc/icon.png";
root.CursorArrowPath = "profiles/chlcc/cursor_arrow.png";
root.CursorPointerPath = "profiles/chlcc/cursor_pointer.png";

root.CharaIsMvl = false;
root.UseMoviePriority = true;
root.LayFileBigEndian = true;
root.LayFileTexXMultiplier = 2048;
root.LayFileTexYMultiplier = 1024;

root.Vm = {
    StartScript = 0,
    StartScriptBuffer = 0,
    GameInstructionSet = InstructionSet.CHLCC,
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
include('chlcc/config.lua');
include('chlcc/scriptinput.lua');
include('chlcc/scriptvars.lua');
include('chlcc/savedata.lua');
include('chlcc/achievementsystem.lua');
include('chlcc/tipssystem.lua');
include('chlcc/vfs.lua');
include('chlcc/sprites.lua');
include('common/animation.lua');
include('chlcc/charset.lua');
include('common/charset.lua');
if root.Language == "Japanese" then
    include('chlcc/font.lua');
else
    include('chlcc/font-lb.lua');
end
include('chlcc/dialogue.lua');
include('chlcc/hud/saveicon.lua');
include('chlcc/hud/loadingdisplay.lua');
include('chlcc/hud/datedisplay.lua');
include('chlcc/hud/titlemenu.lua');
include('chlcc/hud/backlogmenu.lua');
include('chlcc/hud/systemmenu.lua');
include('chlcc/hud/savemenu.lua');
include('chlcc/hud/sysmesboxdisplay.lua');
include('chlcc/hud/selectiondisplay.lua');
include('chlcc/hud/tipsmenu.lua');
include('chlcc/hud/tipsnotification.lua');
include('chlcc/hud/optionsmenu.lua');
include('chlcc/hud/extramenus.lua');
include('chlcc/hud/trophymenu.lua');
include('chlcc/hud/delusiontrigger.lua');

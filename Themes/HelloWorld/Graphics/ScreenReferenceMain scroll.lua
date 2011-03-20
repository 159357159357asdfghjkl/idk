local gc = Var("GameCommand")
local itemName = gc:GetName()

local t = Def.ActorFrame{
	-- focused frame
	LoadActor( THEME:GetPathG("SectionMenu","ItemFocused") )..{
		GainFocusCommand=cmd(decelerate,0.5;diffusealpha,1);
		LoseFocusCommand=cmd(stoptweening;decelerate,0.5;diffusealpha,0);
	};
	-- unfocused
	LoadActor( THEME:GetPathG("SectionMenu","ItemUnfocused") )..{
		GainFocusCommand=cmd(decelerate,0.5;diffusealpha,0);
		LoseFocusCommand=cmd(stoptweening;decelerate,0.5;diffusealpha,1);
	};

	LoadActor( THEME:GetPathG("_section", "Reference") )..{
		InitCommand=cmd(x,-250;y,-24;Real;halign,0);
		GainFocusCommand=cmd(decelerate,0.5;diffusealpha,0.9);
		LoseFocusCommand=cmd(stoptweening;decelerate,0.5;diffusealpha,0.45);
	};

	Def.ActorFrame{
		LoadFont("_frutiger roman 24px")..{
			Name="SectionName";
			Text=THEME:GetString("ScreenReferenceMain",itemName);
			InitCommand=cmd(x,-250;y,-6;halign,0;diffuse,color("#222222FF"));
			GainFocusCommand=cmd(decelerate,0.5;diffusealpha,1);
			LoseFocusCommand=cmd(stoptweening;decelerate,0.5;diffusealpha,0.5);
		};
		LoadFont("_frutiger roman 24px")..{
			Name="SectionDesc";
			Text=THEME:GetString("ScreenReferenceMain",itemName.."Desc");
			InitCommand=cmd(x,-248;y,10;align,0,0;diffuse,color("#222222FF");zoom,0.5);
			GainFocusCommand=cmd(decelerate,0.5;diffusealpha,1);
			LoseFocusCommand=cmd(stoptweening;decelerate,0.5;diffusealpha,0.5);
		};
	};
};

return t;
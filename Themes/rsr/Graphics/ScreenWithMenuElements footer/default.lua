local t = Def.ActorFrame {};
t[#t+1] = Def.ActorFrame {
	InitCommand=cmd(y,-64);
	Def.Quad {
		InitCommand=cmd(vertalign,top;zoomto,SCREEN_WIDTH,64);
		OnCommand=cmd(diffuse,Color.Black;diffusealpha,0.75);
	};
};
return t
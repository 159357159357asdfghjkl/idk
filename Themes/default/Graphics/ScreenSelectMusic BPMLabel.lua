return Def.ActorFrame {
	LoadFont("Common Normal") .. {
		Text="BPM";
		InitCommand=cmd(horizalign,right;zoom,0.75;strokecolor,Color("Outline"));
		SetCommand=function(self)
			local bIsFirst = false;
			local song = GAMESTATE:GetCurrentSong();
			self:stoptweening();
-- 			self:linear(0.25);
			if song then
				self:diffusebottomedge( song:GetTimingData():HasStops() and Color("Orange") or Color("White") );
			else
				self:diffusebottomedge( Color("White") );
			end;
		end;
		CurrentSongChangedMessageCommand=cmd(playcommand,"Set");
		CurrentCourseChangedMessageCommand=cmd(playcommand,"Set");
	};
};
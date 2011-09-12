-- sm-ssc fallback theme | script ring 03 | Gameplay.lua
-- [en] This file is used to store settings that should be different in each
-- game mode.

-- shakesoda calls this pump.lua

-- GetExtraColorThreshold()
-- [en] returns the difficulty threshold in meter
-- for songs that should be counted as boss songs.
function GetExtraColorThreshold()
	sGame = GAMESTATE:GetCurrentGame():GetName()
	local Modes = {
		dance = 10,
		pump = 21,
		beat = 12,
		kb7 = 10,
		para = 10,
		techno = 10,
		lights = 10, -- lights shouldn't be playable
	}
	return Modes[sGame]
end

-- GameCompatibleModes:
-- [en] returns possible modes for ScreenSelectPlayMode
function GameCompatibleModes()
	sGame = GAMESTATE:GetCurrentGame():GetName()
	local Modes = {
		dance = "Single,Double,Solo,Versus,Couple",
		pump = "Single,Double,HalfDouble,Versus,Couple,Routine",
		beat = "5Keys,7Keys,10Keys,14Keys,Versus5,Versus7",
		kb7 = "KB7",
		para = "Single",
		techno = "Single4,Single5,Single8,Double4,Double8",
		lights = "Single" -- lights shouldn't be playable
	}
	return Modes[sGame]
end

function SelectProfileKeys()
	local sGame = GAMESTATE:GetCurrentGame():GetName()	
	if sGame == "pump" then
		return "Up,Down,Start,Back,Center,DownLeft,DownRight"
	elseif sGame == "dance" then
		return "Up,Down,Start,Back,Up2,Down2"
	else
		return "Up,Down,Start,Back"
	end
end

-- ScoreKeeperClass:
-- [en] Determines the correct ScoreKeeper class to use.
function ScoreKeeperClass()
	sGame = GAMESTATE:GetCurrentGame():GetName()
	local ScoreKeepers = {
		-- xxx: allow for ScoreKeeperShared when needed
		dance = "ScoreKeeperNormal",
		pump = "ScoreKeeperNormal",
		beat = "ScoreKeeperNormal",
		kb7 = "ScoreKeeperNormal",
		para = "ScoreKeeperNormal",
		techno = "ScoreKeeperNormal",
		ez2 = "ScoreKeeperNormal",
		ds3ddx = "ScoreKeeperNormal",
		maniax = "ScoreKeeperNormal"
	}
	return ScoreKeepers[sGame]
end

-- ComboContinue:
-- [en] 
function ComboContinue()
	sGame = GAMESTATE:GetCurrentGame():GetName()
	local Continue = {
		dance = GAMESTATE:GetPlayMode() == "PlayMode_Oni" and "TapNoteScore_W2" or "TapNoteScore_W3",
		pump = "TapNoteScore_W3",
		beat = "TapNoteScore_W3",
		kb7 = "TapNoteScore_W3",
		para = "TapNoteScore_W4"
	}
	return Continue[sGame]
end

function ComboMaintain()
	sGame = GAMESTATE:GetCurrentGame():GetName()
	local Maintain = {
		dance = "TapNoteScore_W3",
		pump = "TapNoteScore_W4",
		beat = "TapNoteScore_W3",
		kb7 = "TapNoteScore_W3",
		para = "TapNoteScore_W4"
	}
	return Maintain[sGame]
end

function ComboPerRow()
	sGame = GAMESTATE:GetCurrentGame():GetName()
	if sGame == "pump" then
		return true
	elseif GAMESTATE:GetPlayMode() == "PlayMode_Oni" then
		return true
	else
		return false
	end
end

function EvalUsesCheckpointsWithJudgments()
	return (GAMESTATE:GetCurrentGame():GetName() == "pump") and true or false
end

-- these need cleanup really.
function HitCombo()
	sGame = GAMESTATE:GetCurrentGame():GetName()
	local Combo = {
		dance = 2,
		pump = 4,
		beat = 2,
		kb7 = 2,
		para = 2
	}
	return Combo[sGame]
end

function MissCombo()
	sGame = GAMESTATE:GetCurrentGame():GetName()
	local Combo = {
		dance = 2,
		pump = 4,
		beat = 0,
		kb7 = 0,
		para = 0
	}
	return Combo[sGame]
end

-- FailCombo:
-- [en] The combo that causes game failure.
function FailCombo()
	sGame = GAMESTATE:GetCurrentGame():GetName()
	local Combo = {
		dance = -1, -- ITG uses 30
		pump = 51, -- Pump Pro uses 30, real Pump uses 51
		beat = -1,
		kb7 = -1,
		para = -1
	}
	return Combo[sGame]
end

function RoutineSkinP1()
	sGame = GAMESTATE:GetCurrentGame():GetName()
	local Combo = {
		dance = "midi-routine-p1",
		pump = "cmd-routine-p1",
		beat = "default",
		kb7 = "default",
		para = "default"
	}
	return Combo[sGame]
end

function RoutineSkinP2()
	sGame = GAMESTATE:GetCurrentGame():GetName()
	local Combo = {
		dance = "midi-routine-p2",
		pump = "cmd-routine-p2",
		beat = "default",
		kb7 = "retrobar",
		para = "default"
	}
	return Combo[sGame]
end

-- todo: use tables for some of these -aj
function HoldTiming()
	if GAMESTATE:GetCurrentGame():GetName() == "pump" then
		return 0
	else
		return PREFSMAN:GetPreference("TimingWindowSecondsHold")
	end
end

function ShowHoldJudgments()
	return not GAMESTATE:GetCurrentGame():GetName() == "pump"
end

function HoldHeadStep()
	return not GAMESTATE:GetCurrentGame():GetName() == "pump"
end

function InitialHoldLife()
	return GAMESTATE:GetCurrentGame():GetName() == "pump" and 0.05 or 1
end

function MaxHoldLife()
	return GAMESTATE:GetCurrentGame():GetName() == "pump" and 0.05 or 1
end

function ImmediateHoldLetGo()
	return not GAMESTATE:GetCurrentGame():GetName() == "pump"
end

function RollBodyIncrementsCombo()
	return false
--[[ 	if GAMESTATE:GetCurrentGame():GetName() == "pump" then
		return false
	else
		return true
	end --]]
end

function CheckpointsTapsSeparateJudgment()
	return not GAMESTATE:GetCurrentGame():GetName() == "pump"
end

function ScoreMissedHoldsAndRolls()
	return not GAMESTATE:GetCurrentGame():GetName() == "pump"
end

local tNotePositions = {
	-- StepMania 3.9/4.0
	Normal = { -144, 144, },
	-- ITG
	Lower = { -125, 145, }
}

function GetTapPosition( sType )
	bCategory = (sType == 'Standard') and 1 or 2
	-- true: Normal
	-- false: Lower
	bPreference = GetUserPrefB("UserPrefNotePosition") and "Normal" or "Lower"
	tNotePos = tNotePositions[bPreference]
	return tNotePos[bCategory]
end

function ComboUnderField()
	return GetUserPrefB("UserPrefComboUnderField")
end

function TwoPartSelection()
	return GAMESTATE:GetCurrentGame():GetName() == "pump"
end

function TwoPartConfirmsOnly()
	return GAMESTATE:GetCurrentGame():GetName() == "pump"
end

local CodeDetectorCodes = {
	-- steps
	PrevSteps1 = {
		default = "",
		dance = "Up,Up",
		pump = "+UpLeft",
	},
	PrevSteps2 = {
		default = "MenuUp,MenuUp",
		dance = "MenuUp,MenuUp",
		pump = "",
	},
	NextSteps1 = {
		default = "",
		dance = "Down,Down",
		pump = "+UpRight",
	},
	NextSteps2 = {
		default = "MenuDown,MenuDown",
		dance = "MenuDown,MenuDown",
		pump = "",
	},
	-- group
	NextGroup = {
		default = "",
		dance = "MenuUp,MenuRight,MenuRight",
		pump = "",
	},
	PrevGroup = {
		default = "",
		dance = "MenuUp,MenuDown,MenuUp,MenuDown",
		pump = "",
	},
	CloseCurrentFolder = {
		default = "MenuUp-MenuDown",
	},
	-- sorts
	NextSort1 = {
		default = "@MenuLeft-@MenuRight-Start",
		dance = "@MenuLeft-@MenuRight-Start",
		pump = "@MenuLeft-@MenuRight-Start",
	},
	NextSort2 = {
		default = "MenuLeft-MenuRight",
		dance = "MenuLeft-MenuRight",
		pump = "MenuLeft-MenuRight",
	},
	NextSort3 = {
		default = "",
		dance = "@Left-@Right-Start",
		pump = "@DownLeft-@DownRight-Start",
	},
	NextSort4 = {
		default = "",
		dance = "Left-Right",
		pump = "DownLeft-DownRight",
	},
	-- modemenu
	ModeMenu1 = {
		default = "",
		dance = "Up,Down,Up,Down",
	},
	ModeMenu2 = {
		default = "MenuUp,MenuDown,MenuUp,MenuDown",
	},
	-- Evaluation:
	SaveScreenshot1 = {
		default = "MenuLeft-MenuRight",
	},
	SaveScreenshot2 = {
		default = "Select",
	},
	-- modifiers section
	CancelAll = {
		default = "",
		dance = "Left,Right,Left,Right,Left,Right,Left,Right",
	},
	--- specific modifiers
	Mirror = {
		default = "",
		dance = "Up,Left,Right,Left,Right",
		pump = "DownRight,DownLeft,UpRight,UpLeft,DownRight,DownLeft,UpRight,UpLeft,Center",
	},
	Left = {
		default = "",
		dance = "Up,Down,Right,Left",
	},
	Right = {
		default = "",
		dance = "Up,Down,Left,Right",
	},
	Shuffle = {
		default = "",
		dance = "Down,Up,Down,Up",
		pump = "UpLeft,UpRight,UpLeft,UpRight,DownLeft,DownRight,DownLeft,DownRight,Center", -- random
	},
	SuperShuffle = {
		default = "",
		dance = "Down,Up,Left,Right",
		pump = "UpLeft,UpRight,DownLeft,DownRight,UpLeft,UpRight,DownLeft,DownRight,Center"
	},
	Reverse = {
		default = "",
		dance = "Down,Left,Right,Left,Right",
		pump = "UpLeft,DownLeft,UpRight,DownRight,UpLeft,DownLeft,UpRight,DownRight,DownRight", -- drop
	},
	HoldNotes = {
		default = "",
		dance = "Right,Left,Down,Up",
	},
	Mines = {
		default = "",
	},
	Dark = {
		default = "",
	},
	Hidden = {
		default = "",
		pump = "UpLeft,UpRight,DownLeft,DownRight,Center", -- vanish
	},
	RandomVanish = {
		default = "",
	},
	-- boost (accel), brake (decel), stealth (nonstep)
	--- next/prev modifiers
	NextTransform = {
		default = "",
	},
	NextScrollSpeed = {
		default = "",
		dance = "Up,Left,Down,Left,Up",
		pump = "UpLeft,UpRight,UpLeft,UpRight,Center",
	},
	PreviousScrollSpeed = {
		default = "",
		dance = "Down,Right,Up,Right,Down",
		pump = "UpRight,UpLeft,UpRight,UpLeft,Center",
	},
	NextAccel = {
		default = "",
		dance = "Left,Right,Down,Up",
	},
	NextEffect = {
		default = "",
		dance = "Left,Down,Right",
	},
	NextAppearance = {
		default = "",
		dance = "Left,Up,Right",
	},
	NextTurn = {
		default = "",
	},
	-- cancel all in player options
	CancelAllPlayerOptions = {
		default = "",
		dance = "Left,Right,Left,Right,Left,Right",
	},
};

function GetCodeForGame(codeName)
	local gameName = string.lower(GAMESTATE:GetCurrentGame():GetName())
	local inputCode = CodeDetectorCodes[codeName]
	return inputCode[gameName] or inputCode["default"]
end
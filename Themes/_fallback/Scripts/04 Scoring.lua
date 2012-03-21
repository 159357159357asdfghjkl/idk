--internals table
local Shared = {};
--Special Scoring types.
local r = {};
local DisabledScoringModes = { '[SSC] Radar Master' };
--the following metatable makes any missing value in a table 0 instead of nil.
local ZeroIfNotFound = { __index = function() return 0 end; };

-- Retrieve the amount of taps/holds/rolls involved. Used for some formulas.
function GetTotalItems(radars)
	local total = radars:GetValue('RadarCategory_TapsAndHolds')
	total = total + radars:GetValue('RadarCategory_Holds') 
	total = total + radars:GetValue('RadarCategory_Rolls')
	-- [ja] Liftを加えると一部二重加算になるため除外する。
	-- total = total + radars:GetValue('RadarCategory_Lifts')

	-- [en] prevent divide by 0
	-- [ja] 0除算対策（しなくても動作するけど満点になっちゃうんで）
	return math.max(1,total);
end;

-- Determine whether marvelous timing is to be considered.
function IsW1Allowed(tapScore)
	return tapScore == 'TapNoteScore_W2'
		and (PREFSMAN:GetPreference("AllowW1") ~= 'AllowW1_Never' 
		or not (GAMESTATE:IsCourseMode() and 
		PREFSMAN:GetPreference("AllowW1") == 'AllowW1_CoursesOnly'));
end;

-- Get the radar values directly. The individual steps aren't used much.
function GetDirectRadar(player)
	return GAMESTATE:GetCurrentSteps(player):GetRadarValues(player);
end;

-----------------------------------------------------------
--StepMania 3.9 (Plus) Scorings by @803832
--3.9 MAX2
--3.9 5TH
--3.9Plus HYBRID
--3.9Plus SN
--3.9Plus SN2
-- note: 3.9Plus PIU Scoring cannot be implemented due to no NumTapsInRow
-- in JudgmentMessage. This is fixed in hanubeki-modified-sm-ssc revision 6c7184e7df9a:
-- http://code.google.com/r/hanubeki-modified-sm-ssc/source/detail?r=6c7184e7df9a3694649f965c2976859bb9e45963 
-----------------------------------------------------------
local function isW2OrAbove(tns)
	if tns == 'TapNoteScore_W1' or tns == 'TapNoteScore_W2' then
		return true;
	end;
	return false;
end
local sm3_Steps = {0,0};
local sm3_Score = {0,0};
local sm3_CurMaxScore = {0,0};
local sm3_Combo = {0,0};
local sm3_CurMaxCombo = {0,0};
local sm3_GradePoints = {0,0};
local sm3_Bonus = {0,0};
local sm3_MaxBonus = {0,0};

r["3.9 MAX2"] = function(params, pss)
	local multLookup =
	{
		['TapNoteScore_W1'] = 10,
		['TapNoteScore_W2'] = GAMESTATE:ShowW1() and 9 or 10,
		['TapNoteScore_W3'] = 5
	};
	setmetatable(multLookup, ZeroIfNotFound);
	local radarValues = GetDirectRadar(params.Player);
	local totalItems = GetTotalItems(radarValues);
	-- 1+2+3+...+totalItems value/の値
	local sTotal = (totalItems+1)*totalItems/2;
	local steps = GAMESTATE:GetCurrentSteps(params.Player);
	local meter = steps:GetMeter();
	meter = clamp(meter,1,10);

	-- [ja] 満点を求める
	local length = 1;
	if (GAMESTATE:GetCurrentSong():IsMarathon()) then
		length = 3;
	elseif (GAMESTATE:GetCurrentSong():IsLong()) then
		length = 2;
	end;
	local baseScore = meter * 10000000 * length;

	local sOne = baseScore/sTotal;

	local p = (params.Player == 'PlayerNumber_P1') and 1 or 2;

	-- [ja] スコアが0の時に初期化
	if pss:GetScore()==0 then
		sm3_Steps[p] = 0;
	end;

	-- [ja] 現在のステップ数
	-- [ja] 地雷とHoldCheckpointsの場合は加算しない
	if params.TapNoteScore ~= "TapNoteScore_HitMine" and
	   params.TapNoteScore ~= "TapNoteScore_AvoidMine" and
	   params.TapNoteScore ~= "TapNoteScore_CheckpointHit" and
	   params.TapNoteScore ~= "TapNoteScore_CheckpointMiss"
	then
		sm3_Steps[p]=sm3_Steps[p]+1;
	end;

	-- [en] current score
	-- [ja] 今回加算するスコア（W1の時）
	local vScore = sOne*sm3_Steps[p];
	pss:SetCurMaxScore(pss:GetCurMaxScore()+math.floor(vScore));
	-- [ja] 判定によって加算量を変更
	if (params.HoldNoteScore == 'HoldNoteScore_Held') then
		vScore = vScore;
	else
		if (params.HoldNoteScore == 'HoldNoteScore_LetGo') then
			vScore = 0;
		else
			vScore = vScore*multLookup[params.TapNoteScore]/10;
		end;
	end;

	-- [ja] 落ちていた場合は入るスコアが減る
	if (pss:GetFailed()) then
		if (params.HoldNoteScore == 'HoldNoteScore_Held') then
			-- [ja] O.K.判定時は10点
			sm3_Score[p] = sm3_Score[p]+10;
		elseif (params.HoldNoteScore ~= 'HoldNoteScore_LetGo') then
			-- [ja] O.K.でもN.G.でもない場合のスコア：
			-- W1 (Marvelous): 10
			-- W2 (Perfect):    9
			-- W3 (Great):      5
			sm3_Score[p] = sm3_Score[p]+multLookup[params.TapNoteScore];
		end;

		-- [ja] 5点単位の処理がなぜかここでされる
		if multLookup[params.TapNoteScore] > 0 then
			sm3_Score[p] = math.floor((sm3_Score[p] + 2) / 5) * 5;
		end;
	else
		pss:SetScore(pss:GetScore()+math.floor(vScore));
	end;

	if (sm3_Steps[p] == totalItems and not pss:GetFailed()) then
		-- [ja] 最後の1ステップがW2以上の場合、端数を加算する
		-- [ja] 端数は設定上の満点と最終ステップ時点での満点の差から求める
		if (isW2OrAbove(params.TapNoteScore) or params.HoldNoteScore == 'HoldNoteScore_Held') then
			pss:SetScore(pss:GetScore()+(baseScore-pss:GetCurMaxScore()));
			pss:SetCurMaxScore(pss:GetCurMaxScore()+(baseScore-pss:GetCurMaxScore()));
		end;
	end;
end;

-- TODO: Don't add combo bonus if autoplay
r["3.9 5TH"] = function(params, pss)
	local multLookup =
	{
		['TapNoteScore_W1'] = 10,
		['TapNoteScore_W2'] = GAMESTATE:ShowW1() and 9 or 10,
		['TapNoteScore_W3'] = 5
	};
	setmetatable(multLookup, ZeroIfNotFound);
	local radarValues = GetDirectRadar(params.Player);
	local totalItems = GetTotalItems(radarValues);
	-- 1+2+3+...+totalItems value/の値
	local sTotal = (totalItems+1)*totalItems/2;
	local steps = GAMESTATE:GetCurrentSteps(params.Player);
	local meter = steps:GetMeter();
	meter = clamp(meter,1,10);

	-- [ja] 満点を求める
	local length = 1;
	if (GAMESTATE:GetCurrentSong():IsMarathon()) then
		length = 3;
	elseif (GAMESTATE:GetCurrentSong():IsLong()) then
		length = 2;
	end;
	local baseScore = (meter * length + 1) * 5000000;

	local sOne = baseScore/sTotal;

	local p = (params.Player == 'PlayerNumber_P1') and 1 or 2;

	-- [ja] スコアが0の時に初期化
	if pss:GetScore()==0 then
		sm3_Steps[p] = 0;
		sm3_Score[p] = 0;
		sm3_CurMaxScore[p] = 0;
		sm3_Combo[p] = 0;
		sm3_CurMaxCombo[p] = 0;
		sm3_GradePoints[p] = 0;
		sm3_Bonus[p] = 0;
		sm3_MaxBonus[p] = 0;
	end;

	-- [ja] 現在のステップ数
	-- [ja] 地雷とHoldCheckpointsの場合は加算しない
	if params.TapNoteScore ~= "TapNoteScore_HitMine" and
	   params.TapNoteScore ~= "TapNoteScore_AvoidMine" and
	   params.TapNoteScore ~= "TapNoteScore_CheckpointHit" and
	   params.TapNoteScore ~= "TapNoteScore_CheckpointMiss"
	then
		sm3_Steps[p]=sm3_Steps[p]+1;
	end;

	-- [en] current score
	-- [ja] 今回加算するスコア（W1の時）
	local vScore = sOne*sm3_Steps[p];
	sm3_CurMaxScore[p] = sm3_CurMaxScore[p]+math.floor(vScore);

	-- [ja] グレードの計算は5THではなくMAX2方式
	local gradeWeightTable = {
		['TapNoteScore_W1'] = 2,
		['TapNoteScore_W2'] = 2,
		['TapNoteScore_W3'] = 1,
		['TapNoteScore_W5'] = -4,
		['TapNoteScore_Miss'] = -8,
	};
	setmetatable(gradeWeightTable, ZeroIfNotFound);

	-- [ja] 判定によって加算量を変更
	if (params.HoldNoteScore == 'HoldNoteScore_Held') then
		vScore = vScore;
		sm3_GradePoints[p] = sm3_GradePoints[p] + 6;
	else
		if (params.HoldNoteScore == 'HoldNoteScore_LetGo') then
			vScore = 0;
		else
			vScore = vScore*multLookup[params.TapNoteScore]/10;
			sm3_GradePoints[p] = sm3_GradePoints[p] + gradeWeightTable[params.TapNoteScore];
		end;
	end;

	-- [ja] コンボは自前で処理する
	if not params.HoldNoteScore then
		if params.TapNoteScore == 'TapNoteScore_W1' or
		   params.TapNoteScore == 'TapNoteScore_W2' or
		   (
		   	GAMESTATE:GetPlayMode() ~= 'PlayMode_Oni' and
		   	params.TapNoteScore == 'TapNoteScore_W3'
		   )
		then
			sm3_Combo[p] = sm3_Combo[p] + 1;
		-- [ja] 地雷とHoldCheckpointsの場合はリセットしない
		elseif params.TapNoteScore ~= "TapNoteScore_HitMine" and
		       params.TapNoteScore ~= "TapNoteScore_AvoidMine" and
		       params.TapNoteScore ~= "TapNoteScore_CheckpointHit" and
		       params.TapNoteScore ~= "TapNoteScore_CheckpointMiss"
		then
			sm3_Combo[p] = 0;
		end;
		sm3_CurMaxCombo[p] = sm3_CurMaxCombo[p] + 1;
	end;

	local bonusTable =
	{
		['TapNoteScore_W1'] = 55,
		['TapNoteScore_W2'] = 55,
		['TapNoteScore_W3'] = 33
	};
	setmetatable(bonusTable, ZeroIfNotFound);

	-- [ja] 落ちていた場合は入るスコアが減る
	if (pss:GetFailed()) then
		if (params.HoldNoteScore == 'HoldNoteScore_Held') then
			-- [ja] O.K.判定時は10点
			sm3_Score[p] = sm3_Score[p]+10;
		elseif (params.HoldNoteScore ~= 'HoldNoteScore_LetGo') then
			-- [ja] O.K.でもN.G.でもない場合のスコア：
			-- W1 (Marvelous): 10
			-- W2 (Perfect):    9
			-- W3 (Great):      5
			sm3_Score[p] = sm3_Score[p]+multLookup[params.TapNoteScore];
		end;

		-- [ja] 5点単位の処理がなぜかここでされる
		if multLookup[params.TapNoteScore] > 0 then
			sm3_Score[p] = math.floor((sm3_Score[p] + 2) / 5) * 5;
		end;
	else
		sm3_Score[p] = sm3_Score[p]+math.floor(vScore);
		if (params.HoldNoteScore == 'HoldNoteScore_Held') then
			sm3_Bonus[p] = sm3_Bonus[p]+55*sm3_Combo[p];
		elseif (params.HoldNoteScore ~= 'HoldNoteScore_LetGo') then
			sm3_Bonus[p] = sm3_Bonus[p]+bonusTable[params.TapNoteScore]*sm3_Combo[p];
		end;
		sm3_MaxBonus[p] = sm3_MaxBonus[p]+55*sm3_CurMaxCombo[p];
	end;

	if (sm3_Steps[p] == totalItems and not pss:GetFailed()) then
		-- [ja] 最後の1ステップがW2以上の場合、端数を加算する
		-- [ja] 端数は設定上の満点と最終ステップ時点での満点の差から求める
		if (isW2OrAbove(params.TapNoteScore) or params.HoldNoteScore == 'HoldNoteScore_Held') then
			sm3_Score[p] = sm3_Score[p]+(baseScore-sm3_CurMaxScore[p]);
			sm3_CurMaxScore[p] = sm3_CurMaxScore[p]+(baseScore-sm3_CurMaxScore[p]);
		end;
	end;

	-- [ja] 5点単位の処理は5TH方式のみでされる
	pss:SetScore(sm3_Score[p] - sm3_Score[p] % 5);
	pss:SetCurMaxScore(sm3_CurMaxScore[p] - sm3_CurMaxScore[p] % 5);

	if (sm3_Steps[p] == totalItems and not pss:GetFailed()) then
		-- [ja] コンボボーナスとグレードボーナスを加算
		-- [ja] 本当はリザルト画面でやりたいんだけどね
		local totalTaps = radarValues:GetValue('RadarCategory_TapsAndHolds');
		local totalHolds = radarValues:GetValue('RadarCategory_Holds') + radarValues:GetValue('RadarCategory_Rolls');

		local gradeBonusTable = {
			-- Threshold: sm3_GradePoints[p] / (totalTaps * 2 + totalHolds * 6) * 100
			{ Threshold = 100, Bonus = 10000000 },
			{ Threshold =  93, Bonus =  1000000 },
			{ Threshold =  80, Bonus =   100000 },
			{ Threshold =  65, Bonus =    10000 },
			{ Threshold =  45, Bonus =     1000 },
			-- [ja] 45%に達していない場合は100点とする
		};

		local gradeBonus = 100;

		for i = 1,#gradeBonusTable do
			if (sm3_GradePoints[p] / (totalTaps * 2 + totalHolds * 6) * 100 >= gradeBonusTable[i].Threshold) then
				gradeBonus = gradeBonusTable[i].Bonus;
				break;
			end;
		end;

		if pss:GetFailed() then
			gradeBonus = 0;
		end;

		pss:SetScore(pss:GetScore()+sm3_Bonus[p]+gradeBonus);
		pss:SetCurMaxScore(pss:GetCurMaxScore()+sm3_MaxBonus[p]+10000000);
	end;
end;

r["3.9Plus HYBRID"] = function(params, pss)
	local multLookup =
	{
		['TapNoteScore_W1'] = 10,
		['TapNoteScore_W2'] = GAMESTATE:ShowW1() and 9 or 10,
		['TapNoteScore_W3'] = 5
	};
	setmetatable(multLookup, ZeroIfNotFound);
	local radarValues = GetDirectRadar(params.Player);
	local totalItems = GetTotalItems(radarValues);
	-- 1+2+3+...+totalItems value/の値
	local sTotal = (totalItems+1)*totalItems/2;
	local steps = GAMESTATE:GetCurrentSteps(params.Player);
	local meter = steps:GetMeter();
	meter = clamp(meter,1,10);

	-- [ja] 満点を求める
	local length = 1;
	if (GAMESTATE:GetCurrentSong():IsMarathon()) then
		length = 3;
	elseif (GAMESTATE:GetCurrentSong():IsLong()) then
		length = 2;
	end;
	local baseScore = 100000000 * length;

	local sOne = baseScore/sTotal;

	local p = (params.Player == 'PlayerNumber_P1') and 1 or 2;

	-- [ja] スコアが0の時に初期化
	if pss:GetScore()==0 then
		sm3_Steps[p] = 0;
	end;

	-- [ja] 現在のステップ数
	-- [ja] 地雷とHoldCheckpointsの場合は加算しない
	if params.TapNoteScore ~= "TapNoteScore_HitMine" and
	   params.TapNoteScore ~= "TapNoteScore_AvoidMine" and
	   params.TapNoteScore ~= "TapNoteScore_CheckpointHit" and
	   params.TapNoteScore ~= "TapNoteScore_CheckpointMiss"
	then
		sm3_Steps[p]=sm3_Steps[p]+1;
	end;

	-- [en] current score
	-- [ja] 今回加算するスコア（W1の時）
	local vScore = sOne*sm3_Steps[p];
	pss:SetCurMaxScore(pss:GetCurMaxScore()+math.floor(vScore));
	-- [ja] 判定によって加算量を変更
	if (params.HoldNoteScore == 'HoldNoteScore_Held') then
		vScore = vScore;
	else
		if (params.HoldNoteScore == 'HoldNoteScore_LetGo') then
			vScore = 0;
		else
			vScore = vScore*multLookup[params.TapNoteScore]/10;
		end;
	end;

	-- [ja] 落ちていた場合は入るスコアが減る
	if (pss:GetFailed()) then
		if (params.HoldNoteScore == 'HoldNoteScore_Held') then
			-- [ja] O.K.判定時は10点
			pss:SetScore(pss:GetScore()+10);
		elseif (params.HoldNoteScore ~= 'HoldNoteScore_LetGo') then
			-- [ja] O.K.でもN.G.でもない場合のスコア：
			-- W1 (Marvelous): 10
			-- W2 (Perfect):    9
			-- W3 (Great):      5
			pss:SetScore(pss:GetScore()+multLookup[params.TapNoteScore]);
		end;

		-- [ja] 5点単位の処理がなぜかここでされる
		if multLookup[params.TapNoteScore] > 0 then
			pss:SetScore(math.floor((pss:GetScore() + 2) / 5) * 5);
		end;
	else
		pss:SetScore(pss:GetScore()+math.floor(vScore));
	end;

	if (sm3_Steps[p] == totalItems and not pss:GetFailed()) then
		-- [ja] 最後の1ステップがW2以上の場合、端数を加算する
		-- [ja] 端数は設定上の満点と最終ステップ時点での満点の差から求める
		if (isW2OrAbove(params.TapNoteScore) or params.HoldNoteScore == 'HoldNoteScore_Held') then
			pss:SetScore(pss:GetScore()+(baseScore-pss:GetCurMaxScore()));
			pss:SetCurMaxScore(pss:GetCurMaxScore()+(baseScore-pss:GetCurMaxScore()));
		end;
	end;
end;

r["3.9Plus SN"] = function(params, pss)
	local radarValues = GetDirectRadar(params.Player);
	local totalItems = GetTotalItems(radarValues);

	-- [ja] 満点を求める
	local length = 1;
	if (GAMESTATE:GetCurrentSong():IsMarathon()) then
		length = 3;
	elseif (GAMESTATE:GetCurrentSong():IsLong()) then
		length = 2;
	end;
	local baseScore = 10000000 * length;

	-- [en] current score
	-- [ja] 今回加算するスコア
	local multLookup =
	{
		['TapNoteScore_W1'] = math.floor(baseScore / totalItems),
		['TapNoteScore_W2'] = math.floor(baseScore / totalItems),
		['TapNoteScore_W3'] = math.floor(math.floor(baseScore / 2) / totalItems)
	};
	setmetatable(multLookup, ZeroIfNotFound);
	local vScore = multLookup[params.TapNoteScore];
	pss:SetCurMaxScore(pss:GetCurMaxScore()+multLookup['TapNoteScore_W1']);

	-- [ja] 判定によって加算量を変更
	if (params.HoldNoteScore == 'HoldNoteScore_Held') then
		vScore = multLookup['TapNoteScore_W1'];
	elseif (params.HoldNoteScore == 'HoldNoteScore_LetGo') then
		vScore = 0;
	end;

	-- [ja] 落ちていた場合は入るスコアが減る
	if (not pss:GetFailed()) then
		pss:SetScore(pss:GetScore()+math.floor(vScore));
	end;

	-- [ja] 端数の処理はされていないようだ
end;

r["3.9Plus SN2"] = function(params, pss)
	local radarValues = GetDirectRadar(params.Player);
	local totalItems = GetTotalItems(radarValues);

	-- [ja] 満点を求める
	local length = 1;
	if (GAMESTATE:GetCurrentSong():IsMarathon()) then
		length = 3;
	elseif (GAMESTATE:GetCurrentSong():IsLong()) then
		length = 2;
	end;
	local baseScore = 1000000 * length;

	-- [en] current score
	-- [ja] 今回加算するスコア
	local multLookup =
	{
		['TapNoteScore_W1'] = math.floor(baseScore / totalItems),
		['TapNoteScore_W2'] = math.floor(baseScore / totalItems) - 10,
		['TapNoteScore_W3'] = math.floor(math.floor(baseScore / 2) / totalItems) - 10
	};
	setmetatable(multLookup, ZeroIfNotFound);
	local vScore = multLookup[params.TapNoteScore];
	pss:SetCurMaxScore(pss:GetCurMaxScore()+multLookup['TapNoteScore_W1']);

	-- [ja] 判定によって加算量を変更
	if (params.HoldNoteScore == 'HoldNoteScore_Held') then
		vScore = multLookup['TapNoteScore_W1'];
	elseif (params.HoldNoteScore == 'HoldNoteScore_LetGo') then
		vScore = 0;
	end;

	-- [ja] 落ちていた場合は入るスコアが減る
	if (not pss:GetFailed()) then
		pss:SetScore(pss:GetScore()+math.floor(vScore));
	end;

	-- [ja] 端数の処理はされていないようだ
end;

-----------------------------------------------------------
--Oldschool scoring, best described as a modified 4th mix scheme
--with a little 1st mix influence
-----------------------------------------------------------
r['Oldschool'] = function(params, pss)
  local bestPoints = 999
	local scoreLookupTable =
	{ 
		['TapNoteScore_W1']=999, 
		['TapNoteScore_W2']=IsW1Allowed('TapNoteScore_W2') and 888 or 999, 
		['TapNoteScore_W3']=777,
		['TapNoteScore_W4']=555,
		['TapNoteScore_W5']=111, 
	};
	setmetatable(scoreLookupTable, ZeroIfNotFound);
	local comboBonusForThisStep = (pss:GetCurrentCombo()*111)^1.1;
	local capScore = 1000000000;
	pss:SetCurMaxScore(capScore); --i don't really care about weird scoring modes -fsx
	local pointsGot = comboBonusForThisStep + scoreLookupTable[params.TapNoteScore] + (params.HoldNoteScore == 'HoldNoteScore_Held' and 777 or 0);
	pss:SetScore(clamp(pss:GetScore()+pointsGot,0,capScore));
end;

-----------------------------------------------------------
--DDR MAX2/Extreme(-esque) Scoring by @sakuraponila
-----------------------------------------------------------
local ext_Steps = {0,0};
r['DDR Extreme'] = function(params, pss)
	local multLookup =
	{
		['TapNoteScore_W1'] = 10,
		['TapNoteScore_W2'] = 9,
		['TapNoteScore_W3'] = 5
	};
	setmetatable(multLookup, ZeroIfNotFound);
	local steps = GAMESTATE:GetCurrentSteps(params.Player);
	local radarValues = GetDirectRadar(params.Player);
	local totalItems = GetTotalItems(radarValues);
	-- 1 + 2 + 3 + ... + totalItems value/の値
	local sTotal = (totalItems + 1) * totalItems / 2;
	local meter = steps:GetMeter();
	if (steps:IsAnEdit()) then
		meter = 5;
	else
		meter = math.min(10,meter);
	end;
	-- [en] score for one step
	-- [ja] 1ステップあたりのスコア
	local baseScore = meter * 1000000
	if (GAMESTATE:GetCurrentSong():IsMarathon()) then
		baseScore = baseScore * 3;
	elseif (GAMESTATE:GetCurrentSong():IsLong()) then
		baseScore = baseScore * 2;
	end;
	local sOne = math.floor(baseScore / sTotal);
	-- [en] measures for 5 points of units
	-- [ja] 5点単位のための処置
	sOne = sOne - sOne % 5;
	-- [en] because fractions are added by the last step, get value
	-- [ja] 端数は最後の1ステップで加算するのでその値を取得
	local sLast = baseScore - (sOne * sTotal);

	local p = (params.Player == 'PlayerNumber_P1') and 1 or 2;
	-- [en] initialized when score is 0
	-- [ja] スコアが0の時に初期化
	if pss:GetScore() == 0 then
		ext_Steps[p] = 0;
	end;
	-- [en] now step count
	-- [ja] 現在のステップ数
	ext_Steps[p] = ext_Steps[p] + 1;
	-- [en] current score
	-- [ja] 今回加算するスコア（W1の時）
	local vScore = sOne * ext_Steps[p];
	pss:SetCurMaxScore(pss:GetCurMaxScore() + vScore);
	-- [ja] 判定によって加算量を変更
	if (params.HoldNoteScore == 'HoldNoteScore_Held') then
	-- [ja] O.K.判定時は問答無用で満点
		vScore = vScore;
	else
		-- [ja] N.G.判定時は問答無用で0点
		if (params.HoldNoteScore == 'HoldNoteScore_LetGo') then
			vScore = 0;
		-- [en] non-long note scoring
		-- [ja] それ以外ということは、ロングノート以外の判定である
		else
			vScore = vScore * multLookup[params.TapNoteScore] / 10
		end;
	end;
	-- [en] measures for 5 points of units
	-- [ja] ここでも5点単位のための処置
	vScore = vScore - vScore % 5
	pss:SetScore(pss:GetScore() + vScore);
	-- if one of the last step, add the fractions
	-- [ja] 最後の1ステップの場合、端数を加算する
	if ((vScore > 0) and (ext_Steps[p] == totalItems)) then
		pss:SetScore(pss:GetScore() + sLast);
	end;
end;

-----------------------------------------------------------
--HYBRID Scoring, contributed by @waiei
-----------------------------------------------------------
local hyb_Steps={0,0};
r['Hybrid'] = function(params, pss)
	local multLookup =
	{
		['TapNoteScore_W1'] = 10,
		['TapNoteScore_W2'] = 9,
		['TapNoteScore_W3'] = 5
	};
	setmetatable(multLookup, ZeroIfNotFound);
	local radarValues = GetDirectRadar(params.Player);
	local totalItems = GetTotalItems(radarValues);
	-- 1+2+3+...+totalItems value/の値
	local sTotal = (totalItems+1)*totalItems/2;
	-- [en] Score for one song
	-- [ja] 1ステップあたりのスコア
	local sOne = math.floor(100000000/sTotal);
	-- [ja] 端数は最後の1ステップで加算するのでその値を取得
	local sLast = 100000000-(sOne*sTotal);

	local p = (params.Player == 'PlayerNumber_P1') and 1 or 2;

	-- [ja] スコアが0の時に初期化
	if pss:GetScore()==0 then
		hyb_Steps[p]=0;
	end;

	-- [ja] 現在のステップ数
	hyb_Steps[p]=hyb_Steps[p]+1;
	-- [en] current score
	-- [ja] 今回加算するスコア（W1の時）
	local vScore = sOne*hyb_Steps[p];
	pss:SetCurMaxScore(pss:GetCurMaxScore()+vScore);
	-- [ja] 判定によって加算量を変更
	if (params.HoldNoteScore == 'HoldNoteScore_Held') then
		vScore = vScore;
	else
		if (params.HoldNoteScore == 'HoldNoteScore_LetGo') then
			vScore = 0;
		else
			vScore = vScore*multLookup[params.TapNoteScore]/10;
		end;
	end;
	pss:SetScore(pss:GetScore()+vScore);

	-- [ja] 最後の1ステップの場合、端数を加算する
	if ((vScore > 0) and (hyb_Steps[p] == totalItems)) then
		pss:SetScore(pss:GetScore()+sLast);
	end;
end;

-----------------------------------------------------------
--DDR SuperNOVA scoring (Use MARVELOUS) by @sakuraponila
-----------------------------------------------------------
local sntmp_Score = {0,0};
local sntmp_Steps = {0,0};
r['DDR SuperNOVA'] = function(params, pss)
	local multLookup =
	{
		['TapNoteScore_W1'] = 3,
		['TapNoteScore_W2'] = 2,
		['TapNoteScore_W3'] = 1
	};
	setmetatable(multLookup, ZeroIfNotFound);
	local radarValues = GetDirectRadar(params.Player);
	local totalItems = GetTotalItems(radarValues)
	local p = (params.Player == 'PlayerNumber_P1') and 1 or 2;

	-- initialized when score is 0
	-- [ja] スコアが0の時に初期化
	if pss:GetScore() == 0 then
		sntmp_Score[p] = 0;
		sntmp_Steps[p] = 0;
	end;

	-- [ja] 判定によって加算量を変更
	local maxAdd = 0;
	-- [ja] O.K.判定時は問答無用で満点
	if params.HoldNoteScore == 'HoldNoteScore_Held' then
		maxAdd = 3;
	else
		-- [ja] N.G.判定時は問答無用で0点
		if params.HoldNoteScore == 'HoldNoteScore_LetGo' then
			maxAdd = 0;
		-- [ja] それ以外ということは、ロングノート以外の判定である
		else
			maxAdd = multLookup[params.TapNoteScore];
		end
	end;
	sntmp_Score[p] = sntmp_Score[p] + maxAdd;

	-- [ja] 踏み踏みしたステップ数
	sntmp_Steps[p] = sntmp_Steps[p] + 1;
	-- [ja] 現時点での、All W1判定の時のスコア
	pss:SetCurMaxScore(math.floor(10000000 * sntmp_Steps[p] / totalItems / 3));
	-- [ja] 計算して代入
	pss:SetScore(math.floor(10000000 * sntmp_Score[p] / totalItems / 3));
end;

-----------------------------------------------------------
--DDR SuperNOVA 2 scoring by @waiei
-----------------------------------------------------------
local sn2tmp_Sub={0,0};
local sn2tmp_Score={0,0};
local sn2tmp_Steps={0,0};
r['DDR SuperNOVA 2'] = function(params, pss)
	local multLookup =
	{
		['TapNoteScore_W1'] = 10,
		['TapNoteScore_W2'] = 10,
		['TapNoteScore_W3'] = 5
	};
	setmetatable(multLookup, ZeroIfNotFound);
	local radarValues = GetDirectRadar(params.Player);
	local totalItems = GetTotalItems(radarValues);
	local p = (params.Player == 'PlayerNumber_P1') and 1 or 2;

	-- [ja] スコアが0の時に初期化
	if pss:GetScore()==0 then
		sn2tmp_Sub[p]=0;
		sn2tmp_Score[p]=0;
		sn2tmp_Steps[p]=0;
	end;

	-- [ja] maxAdd は 加算する最高点を 10 とした時の値（つまり、10=100% / 5=50%）
	local maxAdd = 0;
	-- [ja] O.K.判定時は問答無用で満点
	if params.HoldNoteScore == 'HoldNoteScore_Held' then
		maxAdd = 10;
	else
		-- [ja] N.G.判定時は問答無用で0点
		if params.HoldNoteScore == 'HoldNoteScore_LetGo' then
			maxAdd = 0;
		-- [ja] それ以外ということは、ロングノート以外の判定である
		else
			maxAdd = multLookup[params.TapNoteScore];
			if (params.TapNoteScore == 'TapNoteScore_W2') or (params.TapNoteScore=='TapNoteScore_W3') then
				-- [ja] W2とW3の数を記録
				sn2tmp_Sub[p]=sn2tmp_Sub[p]+1;
			end;
		end
	end;
	sn2tmp_Score[p]=sn2tmp_Score[p]+maxAdd;
	-- [ja] 踏み踏みしたステップ数
	sn2tmp_Steps[p]=sn2tmp_Steps[p]+1;
	-- [ja] 現時点での、All W1判定の時のスコア
	pss:SetCurMaxScore(math.floor(10000*sn2tmp_Steps[p]/totalItems) * 10);

	-- [ja] 計算して代入
	pss:SetScore((math.floor(10000*sn2tmp_Score[p]/totalItems) * 10) - (sn2tmp_Sub[p]*10) );
end;

-----------------------------------------------------------
--Radar Master (disabled; todo: get this working with StepMania 5)
-----------------------------------------------------------
r['[SSC] Radar Master'] = function(params, pss)
	local masterTable = {
		['RadarCategory_Stream'] = 0,
		['RadarCategory_Voltage'] = 0,
		['RadarCategory_Air'] = 0,
		['RadarCategory_Freeze'] = 0,
		['RadarCategory_Chaos'] = 0
	};
	local totalRadar = 0;
	local finalScore = 0;
	for k,v in pairs(masterTable) do
		local firstRadar = GetDirectRadar(params.Player):GetValue(k);
		if firstRadar == 0 then
			masterTable[k] = nil;
		else
			masterTable[k] = firstRadar;
			totalRadar = totalRadar + firstRadar;
		end;
	end;
	--two loops are needed because we need to calculate totalRadar
	--to actually calculate any part of the score
	for k,v in pairs(masterTable) do
		local curPortion = pss:GetRadarActual():GetValue(k) / v;
		finalScore = finalScore + curPortion*(500000000*(v/totalRadar));
	end;
	pss:SetScore(finalScore);
end;
------------------------------------------------------------
--Marvelous Incorporated Grading System (or MIGS for short)
--basically like DP scoring with locked DP values
------------------------------------------------------------
r['MIGS'] = function(params,pss)
	local curScore = 0;
	local tapScoreTable = 
	{ 
		['TapNoteScore_W1'] = 3,
		['TapNoteScore_W2'] = 2,
		['TapNoteScore_W3'] = 1,
		['TapNoteScore_W5'] = -4,
		['TapNoteScore_Miss'] = -8
	};
	for k,v in pairs(tapScoreTable) do
		curScore = curScore + ( pss:GetTapNoteScores(k) * v );
	end;
  curScore = math.max(0,curScore + ( pss:GetHoldNoteScores('HoldNoteScore_Held') * 6 ));
  pss:SetScore(curScore);
end;

--------------------------------------------------------------
--1bilDP scoring because I can.
--------------------------------------------------------------
r['Billions DP']= function(params,pss)
  local poss = pss:GetPossibleDancePoints()
  pss:SetScore(math.floor((pss:GetActualDancePoints()/poss)*1000000000))
  pss:SetCurMaxScore(math.floor((pss:GetCurrentPossibleDancePoints()/poss)*1000000000))
end
-------------------------------------------------------------------------------
-- Formulas end here.
for v in ivalues(DisabledScoringModes) do r[v] = nil end
Scoring = r;

function UserPrefScoringMode()
  local baseChoices = {}
  for k,v in pairs(Scoring) do table.insert(baseChoices,k) end
  if next(baseChoices) == nil then UndocumentedFeature "No scoring modes available" end
	local t = {
		Name = "UserPrefScoringMode";
		LayoutType = "ShowAllInRow";
		SelectType = "SelectOne";
		OneChoiceForAllPlayers = true;
		ExportOnChange = false;
		Choices = baseChoices;
		LoadSelections = function(self, list, pn)
			if ReadPrefFromFile("UserPrefScoringMode") ~= nil then
        --Load the saved scoring mode from UserPrefs.
				local theValue = ReadPrefFromFile("UserPrefScoringMode");
				local success = false; 
        --HACK: Preview 4 took out 1st and 4th scoring. Replace with a close equivalent.
        if theValue == "DDR 1stMIX" or theValue == "DDR 4thMIX" then theValue = "Oldschool" end
        --Search the list of scoring modes for the saved scoring mode.        
				for k,v in ipairs(baseChoices) do if v == theValue then list[k] = true success = true break end end;
        --We couldn't find it, pick the first available scoring mode as a sane default.
				if success == false then list[1] = true end;
			else
        WritePrefToFile("UserPrefScoringMode", baseChoices[1]);
				list[1] = true;
			end;
		end;
		SaveSelections = function(self, list, pn)
			for k,v in ipairs(list) do if v then WritePrefToFile("UserPrefScoringMode", baseChoices[k]) break end end;
		end;
	};
	setmetatable( t, t );
	return t;
end

#ifndef TIMING_DATA_H
#define TIMING_DATA_H

#include "NoteTypes.h"
#include "TimingSegments.h"
#include "PrefsManager.h"
#include <float.h> // max float
struct lua_State;

/** @brief Compare a TimingData segment's properties with one another. */
#define COMPARE(x) if(this->x!=other.x) return false;

/**
 * @brief Holds data for translating beats<->seconds.
 */
class TimingData
{
public:
	void AddSegment(TimingSegmentType tst, TimingSegment * seg);
	
	int GetSegmentIndexAtRow(TimingSegmentType tst,
							 int row) const;
	
	int GetSegmentIndexAtBeat(TimingSegmentType tst,
							  float beat) const
	{
		return this->GetSegmentIndexAtRow(tst, BeatToNoteRow(beat));
	}
	
	float GetNextSegmentBeatAtRow(TimingSegmentType tst,
								  int row) const;
	
	float GetNextSegmentBeatAtBeat(TimingSegmentType tst,
								   float beat) const
	{
		return this->GetNextSegmentBeatAtRow(tst, BeatToNoteRow(beat));
	}
	
	float GetPreviousSegmentBeatAtRow(TimingSegmentType tst,
									  int row) const;
	
	float GetPreviousSegmentBeatAtBeat(TimingSegmentType tst,
									   float beat) const
	{
		return this->GetPreviousSegmentBeatAtRow(tst, BeatToNoteRow(beat));
	}
	
	bool empty() const;
	
	/**
	 * @brief Sets up initial timing data with a defined offset.
	 * @param fOffset the offset from the 0th beat. */
	TimingData(float fOffset = 0);
	
	~TimingData();
	
	TimingData CopyRange(int startRow, int endRow) const;
	/**
	 * @brief Gets the actual BPM of the song,
	 * while respecting a limit.
	 *
	 * The high limit is due to the implementation of mMods.
	 * @param fMinBPMOut the minimium specified BPM.
	 * @param fMaxBPMOut the maximum specified BPM.
	 * @param highest the highest allowed max BPM.
	 */
	void GetActualBPM( float &fMinBPMOut, float &fMaxBPMOut, float highest = FLT_MAX ) const;
	/**
	 * @brief Retrieve the BPM at the given row.
	 * @param iNoteRow the row in question.
	 * @return the BPM.
	 */
	float GetBPMAtRow( int iNoteRow ) const;
	/**
	 * @brief Retrieve the BPM at the given beat.
	 * @param fBeat the beat in question.
	 * @return the BPM.
	 */
	float GetBPMAtBeat( float fBeat ) const { return GetBPMAtRow( BeatToNoteRow(fBeat)); }
	/**
	 * @brief Set the row to have the new BPM.
	 * @param iNoteRow the row to have the new BPM.
	 * @param fBPM the BPM.
	 */
	void SetBPMAtRow( int iNoteRow, float fBPM );
	/**
	 * @brief Set the beat to have the new BPM.
	 * @param fBeat the beat to have the new BPM.
	 * @param fBPM the BPM.
	 */
	void SetBPMAtBeat( float fBeat, float fBPM ) { SetBPMAtRow( BeatToNoteRow(fBeat), fBPM ); }
	/**
	 * @brief Retrieve the BPMSegment at the specified row.
	 * @param iNoteRow the row that has a BPMSegment.
	 * @return the BPMSegment in question.
	 */
	BPMSegment* GetBPMSegmentAtRow( int iNoteRow );
	/**
	 * @brief Retrieve the BPMSegment at the specified beat.
	 * @param fBeat the beat that has a BPMSegment.
	 * @return the BPMSegment in question.
	 */
	BPMSegment* GetBPMSegmentAtBeat( float fBeat ) { return GetBPMSegmentAtRow( (int)BeatToNoteRow(fBeat)); }
	
	/**
	 * @brief Retrieve the stop time at the given row.
	 * @param iNoteRow the row in question.
	 * @return the stop time.
	 */
	float GetStopAtRow( int iNoteRow ) const;
	/**
	 * @brief Retrieve the stop time at the given beat.
	 * @param fBeat the beat in question.
	 * @return the stop time.
	 */
	float GetStopAtBeat( float fBeat ) const { return GetStopAtRow( BeatToNoteRow(fBeat) ); }
	
	/**
	 * @brief Set the row to have the new stop time.
	 * @param iNoteRow the row to have the new stop time.
	 * @param fSeconds the new stop time.
	 */
	void SetStopAtRow( int iNoteRow, float fSeconds );
	/**
	 * @brief Set the beat to have the new stop time.
	 * @param fBeat to have the new stop time.
	 * @param fSeconds the new stop time.
	 */
	void SetStopAtBeat( float fBeat, float fSeconds ) { SetStopAtRow( BeatToNoteRow(fBeat), fSeconds); }
	/**
	 * @brief Retrieve the StopSegment at the specified row.
	 * @param iNoteRow the row that has a StopSegment.
	 * @return the StopSegment in question.
	 */
	StopSegment* GetStopSegmentAtRow( int iNoteRow );
	/**
	 * @brief Retrieve the StopSegment at the specified beat.
	 * @param fBeat the beat that has a StopSegment.
	 * @return the StopSegment in question.
	 */
	StopSegment* GetStopSegmentAtBeat( float fBeat ) { return GetStopSegmentAtRow( BeatToNoteRow(fBeat)); }
	
	
	/**
	 * @brief Retrieve the delay time at the given row.
	 * @param iNoteRow the row in question.
	 * @return the delay time.
	 */
	float GetDelayAtRow( int iNoteRow ) const;
	/**
	 * @brief Retrieve the delay time at the given beat.
	 * @param fBeat the beat in question.
	 * @return the delay time.
	 */
	float GetDelayAtBeat( float fBeat ) const { return GetDelayAtRow( BeatToNoteRow(fBeat) ); }
	
	/**
	 * @brief Set the row to have the new delay time.
	 *
	 * This function was added specifically for sm-ssc.
	 * @param iNoteRow the row to have the new delay time.
	 * @param fSeconds the new delay time.
	 */
	void SetDelayAtRow( int iNoteRow, float fSeconds );
	/**
	 * @brief Set the beat to have the new delay time.
	 *
	 * This function was added specifically for sm-ssc.
	 * @param fBeat the beat to have the new delay time.
	 * @param fSeconds the new delay time.
	 */
	void SetDelayAtBeat( float fBeat, float fSeconds ) { SetDelayAtRow( BeatToNoteRow(fBeat), fSeconds); }
	/**
	 * @brief Retrieve the DelaySegment at the specified row.
	 * @param iNoteRow the row that has a DelaySegment.
	 * @return the DelaySegment in question.
	 */
	DelaySegment* GetDelaySegmentAtRow( int iNoteRow );
	/**
	 * @brief Retrieve the DelaySegment at the specified beat.
	 * @param fBeat the beat that has a DelaySegment.
	 * @return the DelaySegment in question.
	 */
	DelaySegment* GetDelaySegmentAtBeat( float fBeat ) { return GetDelaySegmentAtRow( BeatToNoteRow(fBeat)); }
		
	/**
	 * @brief Retrieve the Time Signature's numerator at the given row.
	 * @param iNoteRow the row in question.
	 * @return the numerator.
	 */
	int GetTimeSignatureNumeratorAtRow( int iNoteRow );
	/**
	 * @brief Retrieve the Time Signature's numerator at the given beat.
	 * @param fBeat the beat in question.
	 * @return the numerator.
	 */
	int GetTimeSignatureNumeratorAtBeat( float fBeat ) { return GetTimeSignatureNumeratorAtRow( BeatToNoteRow(fBeat) ); }
	/**
	 * @brief Retrieve the Time Signature's denominator at the given row.
	 * @param iNoteRow the row in question.
	 * @return the denominator.
	 */
	int GetTimeSignatureDenominatorAtRow( int iNoteRow );
 	/**
	 * @brief Retrieve the Time Signature's denominator at the given beat.
	 * @param fBeat the beat in question.
	 * @return the denominator.
	 */
	int GetTimeSignatureDenominatorAtBeat( float fBeat ) { return GetTimeSignatureDenominatorAtRow( BeatToNoteRow(fBeat) ); }
	/**
	 * @brief Set the row to have the new Time Signature.
	 * @param iNoteRow the row to have the new Time Signature.
	 * @param iNumerator the numerator.
	 * @param iDenominator the denominator.
	 */
	void SetTimeSignatureAtRow( int iNoteRow, int iNumerator, int iDenominator );
	/**
	 * @brief Set the beat to have the new Time Signature.
	 * @param fBeat the beat to have the new Time Signature.
	 * @param iNumerator the numerator.
	 * @param iDenominator the denominator.
	 */
	void SetTimeSignatureAtBeat( float fBeat, int iNumerator, int iDenominator ) { SetTimeSignatureAtRow( BeatToNoteRow(fBeat), iNumerator, iDenominator ); }
	/**
	 * @brief Set the row to have the new Time Signature numerator.
	 * @param iNoteRow the row to have the new Time Signature numerator.
	 * @param iNumerator the numerator.
	 */
	void SetTimeSignatureNumeratorAtRow( int iNoteRow, int iNumerator );
	/**
	 * @brief Set the beat to have the new Time Signature numerator.
	 * @param fBeat the beat to have the new Time Signature numerator.
	 * @param iNumerator the numerator.
	 */
	void SetTimeSignatureNumeratorAtBeat( float fBeat, int iNumerator ) { SetTimeSignatureNumeratorAtRow( BeatToNoteRow(fBeat), iNumerator); }
	/**
	 * @brief Set the row to have the new Time Signature denominator.
	 * @param iNoteRow the row to have the new Time Signature denominator.
	 * @param iDenominator the denominator.
	 */
	void SetTimeSignatureDenominatorAtRow( int iNoteRow, int iDenominator );
	/**
	 * @brief Set the beat to have the new Time Signature denominator.
	 * @param fBeat the beat to have the new Time Signature denominator.
	 * @param iDenominator the denominator.
	 */
	void SetTimeSignatureDenominatorAtBeat( float fBeat, int iDenominator ) { SetTimeSignatureDenominatorAtRow( BeatToNoteRow(fBeat), iDenominator); }
	/**
	 * @brief Retrieve the TimeSignatureSegment at the specified row.
	 * @param iNoteRow the row that has a TimeSignatureSegment.
	 * @return the TimeSignatureSegment in question.
	 */
	TimeSignatureSegment* GetTimeSignatureSegmentAtRow( int iNoteRow );
	/**
	 * @brief Retrieve the TimeSignatureSegment at the specified beat.
	 * @param fBeat the beat that has a TimeSignatureSegment.
	 * @return the TimeSignatureSegment in question.
	 */
	TimeSignatureSegment* GetTimeSignatureSegmentAtBeat( float fBeat ) { return GetTimeSignatureSegmentAtRow( BeatToNoteRow(fBeat) ); }
	
	/**
	 * @brief Determine the beat to warp to.
	 * @param iRow The row you start on.
	 * @return the beat you warp to.
	 */
	float GetWarpAtRow( int iRow ) const;
	/**
	 * @brief Determine the beat to warp to.
	 * @param fBeat The beat you start on.
	 * @return the beat you warp to.
	 */
	float GetWarpAtBeat( float fBeat ) const { return GetWarpAtRow( BeatToNoteRow( fBeat ) ); }
	/**
	 * @brief Set the beat to warp to given a starting row.
	 * @param iRow The row to start on.
	 * @param fNew The destination beat.
	 */
	void SetWarpAtRow( int iRow, float fNew );
	/**
	 * @brief Set the beat to warp to given a starting beat.
	 * @param fBeat The beat to start on.
	 * @param fNew The destination beat.
	 */
	void SetWarpAtBeat( float fBeat, float fNew ) { SetWarpAtRow( BeatToNoteRow( fBeat ), fNew ); }
	/**
	 * @brief Retrieve the WarpSegment at the specified row.
	 * @param iRow the row to focus on.
	 * @return the WarpSegment in question.
	 */
	WarpSegment* GetWarpSegmentAtRow( int iRow );
	/**
	 * @brief Retrieve the WarpSegment at the specified beat.
	 * @param fBeat the beat to focus on.
	 * @return the WarpSegment in question.
	 */
	WarpSegment* GetWarpSegmentAtBeat( float fBeat ) { return GetWarpSegmentAtRow( BeatToNoteRow( fBeat ) ); }
	/**
	 * @brief Checks if the row is inside a warp.
	 * @param iRow the row to focus on.
	 * @return true if the row is inside a warp, false otherwise.
	 */
	bool IsWarpAtRow( int iRow ) const;
	/**
	 * @brief Checks if the beat is inside a warp.
	 * @param fBeat the beat to focus on.
	 * @return true if the row is inside a warp, false otherwise.
	 */
	bool IsWarpAtBeat( float fBeat ) const { return IsWarpAtRow( BeatToNoteRow( fBeat ) ); }
	
	/**
	 * @brief Retrieve the Tickcount at the given row.
	 * @param iNoteRow the row in question.
	 * @return the Tickcount.
	 */
	int GetTickcountAtRow( int iNoteRow ) const;
	/**
	 * @brief Retrieve the Tickcount at the given beat.
	 * @param fBeat the beat in question.
	 * @return the Tickcount.
	 */
	int GetTickcountAtBeat( float fBeat ) const { return GetTickcountAtRow( BeatToNoteRow(fBeat) ); }
	/**
	 * @brief Set the row to have the new tickcount.
	 * @param iNoteRow the row to have the new tickcount.
	 * @param iTicks the tickcount.
	 */
	void SetTickcountAtRow( int iNoteRow, int iTicks );
	/**
	 * @brief Set the beat to have the new tickcount.
	 * @param fBeat the beat to have the new tickcount.
	 * @param iTicks the tickcount.
	 */
	void SetTickcountAtBeat( float fBeat, int iTicks ) { SetTickcountAtRow( BeatToNoteRow( fBeat ), iTicks ); }
	/**
	 * @brief Retrieve the TickcountSegment at the specified row.
	 * @param iNoteRow the row that has a TickcountSegment.
	 * @return the TickcountSegment in question.
	 */
	TickcountSegment* GetTickcountSegmentAtRow( int iNoteRow );
	/**
	 * @brief Retrieve the TickcountSegment at the specified beat.
	 * @param fBeat the beat that has a TickcountSegment.
	 * @return the TickcountSegment in question.
	 */
	TickcountSegment* GetTickcountSegmentAtBeat( float fBeat ) { return GetTickcountSegmentAtRow( BeatToNoteRow(fBeat) ); }
	
	/**
	 * @brief Retrieve the Combo at the given row.
	 * @param iNoteRow the row in question.
	 * @return the Combo.
	 */
	int GetComboAtRow( int iNoteRow ) const;
	/**
	 * @brief Retrieve the Combo at the given beat.
	 * @param fBeat the beat in question.
	 * @return the Combo.
	 */
	int GetComboAtBeat( float fBeat ) const { return GetComboAtRow( BeatToNoteRow(fBeat) ); }
	/**
	 * @brief Retrieve the Miss Combo at the given row.
	 * @param iNoteRow the row in question.
	 * @return the Miss Combo.
	 */
	int GetMissComboAtRow( int iNoteRow ) const;
	/**
	 * @brief Retrieve the Miss Combo at the given beat.
	 * @param fBeat the beat in question.
	 * @return the Miss Combo.
	 */
	int GetMissComboAtBeat( float fBeat ) const { return GetMissComboAtRow( BeatToNoteRow(fBeat) ); }
	/**
	 * @brief Set the row to have the new Combo.
	 * @param iNoteRow the row to have the new Combo.
	 * @param iCombo the Combo.
	 */
	void SetComboAtRow( int iNoteRow, int iCombo );
	/**
	 * @brief Set the beat to have the new Combo.
	 * @param fBeat the beat to have the new Combo.
	 * @param iCombo the Combo.
	 */
	void SetComboAtBeat( float fBeat, int iCombo ) { SetComboAtRow( BeatToNoteRow( fBeat ), iCombo ); }
	/**
	 * @brief Set the row to have the new Combo and Miss Combo.
	 * @param iNoteRow the row to have the new Combo and Miss Combo.
	 * @param iCombo the Combo.
	 * @param iMiss the Miss Combo.
	 */
	void SetComboAtRow( int iNoteRow, int iCombo, int iMiss );
	/**
	 * @brief Set the beat to have the new Combo and Miss Combo.
	 * @param fBeat the beat to have the new Combo and Miss Combo.
	 * @param iCombo the Combo.
	 * @param iMiss the Miss Combo.
	 */
	void SetComboAtBeat( float fBeat, int iCombo, int iMiss ) { SetComboAtRow( BeatToNoteRow( fBeat ), iCombo, iMiss ); }
	/**
	 * @brief Set the row to have the new Combo.
	 * @param iNoteRow the row to have the new Combo.
	 * @param iCombo the Combo.
	 */
	void SetHitComboAtRow( int iNoteRow, int iCombo );
	/**
	 * @brief Set the beat to have the new Combo.
	 * @param fBeat the beat to have the new Combo.
	 * @param iCombo the Combo.
	 */
	void SetHitComboAtBeat( float fBeat, int iCombo ) { SetHitComboAtRow( BeatToNoteRow( fBeat ), iCombo ); }
	/**
	 * @brief Set the row to have the new Miss Combo.
	 * @param iNoteRow the row to have the new Miss Combo.
	 * @param iCombo the Miss Combo.
	 */
	void SetMissComboAtRow( int iNoteRow, int iCombo );
	/**
	 * @brief Set the beat to have the new Miss Combo.
	 * @param fBeat the beat to have the new Miss Combo.
	 * @param iCombo the Miss Combo.
	 */
	void SetMissComboAtBeat( float fBeat, int iCombo ) { SetMissComboAtRow( BeatToNoteRow( fBeat ), iCombo ); }
	/**
	 * @brief Retrieve the ComboSegment at the specified row.
	 * @param iNoteRow the row that has a ComboSegment.
	 * @return the ComboSegment in question.
	 */
	ComboSegment* GetComboSegmentAtRow( int iNoteRow );
	/**
	 * @brief Retrieve the ComboSegment at the specified beat.
	 * @param fBeat the beat that has a ComboSegment.
	 * @return the ComboSegment in question.
	 */
	ComboSegment* GetComboSegmentAtBeat( float fBeat ) { return GetComboSegmentAtRow( BeatToNoteRow(fBeat) ); }
	
	/**
	 * @brief Retrieve the Label at the given row.
	 * @param iNoteRow the row in question.
	 * @return the Label.
	 */
	RString GetLabelAtRow( int iNoteRow ) const;
	/**
	 * @brief Retrieve the Label at the given beat.
	 * @param fBeat the beat in question.
	 * @return the Label.
	 */
	RString GetLabelAtBeat( float fBeat ) const { return GetLabelAtRow( BeatToNoteRow(fBeat) ); }
	/**
	 * @brief Set the row to have the new Label.
	 * @param iNoteRow the row to have the new Label.
	 * @param sLabel the Label.
	 */
	void SetLabelAtRow( int iNoteRow, const RString sLabel );
	/**
	 * @brief Set the beat to have the new Label.
	 * @param fBeat the beat to have the new Label.
	 * @param sLabel the Label.
	 */
	void SetLabelAtBeat( float fBeat, const RString sLabel ) { SetLabelAtRow( BeatToNoteRow( fBeat ), sLabel ); }
	/**
	 * @brief Retrieve the LabelSegment at the specified row.
	 * @param iNoteRow the row that has a LabelSegment.
	 * @return the LabelSegment in question.
	 */
	LabelSegment* GetLabelSegmentAtRow( int iNoteRow );
	/**
	 * @brief Retrieve the LabelSegment at the specified beat.
	 * @param fBeat the beat that has a LabelSegment.
	 * @return the LabelSegment in question.
	 */
	LabelSegment* GetLabelSegmentAtBeat( float fBeat ) { return GetLabelSegmentAtRow( BeatToNoteRow(fBeat) ); }
	
	/**
	 * @brief Determine if the requisite label already exists.
	 * @param sLabel the label to check.
	 * @return true if it exists, false otherwise. */
	bool DoesLabelExist( RString sLabel ) const;
	
	/**
	 * @brief Retrieve the Speed's percent at the given row.
	 * @param iNoteRow the row in question.
	 * @return the percent.
	 */
	float GetSpeedPercentAtRow( int iNoteRow );
	/**
	 * @brief Retrieve the Speed's percent at the given beat.
	 * @param fBeat the beat in question.
	 * @return the percent.
	 */
	float GetSpeedPercentAtBeat( float fBeat ) { return GetSpeedPercentAtRow( BeatToNoteRow(fBeat) ); }
	/**
	 * @brief Retrieve the Speed's wait at the given row.
	 * @param iNoteRow the row in question.
	 * @return the wait.
	 */
	float GetSpeedWaitAtRow( int iNoteRow );
 	/**
	 * @brief Retrieve the Speed's wait at the given beat.
	 * @param fBeat the beat in question.
	 * @return the wait.
	 */
	float GetSpeedWaitAtBeat( float fBeat ) { return GetSpeedWaitAtRow( BeatToNoteRow(fBeat) ); }
	/**
	 * @brief Retrieve the Speed's mode at the given row.
	 * @param iNoteRow the row in question.
	 * @return the mode.
	 */
	unsigned short GetSpeedModeAtRow( int iNoteRow );
 	/**
	 * @brief Retrieve the Speed's mode at the given beat.
	 * @param fBeat the beat in question.
	 * @return the mode.
	 */
	unsigned short GetSpeedModeAtBeat( float fBeat ) { return GetSpeedModeAtRow( BeatToNoteRow(fBeat) ); }
	/**
	 * @brief Set the row to have the new Speed.
	 * @param iNoteRow the row to have the new Speed.
	 * @param fPercent the percent.
	 * @param fWait the wait.
	 * @param usMode the mode.
	 */
	void SetSpeedAtRow( int iNoteRow, float fPercent, float fWait, unsigned short usMode );
	/**
	 * @brief Set the beat to have the new Speed.
	 * @param fBeat the beat to have the new Speed.
	 * @param fPercent the percent.
	 * @param fWait the wait.
	 * @param usMode the mode.
	 */
	void SetSpeedAtBeat( float fBeat, float fPercent, float fWait, unsigned short usMode ) { SetSpeedAtRow( BeatToNoteRow(fBeat), fPercent, fWait, usMode ); }
	/**
	 * @brief Set the row to have the new Speed percent.
	 * @param iNoteRow the row to have the new Speed percent.
	 * @param fPercent the percent.
	 */
	void SetSpeedPercentAtRow( int iNoteRow, float fPercent );
	/**
	 * @brief Set the beat to have the new Speed percent.
	 * @param fBeat the beat to have the new Speed percent.
	 * @param fPercent the percent.
	 */
	void SetSpeedPercentAtBeat( float fBeat, float fPercent ) { SetSpeedPercentAtRow( BeatToNoteRow(fBeat), fPercent); }
	/**
	 * @brief Set the row to have the new Speed wait.
	 * @param iNoteRow the row to have the new Speed wait.
	 * @param fWait the wait.
	 */
	void SetSpeedWaitAtRow( int iNoteRow, float fWait );
	/**
	 * @brief Set the beat to have the new Speed wait.
	 * @param fBeat the beat to have the new Speed wait.
	 * @param fWait the wait.
	 */
	void SetSpeedWaitAtBeat( float fBeat, float fWait ) { SetSpeedWaitAtRow( BeatToNoteRow(fBeat), fWait); }
	/**
	 * @brief Set the row to have the new Speed mode.
	 * @param iNoteRow the row to have the new Speed mode.
	 * @param usMode the mode.
	 */
	void SetSpeedModeAtRow( int iNoteRow, unsigned short usMode );
	/**
	 * @brief Set the beat to have the new Speed mode.
	 * @param fBeat the beat to have the new Speed mode.
	 * @param usMode the mode.
	 */
	void SetSpeedModeAtBeat( float fBeat, unsigned short usMode ) { SetSpeedModeAtRow( BeatToNoteRow(fBeat), usMode); }
	/**
	 * @brief Retrieve the SpeedSegment at the specified row.
	 * @param iNoteRow the row that has a SpeedSegment.
	 * @return the SpeedSegment in question.
	 */
	SpeedSegment* GetSpeedSegmentAtRow( int iNoteRow );
	/**
	 * @brief Retrieve the SpeedSegment at the specified beat.
	 * @param fBeat the beat that has a SpeedSegment.
	 * @return the SpeedSegment in question.
	 */
	SpeedSegment* GetSpeedSegmentAtBeat( float fBeat ) { return GetSpeedSegmentAtRow( BeatToNoteRow(fBeat) ); }
	
	float GetDisplayedSpeedPercent( float fBeat, float fMusicSeconds ) const;
	
	/**
	 * @brief Retrieve the scrolling factor at the given row.
	 * @param iNoteRow the row in question.
	 * @return the percent.
	 */
	float GetScrollAtRow( int iNoteRow );
	/**
	 * @brief Retrieve the scrolling factor at the given beat.
	 * @param fBeat the beat in question.
	 * @return the percent.
	 */
	float GetScrollAtBeat( float fBeat ) { return GetScrollAtRow( BeatToNoteRow(fBeat) ); }

	/**
	 * @brief Set the row to have the new Scrolling factor.
	 * @param iNoteRow the row to have the new Speed.
	 * @param fPercent the scrolling factor.
	 */
	void SetScrollAtRow( int iNoteRow, float fPercent );
	/**
	 * @brief Set the row to have the new Scrolling factor.
	 * @param iNoteRow the row to have the new Speed.
	 * @param fPercent the scrolling factor.
	 */
	void SetScrollAtBeat( float fBeat, float fPercent ) { SetScrollAtRow( BeatToNoteRow(fBeat), fPercent ); }
	
	/**
	 * @brief Retrieve the ScrollSegment at the specified row.
	 * @param iNoteRow the row that has a ScrollSegment.
	 * @return the ScrollSegment in question.
	 */
	ScrollSegment* GetScrollSegmentAtRow( int iNoteRow );
	/**
	 * @brief Retrieve the ScrollSegment at the specified beat.
	 * @param fBeat the beat that has a ScrollSegment.
	 * @return the ScrollSegment in question.
	 */
	ScrollSegment* GetScrollSegmentAtBeat( float fBeat ) { return GetScrollSegmentAtRow( BeatToNoteRow(fBeat) ); }
	
	/**
	 * @brief Determine when the fakes end.
	 * @param iRow The row you start on.
	 * @return the time when the fakes end.
	 */
	float GetFakeAtRow( int iRow ) const;
	/**
	 * @brief Determine when the fakes end.
	 * @param fBeat The beat you start on.
	 * @return the time when the fakes end.
	 */
	float GetFakeAtBeat( float fBeat ) const { return GetFakeAtRow( BeatToNoteRow( fBeat ) ); }
	/**
	 * @brief Set the beat to indicate when the FakeSegment ends.
	 * @param iRow The row to start on.
	 * @param fNew The destination beat.
	 */
	void SetFakeAtRow( int iRow, float fNew );
	/**
	 * @brief Set the beat to indicate when the FakeSegment ends.
	 * @param fBeat The beat to start on.
	 * @param fNew The destination beat.
	 */
	void SetFakeAtBeat( float fBeat, float fNew ) { SetFakeAtRow( BeatToNoteRow( fBeat ), fNew ); }
	/**
	 * @brief Retrieve the FakeSegment at the specified row.
	 * @param iRow the row to focus on.
	 * @return the FakeSegment in question.
	 */
	FakeSegment* GetFakeSegmentAtRow( int iRow );
	/**
	 * @brief Retrieve the FakeSegment at the specified beat.
	 * @param fBeat the beat to focus on.
	 * @return the FakeSegment in question.
	 */
	FakeSegment* GetFakeSegmentAtBeat( float fBeat ) { return GetFakeSegmentAtRow( BeatToNoteRow( fBeat ) ); }
	/**
	 * @brief Checks if the row is inside a fake.
	 * @param iRow the row to focus on.
	 * @return true if the row is inside a fake, false otherwise.
	 */
	bool IsFakeAtRow( int iRow ) const;
	/**
	 * @brief Checks if the beat is inside a fake.
	 * @param fBeat the beat to focus on.
	 * @return true if the row is inside a fake, false otherwise.
	 */
	bool IsFakeAtBeat( float fBeat ) const { return IsFakeAtRow( BeatToNoteRow( fBeat ) ); }
	
	/**
	 * @brief Determine if this notes on this row can be judged.
	 * @param row the row to focus on.
	 * @return true if the row can be judged, false otherwise. */
	bool IsJudgableAtRow( int row ) const
	{
		return !(IsWarpAtRow(row) || IsFakeAtRow(row));
	}
	/**
	 * @brief Determine if this notes on this beat can be judged.
	 * @param beat the beat to focus on.
	 * @return true if the row can be judged, false otherwise. */
	bool IsJudgableAtBeat( float beat ) const { return IsJudgableAtRow( BeatToNoteRow( beat ) ); }
	
	
	
	void MultiplyBPMInBeatRange( int iStartIndex, int iEndIndex, float fFactor );
	
	void NoteRowToMeasureAndBeat( int iNoteRow, int &iMeasureIndexOut, int &iBeatIndexOut, int &iRowsRemainder ) const;

	void GetBeatAndBPSFromElapsedTime( float fElapsedTime, float &fBeatOut, float &fBPSOut, bool &bFreezeOut, bool &bDelayOut, int &iWarpBeginOut, float &fWarpLengthOut ) const;
	float GetBeatFromElapsedTime( float fElapsedTime ) const	// shortcut for places that care only about the beat
	{
		float fBeat, fThrowAway, fThrowAway2;
		bool bThrowAway, bThrowAway2;
		int iThrowAway;
		GetBeatAndBPSFromElapsedTime( fElapsedTime, fBeat, fThrowAway, bThrowAway, bThrowAway2, iThrowAway, fThrowAway2 );
		return fBeat;
	}
	float GetElapsedTimeFromBeat( float fBeat ) const;

	void GetBeatAndBPSFromElapsedTimeNoOffset( float fElapsedTime, float &fBeatOut, float &fBPSOut, bool &bFreezeOut, bool &bDelayOut, int &iWarpBeginOut, float &fWarpDestinationOut ) const;
	float GetBeatFromElapsedTimeNoOffset( float fElapsedTime ) const	// shortcut for places that care only about the beat
	{
		float fBeat, fThrowAway, fThrowAway2;
		bool bThrowAway, bThrowAway2;
		int iThrowAway;
		GetBeatAndBPSFromElapsedTimeNoOffset( fElapsedTime, fBeat, fThrowAway, bThrowAway, bThrowAway2, iThrowAway, fThrowAway2 );
		return fBeat;
	}
	float GetElapsedTimeFromBeatNoOffset( float fBeat ) const;
	float GetDisplayedBeat( float fBeat ) const;
	/**
	 * @brief View the TimingData to see if a song changes its BPM at any point.
	 * @return true if there is at least one change, false otherwise.
	 */
	bool HasBpmChanges() const;
	/**
	 * @brief View the TimingData to see if there is at least one stop at any point.
	 * @return true if there is at least one stop, false otherwise.
	 */
	bool HasStops() const;
	/**
	 * @brief View the TimingData to see if there is at least one delay at any point.
	 * @return true if there is at least one delay, false otherwise.
	 */
	bool HasDelays() const;
	/**
	 * @brief View the TimingData to see if there is at least one warp at any point.
	 * @return true if there is at least one warp, false otherwise.
	 */
	bool HasWarps() const;
	/**
	 * @brief View the TimingData to see if there is at least one fake segment involved.
	 * @return true if there is at least one fake segment, false otherwise. */
	bool HasFakes() const;
	/**
	 * @brief View the TimingData to see if a song changes its speed scrolling at any point.
	 * @return true if there is at least one change, false otherwise. */
	bool HasSpeedChanges() const;
	/**
	 * @brief View the TimingData to see if a song changes its speed scrolling at any point.
	 * @return true if there is at least one change, false otherwise. */
	bool HasScrollChanges() const;
	/**
	 * @brief Compare two sets of timing data to see if they are equal.
	 *
	 * TODO: Make doubly sure that we're using the operator== on the proper segments,
	 * not the generic TimingSegment.
	 * @param other the other TimingData.
	 * @return the equality or lack thereof of the two TimingData.
	 */
	bool operator==( const TimingData &other )
	{
		unsigned i;
		
		COMPARE(allTimingSegments[SEGMENT_BPM].size());
		for (i = 0; i < allTimingSegments[SEGMENT_BPM].size(); i++)
		{
			BPMSegment * segT =
				static_cast<BPMSegment *>(this->allTimingSegments[SEGMENT_BPM][i]);
			BPMSegment * segO =
				static_cast<BPMSegment *>(other.allTimingSegments[SEGMENT_BPM][i]);
			if (segT->GetRow() != segO->GetRow()) return false;
			if (segT->GetBPS() != segO->GetBPS()) return false;
		}
		
		COMPARE(allTimingSegments[SEGMENT_STOP].size());
		for (i = 0; i < allTimingSegments[SEGMENT_STOP].size(); i++)
		{
			StopSegment * segT =
				static_cast<StopSegment *>(this->allTimingSegments[SEGMENT_STOP][i]);
			StopSegment * segO =
				static_cast<StopSegment *>(other.allTimingSegments[SEGMENT_STOP][i]);
			if (segT->GetRow() != segO->GetRow()) return false;
			if (segT->GetPause() != segO->GetPause()) return false;
		}
		
		COMPARE(allTimingSegments[SEGMENT_DELAY].size());
		for (i = 0; i < allTimingSegments[SEGMENT_DELAY].size(); i++)
		{
			DelaySegment * segT =
				static_cast<DelaySegment *>(this->allTimingSegments[SEGMENT_DELAY][i]);
			DelaySegment * segO =
				static_cast<DelaySegment *>(other.allTimingSegments[SEGMENT_DELAY][i]);
			if (segT->GetRow() != segO->GetRow()) return false;
			if (segT->GetPause() != segO->GetPause()) return false;
		}
		
		COMPARE(allTimingSegments[SEGMENT_WARP].size());
		for (i = 0; i < allTimingSegments[SEGMENT_WARP].size(); i++)
		{
			WarpSegment * segT =
				static_cast<WarpSegment *>(this->allTimingSegments[SEGMENT_WARP][i]);
			WarpSegment * segO =
				static_cast<WarpSegment *>(other.allTimingSegments[SEGMENT_WARP][i]);
			if (segT->GetRow() != segO->GetRow()) return false;
			if (segT->GetLength() != segO->GetLength()) return false;
		}
		
		COMPARE(allTimingSegments[SEGMENT_TIME_SIG].size());
		for (i = 0; i < allTimingSegments[SEGMENT_TIME_SIG].size(); i++)
		{
			TimeSignatureSegment * segT =
				static_cast<TimeSignatureSegment *>(this->allTimingSegments[SEGMENT_TIME_SIG][i]);
			TimeSignatureSegment * segO =
				static_cast<TimeSignatureSegment *>(other.allTimingSegments[SEGMENT_TIME_SIG][i]);
			if (segT->GetRow() != segO->GetRow()) return false;
			if (segT->GetNum() != segO->GetNum()) return false;
			if (segT->GetDen() != segO->GetDen()) return false;
		}
		
		COMPARE(allTimingSegments[SEGMENT_TICKCOUNT].size());
		for (i = 0; i < allTimingSegments[SEGMENT_TICKCOUNT].size(); i++)
		{
			TickcountSegment * segT =
				static_cast<TickcountSegment *>(this->allTimingSegments[SEGMENT_TICKCOUNT][i]);
			TickcountSegment * segO =
				static_cast<TickcountSegment *>(other.allTimingSegments[SEGMENT_TICKCOUNT][i]);
			if (segT->GetRow() != segO->GetRow()) return false;
			if (segT->GetTicks() != segO->GetTicks()) return false;
		}
		
		COMPARE(allTimingSegments[SEGMENT_COMBO].size());
		for (i = 0; i < allTimingSegments[SEGMENT_COMBO].size(); i++)
		{
			ComboSegment * segT =
				static_cast<ComboSegment *>(this->allTimingSegments[SEGMENT_COMBO][i]);
			ComboSegment * segO =
				static_cast<ComboSegment *>(other.allTimingSegments[SEGMENT_COMBO][i]);
			if (segT->GetRow() != segO->GetRow()) return false;
			if (segT->GetCombo() != segO->GetCombo()) return false;
			if (segT->GetMissCombo() != segO->GetMissCombo()) return false;
		}
		
		COMPARE(allTimingSegments[SEGMENT_SPEED].size());
		for (i = 0; i < allTimingSegments[SEGMENT_SPEED].size(); i++)
		{
			SpeedSegment * segT =
				static_cast<SpeedSegment *>(this->allTimingSegments[SEGMENT_SPEED][i]);
			SpeedSegment * segO =
				static_cast<SpeedSegment *>(other.allTimingSegments[SEGMENT_SPEED][i]);
			if (segT->GetRow() != segO->GetRow()) return false;
			if (segT->GetRatio() != segO->GetRatio()) return false;
			if (segT->GetLength() != segO->GetLength()) return false;
			if (segT->GetUnit() != segO->GetUnit()) return false;
		}
		
		COMPARE(allTimingSegments[SEGMENT_SCROLL].size());
		for (i = 0; i < allTimingSegments[SEGMENT_SCROLL].size(); i++)
		{
			ScrollSegment * segT =
				static_cast<ScrollSegment *>(this->allTimingSegments[SEGMENT_SCROLL][i]);
			ScrollSegment * segO =
				static_cast<ScrollSegment *>(other.allTimingSegments[SEGMENT_SCROLL][i]);
			if (segT->GetRow() != segO->GetRow()) return false;
			if (segT->GetRatio() != segO->GetRatio()) return false;
		}
		
		COMPARE(allTimingSegments[SEGMENT_LABEL].size());
		for (i = 0; i < allTimingSegments[SEGMENT_LABEL].size(); i++)
		{
			LabelSegment * segT =
				static_cast<LabelSegment *>(this->allTimingSegments[SEGMENT_LABEL][i]);
			LabelSegment * segO =
				static_cast<LabelSegment *>(other.allTimingSegments[SEGMENT_LABEL][i]);
			if (segT->GetRow() != segO->GetRow()) return false;
			if (segT->GetLabel() != segO->GetLabel()) return false;
		}
		
		COMPARE(allTimingSegments[SEGMENT_FAKE].size());
		for (i = 0; i < allTimingSegments[SEGMENT_FAKE].size(); i++)
		{
			FakeSegment * segT =
				static_cast<FakeSegment *>(this->allTimingSegments[SEGMENT_FAKE][i]);
			FakeSegment * segO =
				static_cast<FakeSegment *>(other.allTimingSegments[SEGMENT_FAKE][i]);
			if (segT->GetRow() != segO->GetRow()) return false;
			if (segT->GetLength() != segO->GetLength()) return false;
		}
		
		COMPARE( m_fBeat0OffsetInSeconds );
		return true;
	}
	/**
	 * @brief Compare two sets of timing data to see if they are not equal.
	 * @param other the other TimingData.
	 * @return the inequality or lack thereof of the two TimingData.
	 */
	bool operator!=( const TimingData &other ) { return !operator==(other); }
	
	void ScaleRegion( float fScale = 1, int iStartRow = 0, int iEndRow = MAX_NOTE_ROW, bool bAdjustBPM = false );
	void InsertRows( int iStartRow, int iRowsToAdd );
	void DeleteRows( int iStartRow, int iRowsToDelete );

	/**
	 * @brief Tidy up the timing data, e.g. provide default BPMs, labels, tickcounts.
	 */
	void TidyUpData();

	// Lua
	void PushSelf( lua_State *L );
	/**
	 * @brief The file of the song/steps that use this TimingData.
	 *
	 * This is for informational purposes only.
	 */
	RString					m_sFile;
	// All of the following vectors must be sorted before gameplay.

	
	vector<TimingSegment *> allTimingSegments[NUM_TimingSegmentType];
	
	/**
	 * @brief The initial offset of a song.
	 */
	float	m_fBeat0OffsetInSeconds;
	
	vector<RString> ToVectorString(TimingSegmentType tst, int dec = 6) const;	
};

#undef COMPARE

#endif

/**
 * @file
 * @author Chris Danford, Glenn Maynard (c) 2001-2004 
 * @section LICENSE
 * All rights reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, and/or sell copies of the Software, and to permit persons to
 * whom the Software is furnished to do so, provided that the above
 * copyright notice(s) and this permission notice appear in all copies of
 * the Software and that both the above copyright notice(s) and this
 * permission notice appear in supporting documentation.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF
 * THIRD PARTY RIGHTS. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR HOLDERS
 * INCLUDED IN THIS NOTICE BE LIABLE FOR ANY CLAIM, OR ANY SPECIAL INDIRECT
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

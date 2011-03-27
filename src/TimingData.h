#ifndef TIMING_DATA_H
#define TIMING_DATA_H

#include "NoteTypes.h"
#include "PrefsManager.h"
struct lua_State;

/** @brief Compare a TimingData segment's properties with one another. */
#define COMPARE(x) if(x!=other.x) return false;

/**
 * @brief Identifies when a song changes its BPM.
 */
struct BPMSegment
{
	/**
	 * @brief Creates a simple BPM Segment with default values.
	 *
	 * It is best to override the values as soon as possible.
	 */
	BPMSegment() : m_iStartRow(-1), m_fBPS(-1.0f) { }
	/**
	 * @brief Creates a BPM Segment with the specified starting row and beats per second.
	 * @param s the starting row of this segment.
	 * @param b the beats per second to be turned into beats per minute.
	 */
	BPMSegment( int s, float b ): m_iStartRow(max(0, s)), m_fBPS(0) { SetBPM( b ); }
	/**
	 * @brief The row in which the BPMSegment activates.
	 */
	int m_iStartRow;
	/**
	 * @brief The BPS to use when this row is reached.
	 */
	float m_fBPS;
	
	/**
	 * @brief Converts the BPS to a BPM.
	 * @param f The BPM.
	 */
	void SetBPM( float f ) { m_fBPS = f / 60.0f; }
	/**
	 * @brief Retrieves the BPM from the BPS.
	 * @return the BPM.
	 */
	float GetBPM() const { return m_fBPS * 60.0f; }

	/**
	 * @brief Compares two BPMSegments to see if they are equal to each other.
	 * @param other the other BPMSegment to compare to.
	 * @return the equality of the two segments.
	 */
	bool operator==( const BPMSegment &other ) const
	{
		COMPARE( m_iStartRow );
		COMPARE( m_fBPS );
		return true;
	}
	/**
	 * @brief Compares two BPMSegments to see if they are not equal to each other.
	 * @param other the other BPMSegment to compare to.
	 * @return the inequality of the two segments.
	 */
	bool operator!=( const BPMSegment &other ) const { return !operator==(other); }
	/**
	 * @brief Compares two BPMSegments to see if one is less than the other.
	 * @param other the other BPMSegment to compare to.
	 * @return the truth/falsehood of if the first is less than the second.
	 */
	bool operator<( const BPMSegment &other ) const
	{
		return m_iStartRow < other.m_iStartRow || 
		( m_iStartRow == other.m_iStartRow && m_fBPS < other.m_fBPS );
	}
	/**
	 * @brief Compares two BPMSegments to see if one is less than or equal to the other.
	 * @param other the other BPMSegment to compare to.
	 * @return the truth/falsehood of if the first is less or equal to than the second.
	 */
	bool operator<=( const BPMSegment &other ) const
	{
		return ( operator<(other) || operator==(other) );
	}
	/**
	 * @brief Compares two BPMSegments to see if one is greater than the other.
	 * @param other the other BPMSegment to compare to.
	 * @return the truth/falsehood of if the first is greater than the second.
	 */
	bool operator>( const BPMSegment &other ) const { return !operator<=(other); }
	/**
	 * @brief Compares two BPMSegments to see if one is greater than or equal to the other.
	 * @param other the other BPMSegment to compare to.
	 * @return the truth/falsehood of if the first is greater than or equal to the second.
	 */
	bool operator>=( const BPMSegment &other ) const { return !operator<(other); }
};
/**
 * @brief Identifies when a song has a stop or a delay.
 *
 * It is hopeful that stops and delays can be made into their own segments at some point.
 */
struct StopSegment
{
	/**
	 * @brief Creates a simple Stop Segment with default values.
	 *
	 * It is best to override the values as soon as possible.
	 */
	StopSegment() : m_iStartRow(-1), m_fStopSeconds(-1.0f), m_bDelay(false)  { }
	/**
	 * @brief Creates a Stop Segment at the specified row for the specified length of time.
	 *
	 * This will not create a dedicated delay segment. Use the third constructor for
	 * making delays.
	 * @param s the starting row of this segment.
	 * @param f the length of time to pause the note scrolling.
	 */
	StopSegment( int s, float f ) : m_iStartRow(max(0, s)), 
		m_fStopSeconds(f), m_bDelay(false)
	{
		if (!PREFSMAN->m_bQuirksMode) m_fStopSeconds = max( 0.0f, f );
	}
	/**
	 * @brief Creates a Stop (or Delay) Segment at the specified row for the specified length of time.
	 * @param s the starting row of this segment.
	 * @param f the length of time to pause the note scrolling.
	 * @param d the flag that makes this Stop Segment a Delay Segment.
	 */
	StopSegment( int s, float f, bool d ) : m_iStartRow(max(0, s)), 
		m_fStopSeconds(f), m_bDelay(d)
	{
		if (!PREFSMAN->m_bQuirksMode) m_fStopSeconds = max( 0.0f, f );
	}
	/**
	 * @brief The row in which the StopSegment activates.
	 */
	int m_iStartRow;
	/**
	 * @brief The amount of time to complete the pause at the given row.
	 */
	float m_fStopSeconds;
	/**
	 * @brief If true, the Stop Segment is treated as a Delay Segment, similar to the Pump It Up series.
	 */
	bool m_bDelay;
	/**
	 * @brief Compares two StopSegments to see if they are equal to each other.
	 * @param other the other StopSegment to compare to.
	 * @return the equality of the two segments.
	 */
	bool operator==( const StopSegment &other ) const
	{
		COMPARE( m_iStartRow );
		COMPARE( m_fStopSeconds );
		COMPARE( m_bDelay );
		return true;
	}
	/**
	 * @brief Compares two StopSegments to see if they are not equal to each other.
	 * @param other the other StopSegment to compare to.
	 * @return the inequality of the two segments.
	 */
	bool operator!=( const StopSegment &other ) const { return !operator==(other); }
	/**
	 * @brief Compares two StopSegments to see if one is less than the other.
	 *
	 * It should be observed that Delay Segments have to come before Stop Segments.
	 * Otherwise, it will act like a Stop Segment with extra time from the Delay at
	 * the same row.
	 * @param other the other StopSegment to compare to.
	 * @return the truth/falsehood of if the first is less than the second.
	 */
	bool operator<( const StopSegment &other ) const
	{
		return ( m_iStartRow < other.m_iStartRow ) ||
		( m_iStartRow == other.m_iStartRow && 
		 ( ( m_bDelay && !other.m_bDelay ) || m_fStopSeconds < other.m_fStopSeconds ));
	}
	/**
	 * @brief Compares two StopSegments to see if one is less than or equal to the other.
	 * @param other the other StopSegment to compare to.
	 * @return the truth/falsehood of if the first is less or equal to than the second.
	 */
	bool operator<=( const StopSegment &other ) const
	{
		return ( operator<(other) || operator==(other) );
	}
	/**
	 * @brief Compares two StopSegments to see if one is greater than the other.
	 *
	 * Similar to the less than operator function, stops must come after delays
	 * to avoid rendering the point of delays pointless.
	 * @param other the other StopSegment to compare to.
	 * @return the truth/falsehood of if the first is greater than the second.
	 */
	bool operator>( const StopSegment &other ) const { return !operator<=(other); }
	/**
	 * @brief Compares two StopSegments to see if one is greater than or equal to the other.
	 * @param other the other StopSegment to compare to.
	 * @return the truth/falsehood of if the first is greater than or equal to the second.
	 */
	bool operator>=( const StopSegment &other ) const { return !operator<(other); }
};

/**
 * @brief Identifies when a song changes its time signature.
 *
 * This only supports simple time signatures. The upper number (called the numerator here, though this isn't
 * properly a fraction) is the number of beats per measure. The lower number (denominator here)
 * is the note value representing one beat. */
struct TimeSignatureSegment
{
	/**
	 * @brief Creates a simple Time Signature Segment with default values.
	 */
	TimeSignatureSegment() : m_iStartRow(-1), m_iNumerator(4), m_iDenominator(4)  { }
	/**
	 * @brief Creates a Time Signature Segment at the given row with a supplied numerator.
	 *
	 * The denominator will be 4 if this is called.
	 * @param r the starting row of the segment.
	 * @param n the numerator for the segment.
	 */
	TimeSignatureSegment( int r, int n ): m_iStartRow(max(0, r)),
		m_iNumerator(max(1, n)), m_iDenominator(4) {}
	/**
	 * @brief Creates a Time Signature Segment at the given row with a supplied numerator & denominator.
	 * @param r the starting row of the segment.
	 * @param n the numerator for the segment.
	 * @param d the denonimator for the segment.
	 */
	TimeSignatureSegment( int r, int n, int d ): m_iStartRow(max(0, r)),
	m_iNumerator(max(1, n)), m_iDenominator(max(1, d)) {}
	/**
	 * @brief The row in which the TimeSignatureSegment activates.
	 */
	int m_iStartRow;
	/**
	 * @brief The numerator of the TimeSignatureSegment.
	 */
	int m_iNumerator;
	/**
	 * @brief The denominator of the TimeSignatureSegment.
	 */
	int m_iDenominator;

	/**
	 * @brief Retrieve the number of note rows per measure within the TimeSignatureSegment.
	 * 
	 * With BeatToNoteRow(1) rows per beat, then we should have BeatToNoteRow(1)*m_iNumerator
	 * beats per measure. But if we assume that every BeatToNoteRow(1) rows is a quarter note,
	 * and we want the beats to be 1/m_iDenominator notes, then we should have
	 * BeatToNoteRow(1)*4 is rows per whole note and thus BeatToNoteRow(1)*4/m_iDenominator is
	 * rows per beat. Multiplying by m_iNumerator gives rows per measure.
	 * @returns the number of note rows per measure.
	 */
	int GetNoteRowsPerMeasure() const { return BeatToNoteRow(1) * 4 * m_iNumerator / m_iDenominator; }
	/**
	 * @brief Compares two TimeSignatureSegments to see if they are equal to each other.
	 * @param other the other TimeSignatureSegment to compare to.
	 * @return the equality of the two segments.
	 */
	bool operator==( const TimeSignatureSegment &other ) const
	{
		COMPARE( m_iStartRow );
		COMPARE( m_iNumerator );
		COMPARE( m_iDenominator );
		return true;
	}
	/**
	 * @brief Compares two TimeSignatureSegments to see if they are not equal to each other.
	 * @param other the other TimeSignatureSegment to compare to.
	 * @return the inequality of the two segments.
	 */
	bool operator!=( const TimeSignatureSegment &other ) const { return !operator==(other); }
	/**
	 * @brief Compares two TimeSignatureSegments to see if one is less than the other.
	 * @param other the other TimeSignatureSegment to compare to.
	 * @return the truth/falsehood of if the first is less than the second.
	 */
	bool operator<( const TimeSignatureSegment &other ) const
	{ 
		return m_iStartRow < other.m_iStartRow ||
		( m_iStartRow == other.m_iStartRow && 
		 ( m_iNumerator < other.m_iNumerator || 
		  ( m_iNumerator == other.m_iNumerator && m_iDenominator < other.m_iDenominator )));
	}
	/**
	 * @brief Compares two TimeSignatureSegments to see if one is less than or equal to the other.
	 * @param other the other TimeSignatureSegment to compare to.
	 * @return the truth/falsehood of if the first is less or equal to than the second.
	 */
	bool operator<=( const TimeSignatureSegment &other ) const
	{
		return ( operator<(other) || operator==(other) );
	}
	/**
	 * @brief Compares two TimeSignatureSegments to see if one is greater than the other.
	 * @param other the other TimeSignatureSegment to compare to.
	 * @return the truth/falsehood of if the first is greater than the second.
	 */
	bool operator>( const TimeSignatureSegment &other ) const { return !operator<=(other); }
	/**
	 * @brief Compares two TimeSignatureSegments to see if one is greater than or equal to the other.
	 * @param other the other TimeSignatureSegment to compare to.
	 * @return the truth/falsehood of if the first is greater than or equal to the second.
	 */
	bool operator>= (const TimeSignatureSegment &other ) const { return !operator<(other); }
};

/**
 * @brief Identifies when a song needs to warp to a new beat.
 *
 * A warp segment is used to replicate the effects of Negative BPMs without
 * abusing negative BPMs. Negative BPMs should be converted to warp segments.
 * WarpAt=WarpTo is the format, where both are in beats. (Technically they're
 * both rows though.) */
struct WarpSegment
{
	/**
	 * @brief Create a simple Warp Segment with default values.
	 *
	 * It is best to override the values as soon as possible.
	 */
	WarpSegment() : m_iStartRow(-1), m_fEndBeat(-1) { }
	/**
	 * @brief Create a Warp Segment with the specified starting row and row to warp to.
	 * @param s the starting row of this segment.
	 * @param r the row to warp to.
	 */
	WarpSegment( int s, int r ): m_iStartRow(max(0, (s < r ? s : r))),
		 m_fEndBeat(max(0, NoteRowToBeat((r > s ? r : s)))) {}
	/**
	 * @brief Creates a Warp Segment with the specified starting row and beat to warp to.
	 * @param s the starting row of this segment.
	 * @param b the beat to warp to.
	 */
	WarpSegment( int s, float b ): m_iStartRow(max(0, s)),
		m_fEndBeat(max(0, b)) {}
	/**
	 * @brief Create a Warp Segment with the specified starting beat and row to warp to.
	 * @param s the starting beat in this segment.
	 * @param r the row to warp to.
	 */
	WarpSegment( float s, int r ):
		m_iStartRow(max(0, BeatToNoteRow(s))),
		m_fEndBeat(max(0, NoteRowToBeat(r))) {}
	/**
	 * @brief Creates a Warp Segment with the specified starting beat and beat to warp to.
	 * @param s the starting beat of this segment.
	 * @param b the beat to warp to.
	 */
	WarpSegment( float s, float b ):
		m_iStartRow(max(0, BeatToNoteRow((s < b ? s : b)))),
		m_fEndBeat(max(0, (b > s ? b : s))) {}
	/**
	 * @brief The row in which the WarpSegment activates.
	 */
	int m_iStartRow;
	/**
	 * @brief The beat to warp to.
	 */
	float m_fEndBeat;
	/**
	 * @brief Compares two WarpSegments to see if they are equal to each other.
	 * @param other the other WarpSegment to compare to.
	 * @return the equality of the two segments.
	 */
	bool operator==( const WarpSegment &other ) const
	{
		COMPARE( m_iStartRow );
		COMPARE( m_fEndBeat );
		return true;
	}
	/**
	 * @brief Compares two WarpSegments to see if they are not equal to each other.
	 * @param other the other WarpSegment to compare to.
	 * @return the inequality of the two segments.
	 */
	bool operator!=( const WarpSegment &other ) const { return !operator==(other); }
	/**
	 * @brief Compares two WarpSegments to see if one is less than the other.
	 * @param other the other WarpSegment to compare to.
	 * @return the truth/falsehood of if the first is less than the second.
	 */
	bool operator<( const WarpSegment &other ) const
	{ 
		return m_iStartRow < other.m_iStartRow ||
		( m_iStartRow == other.m_iStartRow && m_fEndBeat < other.m_fEndBeat );
	}
	/**
	 * @brief Compares two WarpSegments to see if one is less than or equal to the other.
	 * @param other the other WarpSegment to compare to.
	 * @return the truth/falsehood of if the first is less or equal to than the second.
	 */
	bool operator<=( const WarpSegment &other ) const
	{
		return ( operator<(other) || operator==(other) );
	}
	/**
	 * @brief Compares two WarpSegments to see if one is greater than the other.
	 * @param other the other WarpSegment to compare to.
	 * @return the truth/falsehood of if the first is greater than the second.
	 */
	bool operator>( const WarpSegment &other ) const { return !operator<=(other); }
	/**
	 * @brief Compares two WarpSegments to see if one is greater than or equal to the other.
	 * @param other the other WarpSegment to compare to.
	 * @return the truth/falsehood of if the first is greater than or equal to the second.
	 */
	bool operator>=( const WarpSegment &other ) const { return !operator<(other); }
};

/**
 * @brief Identifies when a chart is to have a different tickcount value for hold notes.
 * 
 * A tickcount segment is used to better replicate the checkpoint hold
 * system used by various based video games. The number is used to 
 * represent how many ticks can be counted in one beat.
 */
struct TickcountSegment
{
	/**
	 * @brief Creates a simple Tickcount Segment with default values.
	 *
	 * It is best to override the values as soon as possible.
	 */
	TickcountSegment() : m_iStartRow(-1), m_iTicks(2) { }
	/**
	 * @brief Creates a Tickcount Segment with the specified starting row and beats per second.
	 * @param s the starting row of this segment.
	 * @param t the amount of ticks counted per beat.
	 */
	TickcountSegment( int s, int t ): m_iStartRow(max(0, s)), 
		m_iTicks(max(0, t)) {}
	/**
	 * @brief The row in which the TickcountSegment activates.
	 */
	int m_iStartRow;
	/**
	 * @brief The amount of ticks counted per beat.
	 */
	int m_iTicks;
	
	/**
	 * @brief Compares two TickcountSegments to see if they are equal to each other.
	 * @param other the other TickcountSegment to compare to.
	 * @return the equality of the two segments.
	 */
	bool operator==( const TickcountSegment &other ) const
	{
		COMPARE( m_iStartRow );
		COMPARE( m_iTicks );
		return true;
	}
	/**
	 * @brief Compares two TickcountSegments to see if they are not equal to each other.
	 * @param other the other TickcountSegment to compare to.
	 * @return the inequality of the two segments.
	 */
	bool operator!=( const TickcountSegment &other ) const { return !operator==(other); }
	/**
	 * @brief Compares two TickcountSegments to see if one is less than the other.
	 * @param other the other TickcountSegment to compare to.
	 * @return the truth/falsehood of if the first is less than the second.
	 */
	bool operator<( const TickcountSegment &other ) const { return m_iStartRow < other.m_iStartRow; }
	/**
	 * @brief Compares two TickcountSegments to see if one is less than or equal to the other.
	 * @param other the other TickcountSegment to compare to.
	 * @return the truth/falsehood of if the first is less or equal to than the second.
	 */
	bool operator<=( const TickcountSegment &other ) const
	{
		return ( operator<(other) || operator==(other) );
	}
	/**
	 * @brief Compares two TickcountSegments to see if one is greater than the other.
	 * @param other the other TickcountSegment to compare to.
	 * @return the truth/falsehood of if the first is greater than the second.
	 */
	bool operator>( const TickcountSegment &other ) const { return !operator<=(other); }
	/**
	 * @brief Compares two TickcountSegments to see if one is greater than or equal to the other.
	 * @param other the other TickcountSegment to compare to.
	 * @return the truth/falsehood of if the first is greater than or equal to the second.
	 */
	bool operator>=( const TickcountSegment &other ) const { return !operator<(other); }
};

/**
 * @brief Identifies when a chart is to have a different combo multiplier value.
 * 
 * Admitedly, this would primarily be used for mission mode style charts. However,
 * it can have its place during normal gameplay.
 */
struct ComboSegment
{
	/**
	 * @brief Creates a simple Combo Segment with default values.
	 *
	 * It is best to override the values as soon as possible.
	 */
	ComboSegment() : m_iStartRow(-1), m_iCombo(1) { }
	/**
	 * @brief Creates a Combo Segment with the specified starting row and combo factor.
	 * @param s the starting row of this segment.
	 * @param t the amount the combo increases on a succesful hit.
	 */
	ComboSegment( int s, int t ): m_iStartRow(max(0, s)),
		m_iCombo(max(1,t)) {}
	/**
	 * @brief The row in which the ComboSegment activates.
	 */
	int m_iStartRow;
	/**
	 * @brief The amount the combo increases at this point.
	 */
	int m_iCombo;
	
	/**
	 * @brief Compares two ComboSegments to see if they are equal to each other.
	 * @param other the other ComboSegment to compare to.
	 * @return the equality of the two segments.
	 */
	bool operator==( const ComboSegment &other ) const
	{
		COMPARE( m_iStartRow );
		COMPARE( m_iCombo );
		return true;
	}
	/**
	 * @brief Compares two ComboSegments to see if they are not equal to each other.
	 * @param other the other ComboSegment to compare to.
	 * @return the inequality of the two segments.
	 */
	bool operator!=( const ComboSegment &other ) const { return !operator==(other); }
	/**
	 * @brief Compares two ComboSegments to see if one is less than the other.
	 * @param other the other ComboSegment to compare to.
	 * @return the truth/falsehood of if the first is less than the second.
	 */
	bool operator<( const ComboSegment &other ) const { return m_iStartRow < other.m_iStartRow; }
	/**
	 * @brief Compares two ComboSegments to see if one is less than or equal to the other.
	 * @param other the other ComboSegment to compare to.
	 * @return the truth/falsehood of if the first is less or equal to than the second.
	 */
	bool operator<=( const ComboSegment &other ) const
	{
		return ( operator<(other) || operator==(other) );
	}
	/**
	 * @brief Compares two ComboSegments to see if one is greater than the other.
	 * @param other the other ComboSegment to compare to.
	 * @return the truth/falsehood of if the first is greater than the second.
	 */
	bool operator>( const ComboSegment &other ) const { return !operator<=(other); }
	/**
	 * @brief Compares two ComboSegments to see if one is greater than or equal to the other.
	 * @param other the other ComboSegment to compare to.
	 * @return the truth/falsehood of if the first is greater than or equal to the second.
	 */
	bool operator>=( const ComboSegment &other ) const { return !operator<(other); }
};

/**
 * @brief Holds data for translating beats<->seconds.
 */
class TimingData
{
public:
	/**
	 * @brief Sets up initial timing data.
	 */
	TimingData();
	/**
	 * @brief Gets the actual BPM of the song.
	 * @param fMinBPMOut the minimium specified BPM.
	 * @param fMaxBPMOut the maximum specified BPM.
	 */
	void GetActualBPM( float &fMinBPMOut, float &fMaxBPMOut ) const;
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
	BPMSegment& GetBPMSegmentAtRow( int iNoteRow );
	/**
	 * @brief Retrieve the BPMSegment at the specified beat.
	 * @param fBeat the beat that has a BPMSegment.
	 * @return the BPMSegment in question.
	 */
	BPMSegment& GetBPMSegmentAtBeat( float fBeat ) { return GetBPMSegmentAtRow( (int)BeatToNoteRow(fBeat)); }
	/**
	 * @brief Retrieve the index of the BPMSegments at the specified row.
	 * @param iNoteRow the row that has a BPMSegment.
	 * @return the BPMSegment's index in question.
	 */
	int GetBPMSegmentIndexAtRow( int iNoteRow ) const;
	/**
	 * @brief Retrieve the index of the BPMSegments at the specified beat.
	 * @param fBeat the beat that has a BPMSegment.
	 * @return the BPMSegment's index in question.
	 */
	int GetBPMSegmentIndexAtBeat( float fBeat ) const { return GetBPMSegmentIndexAtRow( BeatToNoteRow(fBeat)); }
	/**
	 * @brief Add the BPMSegment to the TimingData.
	 * @param seg the new BPMSegment.
	 */
	void AddBPMSegment( const BPMSegment &seg );
	
	/**
	 * @brief Retrieve the Stop/Delay at the given row.
	 * @param iNoteRow the row in question.
	 * @param bDelayOut A flag to determine if we are getting a delay or not.
	 * @return the time we stop at this row.
	 */
	float GetStopAtRow( int iNoteRow, bool bDelayOut ) const;
	/**
	 * @brief Retrieve the Stop/Delay at the given row.
	 * @param fBeat the beat in question.
	 * @param bDelayOut A flag to determine if we are getting a delay or not.
	 * @return the time we stop at this beat.
	 */
	float GetStopAtBeat( float fBeat, bool bDelayOut ) const { return GetStopAtRow( BeatToNoteRow(fBeat), bDelayOut ); }
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
	 * @brief Set the row to have the new stop time.
	 * @param iNoteRow the row to have the new stop time.
	 * @param fSeconds the new stop time.
	 */
	void SetStopAtRow( int iNoteRow, float fSeconds ) { SetStopAtRow( iNoteRow, fSeconds, false ); }
	/**
	 * @brief Set the row to have the new pause time.
	 *
	 * This function was added specifically for sm-ssc.
	 * @param iNoteRow the row to have the new pause time.
	 * @param fSeconds the new pause time.
	 * @param bDelay If true, this is a Delay Segment. Otherwise, it is a StopSegment.
	 */
	void SetStopAtRow( int iNoteRow, float fSeconds, bool bDelay );
	/**
	 * @brief Set the row to have the new delay time.
	 *
	 * This function was added specifically for sm-ssc.
	 * @param iNoteRow the row to have the new delay time.
	 * @param fSeconds the new delay time.
	 */
	void SetDelayAtRow( int iNoteRow, float fSeconds ) { SetStopAtRow( iNoteRow, fSeconds, true ); }
	/**
	 * @brief Set the beat to have the new stop time.
	 * @param fBeat to have the new stop time.
	 * @param fSeconds the new stop time.
	 */
	void SetStopAtBeat( float fBeat, float fSeconds ) { SetStopAtRow( BeatToNoteRow(fBeat), fSeconds, false ); }
	/**
	 * @brief Set the beat to have the new pause time.
	 *
	 * This function was added specifically for sm-ssc.
	 * @param fBeat the beat to have the new pause time.
	 * @param fSeconds the new pause time.
	 * @param bDelay If true, this is a Delay Segment. Otherwise, it is a StopSegment.
	 */
	void SetStopAtBeat( float fBeat, float fSeconds, bool bDelay ) { SetStopAtRow( BeatToNoteRow(fBeat), fSeconds, bDelay ); }
	/**
	 * @brief Set the beat to have the new delay time.
	 *
	 * This function was added specifically for sm-ssc.
	 * @param fBeat the beat to have the new delay time.
	 * @param fSeconds the new delay time.
	 */
	void SetDelayAtBeat( float fBeat, float fSeconds ) { SetStopAtRow( BeatToNoteRow(fBeat), fSeconds, true ); }
	/**
	 * @brief Retrieve the StopSegment at the specified row.
	 * @param iNoteRow the row that has a StopSegment.
	 * @return the StopSegment in question.
	 */
	StopSegment& GetStopSegmentAtRow( int iNoteRow ) { return GetStopSegmentAtRow( iNoteRow, false ); }
	/**
	 * @brief Retrieve the StopSegment at the specified beat.
	 * @param fBeat the beat that has a StopSegment.
	 * @return the StopSegment in question.
	 */
	StopSegment& GetStopSegmentAtBeat( float fBeat ) { return GetStopSegmentAtRow( BeatToNoteRow(fBeat), false); }
	/**
	 * @brief Retrieve the StopSegment at the specified row.
	 * @param iNoteRow the row that has a StopSegment.
	 * @param bDelay If true, this is actually a DelaySegment.
	 * @return the StopSegment in question.
	 */
	StopSegment& GetStopSegmentAtRow( int iNoteRow, bool bDelay );
	/**
	 * @brief Retrieve the StopSegment at the specified beat.
	 * @param fBeat the beat that has a StopSegment.
	 * @param bDelay If true, this is actually a DelaySegment.
	 * @return the StopSegment in question.
	 */
	StopSegment& GetStopSegmentAtBeat( float fBeat, bool bDelay ) { return GetStopSegmentAtRow( BeatToNoteRow(fBeat), bDelay ); }
	/**
	 * @brief Retrieve the DelaySegment at the specified row.
	 * @param iNoteRow the row that has a DelaySegment.
	 * @return the DelaySegment in question.
	 */
	StopSegment& GetDelaySegmentAtRow( int iNoteRow ) { return GetStopSegmentAtRow( iNoteRow, true ); }
	/**
	 * @brief Retrieve the DelaySegment at the specified beat.
	 * @param fBeat the beat that has a DelaySegment.
	 * @return the DelaySegment in question.
	 */
	StopSegment& GetDelaySegmentAtBeat( float fBeat ) { return GetStopSegmentAtRow( BeatToNoteRow(fBeat), true); }
	/**
	 * @brief Retrieve the index of the StopSegments at the specified row.
	 * @param iNoteRow the row that has a StopSegment.
	 * @return the StopSegment's index in question.
	 */
	int GetStopSegmentIndexAtRow( int iNoteRow ) const { return GetStopSegmentIndexAtRow( iNoteRow, false ); }
	/**
	 * @brief Retrieve the index of the StopSegments at the specified beat.
	 * @param fBeat the beat that has a StopSegment.
	 * @return the StopSegment's index in question.
	 */
	int GetStopSegmentIndexAtBeat( float fBeat ) const { return GetStopSegmentIndexAtRow( BeatToNoteRow(fBeat), false ); }
	/**
	 * @brief Retrieve the index of the StopSegments at the specified row.
	 * @param iNoteRow the row that has a StopSegment.
	 * @param bDelay If true, it's a Delay Segment. Otherwise, it's a StopSegment.
	 * @return the StopSegment's index in question.
	 */
	int GetStopSegmentIndexAtRow( int iNoteRow, bool bDelay ) const;
	/**
	 * @brief Retrieve the index of the StopSegments at the specified beat.
	 * @param fBeat the beat that has a StopSegment.
	 * @param bDelay If true, it's a Delay Segment. Otherwise, it's a StopSegment.
	 * @return the StopSegment's index in question.
	 */
	int GetStopSegmentIndexAtBeat( float fBeat, bool bDelay ) const { return GetStopSegmentIndexAtRow( BeatToNoteRow(fBeat), bDelay ); }
	/**
	 * @brief Retrieve the index of the Delay Segments at the specified row.
	 * @param iNoteRow the row that has a Delay Segment.
	 * @return the StopSegment's index in question.
	 */
	int GetDelaySegmentIndexAtRow( int iNoteRow ) const { return GetStopSegmentIndexAtRow( iNoteRow, true ); }
	/**
	 * @brief Retrieve the index of the Delay Segments at the specified beat.
	 * @param fBeat the beat that has a Delay Segment.
	 * @return the StopSegment's index in question.
	 */
	int GetDelaySegmentIndexAtBeat( float fBeat ) const { return GetStopSegmentIndexAtRow( BeatToNoteRow(fBeat), true ); }
	/**
	 * @brief Add the StopSegment to the TimingData.
	 * @param seg the new StopSegment.
	 */
	void AddStopSegment( const StopSegment &seg );
	
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
	TimeSignatureSegment& GetTimeSignatureSegmentAtRow( int iNoteRow );
	/**
	 * @brief Retrieve the TimeSignatureSegment at the specified beat.
	 * @param fBeat the beat that has a TimeSignatureSegment.
	 * @return the TimeSignatureSegment in question.
	 */
	TimeSignatureSegment& GetTimeSignatureSegmentAtBeat( float fBeat ) { return GetTimeSignatureSegmentAtRow( BeatToNoteRow(fBeat) ); }
	/**
	 * @brief Retrieve the index of the TimeSignatureSegments at the specified row.
	 * @param iNoteRow the row that has a TimeSignatureSegment.
	 * @return the TimeSignatureSegment's index in question.
	 */
	int GetTimeSignatureSegmentIndexAtRow( int iNoteRow ) const;
	/**
	 * @brief Retrieve the index of the TimeSignatureSegments at the specified beat.
	 * @param fBeat the beat that has a TimeSignatureSegment.
	 * @return the TimeSignatureSegment's index in question.
	 */
	int GetTimeSignatureSegmentIndexAtBeat( float fBeat ) const { return GetTimeSignatureSegmentIndexAtRow( BeatToNoteRow(fBeat) ); }
	/**
	 * @brief Add the TimeSignatureSegment to the TimingData.
	 * @param seg the new TimeSignatureSegment.
	 */
	void AddTimeSignatureSegment( const TimeSignatureSegment &seg );
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
	WarpSegment& GetWarpSegmentAtRow( int iRow );
	/**
	 * @brief Retrieve the WarpSegment at the specified beat.
	 * @param fBeat the beat to focus on.
	 * @return the WarpSegment in question.
	 */
	WarpSegment& GetWarpSegmentAtBeat( float fBeat ) { return GetWarpSegmentAtRow( BeatToNoteRow( fBeat ) ); }
	/**
	 * @brief Retrieve the index of the WarpSegment at the specified row.
	 * @param iRow the row to focus on.
	 * @return the index in question.
	 */
	int GetWarpSegmentIndexAtRow( int iRow ) const;
	/**
	 * @brief Retrieve the index of the WarpSegment at the specified beat.
	 * @param fBeat the beat to focus on.
	 * @return the index in question.
	 */
	int GetWarpSegmentIndexAtBeat( float fBeat ) const { return GetWarpSegmentIndexAtRow( BeatToNoteRow( fBeat ) ); }
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
	 * @brief Add the WarpSegment to the TimingData.
	 * @param seg the new WarpSegment.
	 */
	void AddWarpSegment( const WarpSegment &seg );
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
	TickcountSegment& GetTickcountSegmentAtRow( int iNoteRow );
	/**
	 * @brief Retrieve the TickcountSegment at the specified beat.
	 * @param fBeat the beat that has a TickcountSegment.
	 * @return the TickcountSegment in question.
	 */
	TickcountSegment& GetTickcountSegmentAtBeat( float fBeat ) { return GetTickcountSegmentAtRow( BeatToNoteRow(fBeat) ); }
	/**
	 * @brief Retrieve the index of the TickcountSegments at the specified row.
	 * @param iNoteRow the row that has a TickcountSegment.
	 * @return the TickcountSegment's index in question.
	 */
	int GetTickcountSegmentIndexAtRow( int iNoteRow ) const;
	/**
	 * @brief Retrieve the index of the TickcountSegments at the specified beat.
	 * @param fBeat the beat that has a TickcountSegment.
	 * @return the TickcountSegment's index in question.
	 */
	int GetTickcountSegmentIndexAtBeat( float fBeat ) const { return GetTickcountSegmentIndexAtRow( BeatToNoteRow(fBeat) ); }
	/**
	 * @brief Add the TickcountSegment to the TimingData.
	 * @param seg the new TickcountSegment.
	 */
	void AddTickcountSegment( const TickcountSegment &seg );
	
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
	 * @brief Set the row to have the new Combo.
	 * @param iNoteRow the row to have the new Combo.
	 * @param iTicks the Combo.
	 */
	void SetComboAtRow( int iNoteRow, int iCombo );
	/**
	 * @brief Set the beat to have the new Combo.
	 * @param fBeat the beat to have the new Combo.
	 * @param iTicks the Combo.
	 */
	void SetComboAtBeat( float fBeat, int iCombo ) { SetComboAtRow( BeatToNoteRow( fBeat ), iCombo ); }
	/**
	 * @brief Retrieve the ComboSegment at the specified row.
	 * @param iNoteRow the row that has a ComboSegment.
	 * @return the ComboSegment in question.
	 */
	ComboSegment& GetComboSegmentAtRow( int iNoteRow );
	/**
	 * @brief Retrieve the ComboSegment at the specified beat.
	 * @param fBeat the beat that has a ComboSegment.
	 * @return the ComboSegment in question.
	 */
	ComboSegment& GetComboSegmentAtBeat( float fBeat ) { return GetComboSegmentAtRow( BeatToNoteRow(fBeat) ); }
	/**
	 * @brief Retrieve the index of the ComboSegments at the specified row.
	 * @param iNoteRow the row that has a ComboSegment.
	 * @return the ComboSegment's index in question.
	 */
	int GetComboSegmentIndexAtRow( int iNoteRow ) const;
	/**
	 * @brief Retrieve the index of the ComboSegments at the specified beat.
	 * @param fBeat the beat that has a ComboSegment.
	 * @return the ComboSegment's index in question.
	 */
	int GetComboSegmentIndexAtBeat( float fBeat ) const { return GetComboSegmentIndexAtRow( BeatToNoteRow(fBeat) ); }
	/**
	 * @brief Add the ComboSegment to the TimingData.
	 * @param seg the new ComboSegment.
	 */
	void AddComboSegment( const ComboSegment &seg );
	
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
	/**
	 * @brief View the TimingData to see if a song changes its BPM at any point.
	 * @return true if there is at least one change, false otherwise.
	 */
	bool HasBpmChanges() const;
	/**
	 * @brief View the TimingData to see if there is at least one stop at any point.
	 *
	 * For the purposes of this function, Stops and Delays are considered the same.
	 * @return true if there is at least one stop, false otherwise.
	 */
	bool HasStops() const;
	/**
	 * @brief View the TimingData to see if there is at least one warp at any point.
	 * @return true if there is at least one warp, false otherwise.
	 */
	bool HasWarps() const;
	/**
	 * @brief Compare two sets of timing data to see if they are equal.
	 * @param other the other TimingData.
	 * @return the equality or lack thereof of the two TimingData.
	 */
	bool operator==( const TimingData &other )
	{
		COMPARE( m_BPMSegments.size() );
		for( unsigned i=0; i<m_BPMSegments.size(); i++ )
			COMPARE( m_BPMSegments[i] );
		COMPARE( m_StopSegments.size() );
		for( unsigned i=0; i<m_StopSegments.size(); i++ )
			COMPARE( m_StopSegments[i] );
		COMPARE( m_WarpSegments.size() );
		for( unsigned i=0; i<m_WarpSegments.size(); i++ )
			COMPARE( m_WarpSegments[i] );
		COMPARE( m_vTimeSignatureSegments.size() );
		for( unsigned i=0; i<m_vTimeSignatureSegments.size(); i++)
			COMPARE( m_vTimeSignatureSegments[i] );
		COMPARE( m_TickcountSegments.size() );
		for( unsigned i=0; i<m_TickcountSegments.size(); i++ )
			COMPARE( m_TickcountSegments[i] );
		COMPARE( m_ComboSegments.size() );
		for( unsigned i=0; i<m_ComboSegments.size(); i++ )
			COMPARE( m_ComboSegments[i] );
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

	// Lua
	void PushSelf( lua_State *L );
	/**
	 * @brief The file of the song/steps that use this TimingData.
	 *
	 * This is for informational purposes only.
	 */
	RString					m_sFile;
	// All of the following vectors must be sorted before gameplay.
	/**
	 * @brief The collection of BPMSegments.
	 */
	vector<BPMSegment>		m_BPMSegments;
	/**
	 * @brief The collection of StopSegments & DelaySegments.
	 */
	vector<StopSegment>		m_StopSegments;
	/**
	 * @brief The collection of TimeSignatureSegments.
	 */
	vector<TimeSignatureSegment>	m_vTimeSignatureSegments;
	/**
	 * @brief The collection of WarpSegments.
	 */
	vector<WarpSegment>		m_WarpSegments;
	/**
	 * @brief The collection of TickcountSegments.
	 */
	vector<TickcountSegment>	m_TickcountSegments;
	/**
	 * @brief The collection of ComboSegments.
	 */
	vector<ComboSegment>		m_ComboSegments;
	/**
	 * @brief The initial offset of a song.
	 */
	float	m_fBeat0OffsetInSeconds;
	/**
	 * @brief A flag to see if negative BPMs are used in this song.
	 *
	 * This is only used for Lua bindings in the Song, with the intentions that
	 * this is moved into TimingData at a future point in time.
	 */
	bool	m_bHasNegativeBpms;
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

#include "global.h"
#include "TimingSegments.h"
#include "EnumHelper.h"

static const char *TimingSegmentTypeNames[] = {
	"BPM",
	"Stop/Delay", // TODO: separate when stops and delays are separate.
	"Time Sig",
	"Warp",
	"Label",
	"Tickcount",
	"Combo",
	"Speed",
	"Scroll",
	"Fake"
};
XToString( TimingSegmentType );

#define LTCOMPARE(x)      if(this->x < other.x) return true; if(this->x > other.x) return false;

TimingSegment::~TimingSegment() {}

void TimingSegment::SetRow(const int s)
{
	this->startingRow = s;
}

void TimingSegment::SetBeat(const float s)
{
	SetRow(BeatToNoteRow(s));
}

int TimingSegment::GetRow() const
{
	return this->startingRow;
}

float TimingSegment::GetBeat() const
{
	return NoteRowToBeat(GetRow());
}

void TimingSegment::Scale( int start, int length, int newLength )
{
	SetRow( ScalePosition( start, length, newLength, this->GetRow() ) );
}


/* ======================================================
   Here comes the actual timing segments implementation!! */

float FakeSegment::GetLength() const
{
	return this->lengthBeats;
}

void FakeSegment::SetLength(const float b)
{
	this->lengthBeats = b;
}

void FakeSegment::Scale( int start, int length, int newLength )
{
	float startBeat    = GetBeat();
	float endBeat      = startBeat + GetLength();
	float newStartBeat = ScalePosition( NoteRowToBeat(start), NoteRowToBeat(length), NoteRowToBeat(newLength), startBeat );
	float newEndBeat   = ScalePosition( NoteRowToBeat(start), NoteRowToBeat(length), NoteRowToBeat(newLength), endBeat );
	SetLength( newEndBeat - newStartBeat );
	TimingSegment::Scale( start, length, newLength );
}

bool FakeSegment::operator<( const FakeSegment &other ) const
{ 
	LTCOMPARE(GetRow());
	LTCOMPARE(GetLength());
	return false;
}




float WarpSegment::GetLength() const
{
	return this->lengthBeats;
}

void WarpSegment::SetLength(const float b)
{
	this->lengthBeats = b;
}

void WarpSegment::Scale( int start, int length, int newLength )
{
	// XXX: this function is duplicated, there should be a better way
	float startBeat    = GetBeat();
	float endBeat      = startBeat + GetLength();
	float newStartBeat = ScalePosition( NoteRowToBeat(start), NoteRowToBeat(length), NoteRowToBeat(newLength), startBeat );
	float newEndBeat   = ScalePosition( NoteRowToBeat(start), NoteRowToBeat(length), NoteRowToBeat(newLength), endBeat );
	SetLength( newEndBeat - newStartBeat );
	TimingSegment::Scale( start, length, newLength );
}

bool WarpSegment::operator<( const WarpSegment &other ) const
{
	LTCOMPARE(GetRow());
	LTCOMPARE(GetLength());
	return false;
}




int TickcountSegment::GetTicks() const
{
	return this->ticks;
}

void TickcountSegment::SetTicks(const int i)
{
	this->ticks = i;
}

bool TickcountSegment::operator<( const TickcountSegment &other ) const
{
	LTCOMPARE(GetRow());
	LTCOMPARE(GetTicks());
	return false;
}




int ComboSegment::GetCombo() const
{
	return this->combo;
}

void ComboSegment::SetCombo(const int i)
{
	this->combo = i;
}

int ComboSegment::GetMissCombo() const
{
	return this->missCombo;
}

void ComboSegment::SetMissCombo(const int i)
{
	this->missCombo = i;
}

bool ComboSegment::operator<( const ComboSegment &other ) const
{
	LTCOMPARE(GetRow());
	LTCOMPARE(GetCombo());
	LTCOMPARE(GetMissCombo());
	return false;
}




RString LabelSegment::GetLabel() const
{
	return this->label;
}

void LabelSegment::SetLabel(const RString l)
{
	this->label = l;
}

bool LabelSegment::operator<( const LabelSegment &other ) const
{ 
	LTCOMPARE(GetRow());
	LTCOMPARE(GetLabel());
	return false;
}



float BPMSegment::GetBPM() const
{
	return this->bps * 60.0f;
}

void BPMSegment::SetBPM(const float bpm)
{
	this->bps = bpm / 60.0f;
}

float BPMSegment::GetBPS() const
{
	return this->bps;
}

void BPMSegment::SetBPS(const float newBPS)
{
	this->bps = newBPS;
}

bool BPMSegment::operator<( const BPMSegment &other ) const
{ 
	LTCOMPARE(GetRow());
	LTCOMPARE(GetBPS());
	return false;
}

int TimeSignatureSegment::GetNum() const
{
	return this->numerator;
}

void TimeSignatureSegment::SetNum(const int i)
{
	this->numerator = i;
}

int TimeSignatureSegment::GetDen() const
{
	return this->denominator;
}

void TimeSignatureSegment::SetDen(const int i)
{
	this->denominator = i;
}

int TimeSignatureSegment::GetNoteRowsPerMeasure() const
{
	return BeatToNoteRow(1) * 4 * numerator / denominator;
}

bool TimeSignatureSegment::operator<( const TimeSignatureSegment &other ) const
{ 
	LTCOMPARE(GetRow());
	LTCOMPARE(GetNum());
	LTCOMPARE(GetDen());
	return false;
}

float SpeedSegment::GetRatio() const
{
	return this->ratio;
}

void SpeedSegment::SetRatio(const float i)
{
	this->ratio = i;
}

float SpeedSegment::GetLength() const
{
	return this->length;
}

void SpeedSegment::SetLength(const float i)
{
	this->length = i;
}


unsigned short SpeedSegment::GetUnit() const
{
	return this->unit;
}

void SpeedSegment::SetUnit(const unsigned short i)
{
	this->unit = i;
}

void SpeedSegment::SetUnit(const int i)
{
	this->unit = static_cast<unsigned short>(i);
}

void SpeedSegment::Scale( int start, int oldLength, int newLength )
{
	if( GetUnit() == 0 )
	{
		// XXX: this function is duplicated, there should be a better way
		float startBeat    = GetBeat();
		float endBeat      = startBeat + GetLength();
		float newStartBeat = ScalePosition(NoteRowToBeat(start),
						   NoteRowToBeat(oldLength),
						   NoteRowToBeat(newLength),
						   startBeat);
		float newEndBeat   = ScalePosition(NoteRowToBeat(start),
						   NoteRowToBeat(oldLength),
						   NoteRowToBeat(newLength),
						   endBeat);
		SetLength( newEndBeat - newStartBeat );
	}
	TimingSegment::Scale( start, oldLength, newLength );
}

bool SpeedSegment::operator<( const SpeedSegment &other ) const
{
	LTCOMPARE(GetRow());
	LTCOMPARE(GetRatio());
	LTCOMPARE(GetLength());
	LTCOMPARE(GetUnit());
	return false;
}

float ScrollSegment::GetRatio() const
{
	return this->ratio;
}

void ScrollSegment::SetRatio(const float i)
{
	this->ratio = i;
}

bool ScrollSegment::operator<( const ScrollSegment &other ) const
{
	LTCOMPARE(GetRow());
	LTCOMPARE(GetRatio());
	return false;
}

float StopSegment::GetPause() const
{
	return this->pauseSeconds;
}

void StopSegment::SetPause(const float i)
{
	this->pauseSeconds = i;
}

bool StopSegment::GetDelay() const
{
	return this->isDelay;
}

void StopSegment::SetDelay(const bool i)
{
	this->isDelay = i;
}

bool StopSegment::operator<( const StopSegment &other ) const
{
	LTCOMPARE(GetRow());
	if (this->GetDelay() && !other.GetDelay()) return true;
	if (!this->GetDelay() && other.GetDelay()) return false;
	LTCOMPARE(GetPause());
	return false;
}

/**
 * @file
 * @author Jason Felds (c) 2011 
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

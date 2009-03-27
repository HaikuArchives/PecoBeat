/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#include "Song.h"

#include <Alert.h>
#include <Looper.h>
#include <Message.h>
#include <MidiDefs.h>
#include <Window.h>

#include "App.h"
#include "MIDIProducer.h"
#include "PatternDefs.h"
#include "PatternRow.h"
#include "Ticker.h"

Song *gSong = 0;

Song::Song()
:	BLooper("Beat Song", 50 ),
	fPattern( 0 ),
	fTicker( 0 )
	
{
	Init();
	Run();

	fTicker =  new Ticker( this );
}

Song::~Song() {

	Stop();
	if (fTicker) {
		fTicker->Lock();
		fTicker->Quit();
	}
	delete fPattern;

}

void Song::Init() {
	fActiveNr = 0,
	fLastFilledNr = -1;
	fVelocityFactor = 1.0;
	fCursorPosition = 0;
	fIsPlaying = false;
	fModified = false;

	fPattern = 	new Pattern();

	for (int i=0; i < (16 * 4); ++i) fPlayList[i] = -1;
}

void Song::SaveToMessage(BMessage &msg) {
	msg.what = 'peco';
	msg.AddString( "signature", "pecobeat" );
	msg.AddFloat( "version", 1.0);
	
	msg.AddInt32("speed", Speed() );
	msg.AddInt32("velocity", Velocity() );

	for(int i=0; i<=LastFilledNr(); ++i) {
		msg.AddInt32("playlist", PlayListEntry(i));
	}

	for (int i=0; i<12; ++i) {
		BMessage part_msg('part');
		for (int j = 0; j<fPattern->CountItems(); ++j) {
			BMessage row_msg('row');
			PatternRow *row = (PatternRow *)fPattern->ItemAt(j);
			if (i==0) {
				row_msg.AddInt16("key", row->Key());
				row_msg.AddInt32("velocity", row->Velocity());
			}
			for (int k=0; k<256; ++k) {
				row_msg.AddInt8("note", row->Note( (i<<8) + k) );
			}
			part_msg.AddMessage("row", &row_msg);
		}
		msg.AddMessage("part", &part_msg);
	}
}

status_t Song::LoadFromMessage(BMessage &msg) {
	
	BString	string;
	float	f;
	int32 	i32;
	int16 	i16;
	int8	i8;

	if (msg.what!='peco') return B_ERROR;
	
	if (msg.FindString("signature", &string)!=B_OK) return B_ERROR;
	if (string!="pecobeat") return B_ERROR;
	
	if (msg.FindFloat("version", &f)!=B_OK) return B_ERROR;
	if (f!=1.0) return B_ERROR;
	
	delete fPattern;
	Init();
	
	if (msg.FindInt32("speed", &i32 )==B_OK) SetSpeed(i32);
	if (msg.FindInt32("velocity", &i32 )==B_OK) SetVelocity(i32);

	int32 j=-1;
	for(int i=0; i<64 && (msg.FindInt32("playlist", i, &i32)==B_OK); ++i) {
		if (i32<12) {
			++j;
			SetPlayListEntry(i, i32);
		}
	}
	SetLastFilledNr(j);
	
	BMessage part_msg;
	for(int i=0; i<12 && (msg.FindMessage("part", i, &part_msg)==B_OK); ++i) {
		BMessage	row_msg;
		for (int j = 0; part_msg.FindMessage("row", j, &row_msg)==B_OK; ++j) {

			PatternRow *row;
			if (!(row = (PatternRow *)fPattern->ItemAt(j))) {
				row = new PatternRow();
				fPattern->AddItem(row);
			}
			if (row_msg.FindInt16("key", &i16)==B_OK) row->SetKey(i16);
			if (row_msg.FindInt32("velocity", &i32)==B_OK) row->SetVelocity(i32);
			
			for (int k=0; k<256 && (row_msg.FindInt8("note", k, &i8 )==B_OK); ++k) {
				row->SetNote( (i<<8) + k, i8 );
			}
		}
	}

	return B_OK;
}

void Song::PlaySong() {
	fCursorPosition = fPlayList[fActiveNr]<<8;
	fPlayMode = PLAY_SONG;
	StartPlaying();
}

void Song::PlayPattern() {
	fCursorPosition = (ActivePart()<<8) + fCursorPosition & 255;
	fPlayMode = PLAY_PATTERN;
	StartPlaying();
}

void Song::Stop() {
	fCursorPosition = -1;

	fTicker->Stop();

	fIsPlaying = false;
	for (int i = 0; i<fPattern->CountItems(); ++i ) {
		PatternRow *row = (PatternRow *)(fPattern->ItemAt(i));
		StopSound( row->Key(), row->Velocity() * fVelocityFactor );
	}
}

void Song::StartPlaying() {
	fTicker->Start();
	fIsPlaying = true;

}

void Song::SetActiveNr( int32 nr ) {
	fActiveNr = nr;
	if (IsPlaying() && PlayMode()==PLAY_SONG) {
		SetCursorPosition( (fCursorPosition & 255) + (PlayListEntry(nr)<<8) );
	}
}

void Song::SetPlayListEntry( int32 entry, int32 part ) {
	SetModified();
	fPlayList[entry] = part;
}

void Song::SetSpeed( int32 speed ) {
	fSpeed = speed;
	fTicker->SetDelay( 3750000 / speed );
};

void Song::MessageReceived(BMessage *msg) {

	switch ( msg->what ) {

		case 'tick': {

			if (fIsPlaying && fCursorPosition!=-1) {
				UpdateCursor();
				PlayAtCursor();
			
				++fCursorPosition;
				UpdateCursor();
			}

		} break;

		case 'plys': {
			PlaySong();
		} break;
		
		case 'plyp': {
			PlayPattern();
		} break;
		
		case 'stop': {
			Stop();
		} break;

	}

}

void Song::UpdateCursor() {

	if (fPlayMode==PLAY_PATTERN) {
		fCursorPosition = (ActivePart() << 8) + (fCursorPosition & 255);
	}
	else {

		if ((fCursorPosition>>8) != fPlayList[fActiveNr]) {

			if (fActiveNr==fLastFilledNr) {
				fActiveNr = 0;
				be_app->PostMessage('stop');
			}
			else {
				fActiveNr++;
				fCursorPosition = fPlayList[fActiveNr]<<8;
			}
		}
	}
}

void Song::PlayAtCursor() {
	if (fCursorPosition==-1) return;
	
	for (int i = 0; i<fPattern->CountItems(); ++i ) {
		PatternRow *row = (PatternRow *)(fPattern->ItemAt(i));
		switch( row->Note(fCursorPosition) ) {
			case NOTE_ON: PlaySound( row->Key(), row->Velocity() * fVelocityFactor ); break;
			case NOTE_OFF: StopSound( row->Key(), row->Velocity() * fVelocityFactor ); break;
		}
	}
}

void Song::PlaySound( int32 key, int32 velocity ) {

	StopSound( key, velocity );
	gMidiProducer->SprayNoteOn( 10, key, (int)(velocity * fVelocityFactor) );

}

void Song::StopSound( int32 key, int32 velocity ) {

	gMidiProducer->SprayNoteOff( 10, key, (int)(velocity * fVelocityFactor) );

}

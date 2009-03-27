/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#include "PatternRow.h"

#include <Message.h>

#include "PatternDefs.h"
#include "Song.h"

PatternRow::PatternRow( BMessage *msg )
{
	if (msg) LoadFromMessage( *msg );
	else Init();
}

void PatternRow::Init() {
	fKey = 0;
	fVelocity = 0;
	for (int i=0; i<256*12; ++i) fNotes[i] = NOTE_NONE;
}

void PatternRow::SaveToMessage(BMessage &msg) {
	
	msg.AddInt8("key", fKey );
	msg.AddInt8("velocity", fVelocity );
	
	for (int i=0; i<256*12; ++i) msg.AddInt8( "note", fNotes[i] );

}

void PatternRow::SaveActivePartToMessage(BMessage &msg) {
	
	gSong->Lock();
	int	offset = gSong->ActivePart() << 8;
	gSong->Unlock();
	
	msg.AddInt8("key", fKey );
	msg.AddInt8("velocity", fVelocity );
	
	for (int i=0; i<256; ++i) {
		msg.AddInt8( "note", fNotes[i + offset] );
	}

}

void PatternRow::LoadFromMessage( BMessage &msg ) {

	Init();
	msg.FindInt8("key", &fKey );
	msg.FindInt8("velocity", &fVelocity );
	
	for (int i=0; i<256*12; ++i) msg.FindInt8( "note", i, &fNotes[i] );

}

PatternRow::~PatternRow() {};

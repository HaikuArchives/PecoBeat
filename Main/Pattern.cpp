/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#include "Pattern.h"

#include <Message.h>

#include <iostream>

#include "PatternDefs.h"
#include "PatternRow.h"

Pattern::Pattern()
:	BList(),
	fActivePart( 0 )
{
}

void Pattern::SaveActivePartToMessage(BMessage &msg) {

	for (int i = 0; i<CountItems(); ++i ) {

		BMessage	notes;
		PatternRow	*row = (PatternRow *)ItemAt(i);
		
		for (int j = 0; j < 256; ++j )
			notes.AddInt8("note", row->Note((j & 255)|(ActivePart()<<8)));
		
		msg.AddMessage("row", &notes);
		
	}

}

int32 Pattern::AddPatternRow( BMessage &msg, int32 index ) {

	PatternRow	*patternRow = new PatternRow( &msg );

	if (index==-1)	{
		AddItem( patternRow );
		index = CountItems() - 1;
	}
	else {
		AddItem( patternRow, index );
	}
	
	gSong->SetModified();
	return index;
	
}

void Pattern::DeletePatternRow( int32 index ) {

	delete (PatternRow *)RemoveItem( index );

	gSong->SetModified();
}

void Pattern::MovePatternRow( int32 from, int32 to ) {

	MoveItem(from, to);

	gSong->SetModified();
}

bool Pattern::PartContainsData( int part ) {

	if (part<0 || part>11) return false;
	
	for (int i = 0; i<CountItems(); ++i ) {
		PatternRow *row = (PatternRow *)ItemAt(i);
		for (int j = 0; j < 256; ++j ) {
			if ( row->Note((j & 255)|(part<<8)) != NOTE_NONE ) return true;
		}
	}

	return false;

}

void Pattern::ClearActivePart() {

	int8 part = fActivePart;
	
	for (int i = 0; i<CountItems(); ++i ) {
		PatternRow *row = (PatternRow *)ItemAt(i);
		for (int j = 0; j < 256; ++j ) row->SetNote((j & 255)|(part<<8), NOTE_NONE );
	}

}

void Pattern::CopyPart( int8 source, int8 destination) {

	int32	source_offset = source << 8;
	int32	dest_offset = destination << 8;
	
	for (int i = 0; i<CountItems(); ++i ) {
		PatternRow *row = (PatternRow *)ItemAt(i);
		for (int j = 0; j < 256; ++j ) row->SetNote(j + dest_offset, row->Note(j + source_offset) );
	}

}

Pattern::~Pattern() {
}


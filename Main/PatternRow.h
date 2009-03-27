/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#ifndef _PATTERN_ROW_H
#define _PATTERN_ROW_H

/*															*/
/* PatternRow ist eine Zeile aus einem einzelnen Pattern	*/
/*															*/

#include <Archivable.h>
#include <SupportDefs.h>

#include <iostream>

#include "Song.h"

class BMessage;

class PatternRow {

public:
				PatternRow( BMessage *msg = NULL );
virtual			~PatternRow();

void			Init();
void 			LoadFromMessage(BMessage &msg);
void			SaveToMessage(BMessage &msg);
void			SaveActivePartToMessage(BMessage &msg);

inline void		SetKey( int16 key ) { gSong->SetModified(); fKey = key; }
inline int16	Key() { return fKey; }

inline void		SetNote( int32 col, int32 note ) { gSong->SetModified(); fNotes[col] = note; }
inline int32	Note( int32 col ) { return fNotes[col]; }

inline void		SetVelocity( int32 velocity ) { gSong->SetModified(); fVelocity = velocity; }
inline int32	Velocity() { return fVelocity; }

private:

inline int32	ActivePart() { return gSong->fPattern->ActivePart(); }

int8			fKey;
int8			fVelocity;
int8			fNotes[256 * 12];

};

#endif

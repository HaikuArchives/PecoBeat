/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

/************************************************************
* Ein Pattern besteht aus einer Liste von PatternRows und	*
* ist eine reine Datenklasse, die ausser dem Organisieren	*
* der Daten keine weiteren Funktionen bietet.				*
*************************************************************/

#ifndef _PATTERN_H
#define _PATTERN_H

#include <List.h>

class BMessage;

class Pattern : public BList
{

public:
				Pattern();
				~Pattern();

void			SaveActivePartToMessage(BMessage &msg);

inline uint8	ActivePart() { return fActivePart; }
inline void		SetActivePart( uint8 active_part ) { fActivePart = active_part; }

int32			AddPatternRow( BMessage &msg, int32 index = -1 );
void			DeletePatternRow( int32 index );
void			MovePatternRow( int32 from, int32 to );

bool			PartContainsData( int part );
void			ClearActivePart();
void			CopyPart( int8 source, int8 destination );

private:

uint8			fActivePart;

};

#endif

/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

/************************************************************************
* Diese Klasse enthält alle Patterns sowie die Playlist und kann ein	*
* Pattern oder den gesamten Song abspielen.								*
*************************************************************************/

#ifndef _SONG_H
#define _SONG_H

#include <Looper.h>

#include <iostream>

#include "Pattern.h"
#include "PlayModes.h"

class Ticker;

class Song : public BLooper {

friend class App;

public:

				Song();
				~Song();

void			Init();

void			SaveToMessage(BMessage &msg);
status_t		LoadFromMessage(BMessage &msg);

void			UpdateCursor();

inline void		PlaySound( int32 key, int32 velocity );
inline void		StopSound( int32 key, int32 velocity );

inline int32	ActivePart() { return fPattern->ActivePart(); }
inline void		SetActivePart( int32 active_part ) { fPattern->SetActivePart(active_part); }

inline int32	ActiveNr() { return fActiveNr; }
void			SetActiveNr( int32 nr );

inline int32	CursorPosition() { return fCursorPosition; }
inline void		SetCursorPosition( int32 cursor_position ) { fCursorPosition = cursor_position; }

inline int32	PlayListEntry( int32 entry ) { return fPlayList[entry]; }
void			SetPlayListEntry( int32 entry, int32 part );

inline int32	ActivePlayListEntry() { return fPlayList[fActiveNr]; }

inline int32	LastFilledNr() { return fLastFilledNr; }
inline void		SetLastFilledNr( int32 nr ) { fLastFilledNr = nr; }

void			SetSpeed( int32 speed );
inline int32	Speed() { return fSpeed; }

inline void		SetVelocity( int32 velocity ) { fVelocityFactor = (float)velocity / 127; }
inline int32	Velocity() { return (int)(fVelocityFactor * 127); }

inline bool		IsPlaying() { return fIsPlaying; }
inline uint32	PlayMode() { return fPlayMode; }

void			MessageReceived(BMessage *msg);

Pattern			*fPattern;

inline void		SetModified( bool modified = true ) { fModified = modified; }
inline bool		Modified() { return fModified; }

protected:

void			PlaySong();
void			PlayPattern();

void			Stop();

void			StartPlaying();

void			PlayAtCursor();

private:

Ticker			*fTicker;
int32			fPlayList[16 * 4];

int32			fActiveNr;
int32			fLastFilledNr;

int32			fSpeed;				// in bpm
float			fVelocityFactor;	// 0..1

uint32			fPlayMode;
int32			fCursorPosition;

bool			fIsPlaying;
bool			fModified;
};

extern	Song *gSong;

#endif

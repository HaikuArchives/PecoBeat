/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */


#ifndef _PLAYLIST_FIELD_H
#define _PLAYLIST_FIELD_H

#include <Bitmap.h>
#include <Control.h>
#include <List.h>

class	BMessageRunner;

class	PlayListField : public BControl {

public:
				PlayListField(BRect rect, BMessage *message=NULL);
virtual			~PlayListField();

void			MouseMoved(	BPoint where, uint32 code, const BMessage *a_message);
void			MouseDown(BPoint where);
void			MouseUp(BPoint where);

void			AttachedToWindow();
void			MessageReceived( BMessage *message );

void			Draw(BRect updateRect);

void			FullUpdate();

void			PlaySong();

void			Stop();

private:

void			StartMessageRunner();

void			Update( bool drawAll = false );
int32			FindButtonAt( BPoint where );
bool			UpdateMousePosition( BPoint where, bool limit = true );

BView			*fBgView;
BBitmap			*fBgBitmap;
BBitmap			*fNrBitmap[13][2];
int32			fLastFilledNr;
int32			fMouseBeforeNr;
bool			fMousePressed;
int32			fDragPosition;
bool			fDragging;
BMessageRunner	*fMessageRunner;
bool			fMouseMoved;
};

#endif

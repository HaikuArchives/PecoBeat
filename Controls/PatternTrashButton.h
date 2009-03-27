/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#ifndef _PATTERN_TRASH_BUTTON_H
#define _PATTERN_TRASH_BUTTON_H

#include <Bitmap.h>
#include <PictureButton.h>
#include <Rect.h>

class	PatternTrashButton : public BControl {

public:
			PatternTrashButton(BRect rect, BMessage *message=NULL);
			~PatternTrashButton();

status_t	Invoke(BMessage *msg = NULL);
void		MessageReceived( BMessage *message );

void		MouseMoved(	BPoint where, uint32 code, const BMessage *a_message);
void		MouseDown(BPoint where);
void		MouseUp(BPoint where);

void		Draw(BRect updateRect);

private:

void		Update();

BBitmap		*fBitmap[2];
bool		fMousePressed;
bool		fHasMessage;
};

#endif

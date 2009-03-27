/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#ifndef _PATTERN_NR_BUTTON_H
#define _PATTERN_NR_BUTTON_H

#include <Bitmap.h>
#include <Control.h>

class PatternNrButton : public BControl {

public:
				PatternNrButton(BRect rect, BMessage *message=NULL);
				~PatternNrButton();

void			MouseMoved(	BPoint where, uint32 code, const BMessage *a_message);
void			MouseDown(BPoint where);
void			MouseUp(BPoint where);

void			AttachedToWindow();
void			Draw(BRect updateRect);

void			MessageReceived( BMessage *message );

void			SetValue(int32 value);

private:

char			FindButtonNr( BPoint where );

BBitmap			*fBitmap[12][2];	// Die Bildchen der Nummern jeweils: empty, filled, active
bool			fMousePressed;
int8			fMarked;			// Markiert, weil andere Nr darüber gehalten wird
};

#endif

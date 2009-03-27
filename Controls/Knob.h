/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#ifndef _KNOB_H
#define _KNOB_H

#include <Bitmap.h>
#include <Control.h>

class Knob : public BControl {

public:
			Knob( BRect frame,
						const char *name,
						BMessage *message,
						uint32 resizeMask = B_FOLLOW_NONE,
						uint32 flags = B_WILL_DRAW,
						int32 maxValue = 127); 
			~Knob();

void		AttachedToWindow();
void		Draw(BRect updateRect);

void		MouseDown( BPoint point );
void		MouseUp( BPoint point );
void		MouseMoved(	BPoint where, uint32 code, const BMessage *a_message);

void		SetValue(int32 value);

void		SetMaxValue(int32 value) { fMaxValue = value; SetValue(Value()); }
void		SetMinValue(int32 value) { fMinValue = value; SetValue(Value()); }

private:

BBitmap		*fBitmap;		// the image
bool		fMousedown;		// is mousebutton pressed?
BPoint		fMousePosition;	// where is the mouse while changing settings?

int32		fMaxValue;		// 
int32		fMinValue;		// 
};

#endif

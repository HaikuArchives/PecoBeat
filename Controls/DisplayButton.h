/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#ifndef _DISPLAY_BUTTON_H
#define _DISPLAY_BUTTON_H

#include <Control.h>

#include "Knob.h"

class BBitmap;

class DisplayButton : public BControl {

public:
				DisplayButton(	BRect frame, const char* name,
								const char* label, BMessage *message);
virtual			~DisplayButton();

virtual void	AttachedToWindow();
virtual	void	Draw(BRect updateRect);
virtual void	SetValue( int32 value );

virtual	void	MessageReceived(BMessage *msg);

void			SetMaxValue( int32 value ) { fKnob->SetMaxValue(value); SetValue(fKnob->Value()); }
void			SetMinValue( int32 value ) { fKnob->SetMinValue(value); SetValue(fKnob->Value()); }

BView			*fBgView;
BBitmap			*fBgBmp;
BBitmap			*fDisplayBmp;

private:
Knob			*fKnob;
};

#endif

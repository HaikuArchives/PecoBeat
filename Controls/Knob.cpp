/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#include "Knob.h"

#include <Beep.h>
#include <Bitmap.h>
#include <WindowScreen.h>

#include <iostream>
#include <math.h>

#include "App.h"

// TODO: I assume PI is supposed to be pi
#define PI 3.14159

Knob::Knob(	BRect frame, const char *name, BMessage *message,
	uint32 resizeMask, uint32 flags, int32 maxValue)
: 	BControl( frame, name, NULL, message, resizeMask, flags),
	fMousedown(false),
	fMaxValue( maxValue ),
	fMinValue( 0 )

{
	fBitmap = ((App *)be_app)->FetchResourceBitmap("knob");
}

void Knob::AttachedToWindow() {
	SetViewColor(B_TRANSPARENT_COLOR);
	SetTarget( Parent() );
}

void Knob::MouseDown( BPoint point ) {

	if (!IsEnabled()) return;

	SetMouseEventMask(B_POINTER_EVENTS, B_LOCK_WINDOW_FOCUS);
	fMousedown = true;
	be_app->HideCursor();
	fMousePosition = point;
	
	BControl::MouseDown( point );
}

void Knob::MouseUp( BPoint point ) {

	if (!IsEnabled() || !fMousedown) return;
	
	ConvertToScreen( &fMousePosition );
	set_mouse_position((int)fMousePosition.x, (int)fMousePosition.y);
	be_app->ShowCursor();
	fMousedown = false;
	InvokeNotify(Message());

	BControl::MouseUp( point );
}

void Knob::MouseMoved( BPoint where, uint32 code, const BMessage *a_message) {

	if (!IsEnabled() || !fMousedown || ConvertToScreen(where).y==fMousePosition.y) return;
	
	be_app->HideCursor();
	set_mouse_position((int)fMousePosition.x, (int)fMousePosition.y);
	be_app->HideCursor();
	
	int delta = (int)(fMousePosition.y-ConvertToScreen(where).y);
	if (delta<-10 || delta>10) return;
	
	// right mouse button or shift key multiplies delta with 10
	if (Window()->CurrentMessage()->FindInt32("buttons") == B_SECONDARY_MOUSE_BUTTON
		|| modifiers() & B_SHIFT_KEY) delta *= 10;
	be_app->HideCursor();
	
	SetValue( Value() + delta );
	InvokeNotify(Message());
	
	BControl::MouseMoved( where, code, a_message );

}

void Knob::Draw(BRect updateRect) {

	SetDrawingMode(B_OP_ALPHA);
	
	DrawBitmap(fBitmap, BPoint(0.0,0.0));

	BRect rect(8.0, 8.0, 9.0, 9.0);
		
	float dist = 6.5; // Abstand der Markierung vom Mittelpunkt
	float winkel = (float)(fMinValue-Value()) / (fMaxValue - fMinValue) * 1.7 * PI - .5;
	
	rect.OffsetBy(sinf(winkel)*dist, cosf(winkel)*dist);

	SetHighColor(255,0,0);
	FillEllipse(rect);
}

void Knob::SetValue(int32 value) {

	if (!IsEnabled()) return;
	
	if (value<fMinValue) value = fMinValue;
	else if (value>fMaxValue) value = fMaxValue;
	
	if (value==Value()) return;

	BControl::SetValue(value);
	
	Draw( Bounds() );
}

Knob::~Knob() {
	delete fBitmap;
}

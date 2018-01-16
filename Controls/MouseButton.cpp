/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#include <Bitmap.h>
#include <Control.h>
#include <String.h>

#include <iostream>

#include "App.h"
#include "MouseButton.h"
#include "PatternDefs.h"

MouseButton::MouseButton( BRect frame, int32 nr, BMessage *msg)
:	BControl( frame, "mousebutton", NULL, msg, 0, B_WILL_DRAW ),
	fBackgroundPicture( 0 ),
	fModePicture ( 0 ),
	fNr( nr )
{

	int32	numButtons;
	get_mouse_type(&numButtons);

	BString	resName;
	switch(numButtons) {
		case 1: resName = "1mousebutton"; break;
		case 2: resName = "2mousebuttons";	resName << nr; break;
		default: resName = "3mousebuttons";	resName << nr; break;
	}

	fBackgroundPicture = ((App *)be_app)->FetchResourceBitmap(resName.String());
	
	// Mouse mapping
	mouse_map	mouseMap;
	get_mouse_map(&mouseMap);
	
	switch ( nr ) {
		case 1: fMouseButton = mouseMap.button[0]; break;
		case 2: fMouseButton = numButtons == 3 ? mouseMap.button[2] : mouseMap.button[1]; break;
		case 3: fMouseButton = mouseMap.button[1]; break;
	}

	// Init Drawing
	SetDrawingMode(B_OP_ALPHA);

}

void MouseButton::AttachedToWindow() {

	BControl::AttachedToWindow();

	switch ( fNr ) {
		case 1: SetMode( NOTE_ON ); break;
		case 2: SetMode( NOTE_OFF ); break;
		case 3: SetMode( NOTE_RUBBER ); break;
	}
}


void MouseButton::SetMode( int32 mode ) {

	fMode = mode;
	switch (fMode) {
		case NOTE_ON: fModePicture = ((App *)be_app)->FetchResourceBitmap("note_on"); break;
		case NOTE_OFF: fModePicture = ((App *)be_app)->FetchResourceBitmap("note_off"); break;
		case NOTE_RUBBER: fModePicture = ((App *)be_app)->FetchResourceBitmap("rubber"); break;
	}

	BMessage	*msg = Message();
	msg->RemoveName("be:value");
	msg->AddInt32("be:value", fMode);
	msg->RemoveName("mousebutton");
	msg->AddInt32("mousebutton", fMouseButton);

	Invoke();

	Draw( Bounds() );
	
}

void MouseButton::Draw(BRect updateRect) {
	DrawBitmap(fBackgroundPicture, BPoint(0,0));
	DrawBitmap(fModePicture, BPoint(16,6));
}

void MouseButton::MouseDown(BPoint where) {
	switch (fMode) {
		case NOTE_ON: SetMode(NOTE_OFF); break;
		case NOTE_OFF: SetMode(NOTE_RUBBER);  break;
		case NOTE_RUBBER: SetMode(NOTE_ON); break;
	}
}

MouseButton::~MouseButton() {
	if (fBackgroundPicture) delete fBackgroundPicture;
	if (fModePicture) delete fModePicture;
}

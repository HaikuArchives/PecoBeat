/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#include "PatternTrashButton.h"

#include <Alert.h>
#include <Window.h>

#include <iostream>

#include "App.h"

PatternTrashButton::PatternTrashButton(BRect rect, BMessage *message)
	: BControl(rect, "patterntrashbutton", NULL, message, B_FOLLOW_NONE, B_WILL_DRAW),
	fMousePressed( false )
{

	be_app->Lock();
	fBitmap[0]	= ((App *)be_app)->FetchResourceBitmap("trash0");
	fBitmap[1]	= ((App *)be_app)->FetchResourceBitmap("trash1");
	be_app->Unlock();
	
	SetEnabled( false );

}

void PatternTrashButton::MouseMoved( BPoint where, uint32 code, const BMessage *a_message) {

	int32 buttons;
	Window()->CurrentMessage()->FindInt32("buttons", &buttons);

	fHasMessage = a_message && a_message->what=='drpt';
	
	if	(!buttons || !IsEnabled() || (!fHasMessage && !fMousePressed)) return;

	Update();

}

void PatternTrashButton::MouseDown(BPoint where) {
	
	if (!IsEnabled()) return;
	
	fMousePressed = true;
	Update();
	
	SetMouseEventMask(B_POINTER_EVENTS, B_SUSPEND_VIEW_FOCUS|B_LOCK_WINDOW_FOCUS );

};

void PatternTrashButton::MouseUp(BPoint where) {

	if (!IsEnabled()) return;

	if (fMousePressed && Bounds().Contains( where ) ) Invoke();
	
	fHasMessage = false;
	fMousePressed = false;
	Update();
	
};

void PatternTrashButton::MessageReceived( BMessage *message ) {

	if (!IsEnabled()) return;
	
	// Man kann ganz normal "invoken", da das zu löschende Pattern
	// aktiviert sein muss (sonst könnte es nicht gezogen werden!)
	if (message->what=='drpt') {
		Invoke();
		return;
	}

}

status_t PatternTrashButton::Invoke(BMessage *msg) {
	if ( (new BAlert("", "Are you sure, that you want to clear this pattern?", "Absolutely", "Oh, wrong button..."))->Go() ) {
		return B_OK;
	}
	return BControl::Invoke(msg);
}

void PatternTrashButton::Update() {

	if (!IsEnabled()) {
		DrawBitmap( fBitmap[0] );
		return;
	}
	
	BPoint	where;
	uint32	buttons;
		
	GetMouse( &where, &buttons );

	int32	newValue = Bounds().Contains( where ) && (fMousePressed || fHasMessage );
	if (newValue != Value()) {
		DrawBitmap( fBitmap[ newValue ] );
		SetValue( newValue );
	}
}

void PatternTrashButton::Draw(BRect updateRect) {

	DrawBitmap( fBitmap[ Value() ] );

}

PatternTrashButton::~PatternTrashButton() {

	for (int i=0; i<2; ++i) delete fBitmap[i];

}

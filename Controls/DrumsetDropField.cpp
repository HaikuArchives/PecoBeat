/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#include <Picture.h>
#include <Window.h>

#include <iostream>

#include "App.h"
#include "DrumsetDropField.h"
#include "Instruments.h"

DrumsetDropField::DrumsetDropField(BRect frame, const char *name, BMessage *message, uint32 resizeMask, uint32 flags)
	: BControl( frame, NULL, NULL, message, resizeMask, flags) {
}

void DrumsetDropField::AttachedToWindow() {
	
	BFont font;
	font.SetFamilyAndStyle("Swis721 BT","Roman"); 
	font.SetSize( 8.0 );
	SetFont(&font);
	
	SetLowColor( 66, 99, 132 );
	SetHighColor( 210, 220, 230 );
		
	BeginPicture( new BPicture() );
	DrawBitmap(((App *)be_app)->FetchResourceBitmap("dropfield"), BPoint(0.0, 0.0));
	DrawString("Drop instrument here!", BPoint(6.0, 12.0));
	fNormalBitmap = EndPicture();
	
	BeginPicture( new BPicture() );
	DrawBitmap(((App *)be_app)->FetchResourceBitmap("dropfield"), BPoint(0.0, 0.0));
	SetDrawingMode(B_OP_ALPHA);
	SetHighColor(0,0,0,80);
	FillRect(Bounds().InsetByCopy(1.0, 1.0));
	SetHighColor( 255, 255, 255 );
	DrawString("Drop instrument here!", BPoint(7.0, 13.0));
	fActiveBitmap = EndPicture();

	SetTarget(Parent());

}

void DrumsetDropField::Draw( BRect updateRect ) {
	DrawPicture( Value() ? fActiveBitmap : fNormalBitmap, BPoint( 0.0, 0.0 ));
}

void DrumsetDropField::MouseMoved( BPoint where, uint32 code, const BMessage *a_message ) {

	if (!a_message || a_message->what!='PecM') return;
	
	SetValue(code==B_ENTERED_VIEW||code==B_INSIDE_VIEW);
}

void DrumsetDropField::MessageReceived( BMessage *message ) {
	
	if (!message || message->what!='PecM') return;
	
	int8	key;
	message->FindInt8("key", &key);

	SetValue(key);
	InvokeNotify(Message());
	
	SetValue(0);

}

DrumsetDropField::~DrumsetDropField() {
	delete fNormalBitmap;
	delete fActiveBitmap;
}

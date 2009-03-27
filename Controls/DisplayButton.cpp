/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#include "App.h"
#include "DisplayButton.h"

#include <iostream>

#define FONT_SIZE 7.0

DisplayButton::DisplayButton(BRect frame, const char* name, const char* label, BMessage *message)
:	BControl( frame, name, label, message, B_FOLLOW_LEFT|B_FOLLOW_TOP, B_WILL_DRAW ),
	fBgView( 0 ),
	fBgBmp( 0 ),
	fDisplayBmp( 0 )
{
	fDisplayBmp = ((App *)be_app)->FetchResourceBitmap("display");

	fBgBmp = new BBitmap( fDisplayBmp->Bounds(), B_RGBA32, true );
	fBgView = new BView( fDisplayBmp->Bounds(), 0, 0, 0 );

	fBgBmp->Lock();
	fBgBmp->AddChild( fBgView );
	fBgView->SetDrawingMode( B_OP_ALPHA );
	fBgView->DrawBitmap( fDisplayBmp, BPoint( 0, 0 ) );
	fBgBmp->Unlock();

	fKnob		= new Knob( BRect( 35.0, FONT_SIZE, frame.right, frame.bottom ), NULL, new BMessage('volu') );
	AddChild( fKnob );
}

DisplayButton::~DisplayButton() {
	delete fBgBmp;
	delete fDisplayBmp;
}

void DisplayButton::AttachedToWindow()
{
	BControl::AttachedToWindow();
	SetViewColor( B_TRANSPARENT_COLOR );
}

void DisplayButton::Draw(BRect updateRect)
{

	BFont font;
	font.SetFamilyAndStyle("Swis721 BT","Roman"); 
	font.SetSize( FONT_SIZE );
	SetFont(&font);
	
	SetHighColor(80, 80, 80);
	SetLowColor(247, 247, 247);
	DrawString(Label(), BPoint(0, 6));

	fBgBmp->Lock();
	DrawBitmap( fBgBmp, BPoint( 0.0, FONT_SIZE ) );
	fBgBmp->Unlock();
		
	BControl::Draw( updateRect );

}

void DisplayButton::MessageReceived(BMessage *msg) {

	switch (msg->what) {
		case 'volu': {
			int32	value = msg->FindInt32( "be:value" );
			SetValue( value );
		} break;
		default:
			BControl::MessageReceived( msg );
	}
}

void DisplayButton::SetValue( int32 value ) {
	if (Value()==value) return;
	fKnob->SetValue( value );
	BControl::SetValue( fKnob->Value() );
	Invoke();
}

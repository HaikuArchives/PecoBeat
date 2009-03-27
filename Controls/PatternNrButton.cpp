/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#include "PatternNrButton.h"

#include <Alert.h>
#include <String.h>
#include <Window.h>

#include <iostream>

#include "App.h"
#include "Instruments.h"
#include "Song.h"

PatternNrButton::PatternNrButton(BRect rect, BMessage *message)
	: BControl(rect, "patternnrbutton", NULL, message, B_FOLLOW_NONE, B_WILL_DRAW),
	fMousePressed( false ),
	fMarked( -1 )
{

	// Fetch Bitmaps from Resource
	for(int i=0; i<12; ++i) {
		for (int j=0; j<2; ++j) {
			BString	fNummer;
			fNummer << i+1 << "_" << (j==0 ? "normal" : "active" );
			fBitmap[i][j] = ((App *)be_app)->FetchResourceBitmap(fNummer.String());
		}
	}
	
	SetEnabled( false );
	SetValue( 1 );
}

PatternNrButton::~PatternNrButton() {

	for(int i=0; i<12; ++i)
		for (int j=0; j<2; ++j)
			delete fBitmap[i][j];

}

void PatternNrButton::MouseMoved( BPoint where, uint32 code, const BMessage *a_message) {

	int32 buttons;
	Window()->CurrentMessage()->FindInt32("buttons", &buttons);
	if (!buttons && fMousePressed) fMousePressed = false;
	
	if	(!Value() || !buttons || !IsEnabled() || !fMousePressed ) return;

	// Pattern wird über ein anderes gezogen
	if (a_message && a_message->what=='drpt') {
		int32	wo = FindButtonNr( where ) - 1;
		if ( wo!=fMarked ) {
			if ( wo > -1 && (wo+1)!=Value() ) {
				SetHighColor( 30, 30, 30, 150 );
				StrokeRoundRect( fBitmap[0][0]->Bounds().OffsetToCopy( wo * 17.0, 0.0 ), 3.0, 3.0 );
				SetHighColor( 128, 128, 128, 90 );
				FillRoundRect( fBitmap[0][0]->Bounds().OffsetToCopy( wo * 17.0, 0.0 ), 3.0, 3.0 );
			}
			if (fMarked>-1) DrawBitmap( fBitmap[fMarked][Value()-1 == fMarked ? 1 : 0], BPoint( fMarked * 17.0, 0.0 ) );

			fMarked = wo;
		}
	}
	
	// Keine Drop-Message -> Nummer wird herausgezogen
	else {

		BBitmap		*dragBitmap = new BBitmap( BRect( 0.0, 0.0, fBitmap[0][0]->Bounds().right + 1, fBitmap[0][0]->Bounds().bottom + 1 ), B_RGBA32, true );
		BView		view( dragBitmap->Bounds(), 0, 0, 0 );
		dragBitmap->Lock();
		dragBitmap->AddChild( &view );
		view.SetHighColor( B_TRANSPARENT_COLOR );
		view.FillRect( view.Bounds() );

		// Bitmap
		view.SetDrawingMode(B_OP_ALPHA);
		view.SetHighColor(255, 0, 0, 150); 
		view.SetBlendingMode(B_CONSTANT_ALPHA, B_ALPHA_COMPOSITE);
		view.DrawBitmap( fBitmap[Value()-1][1], BPoint( 0.0, 0.0 ) );
		
		// Transparenz
		view.SetDrawingMode(B_OP_COPY);
		view.SetHighColor( B_TRANSPARENT_COLOR );
		BRect	bounds( Bounds() );
		bounds.right = 14; bounds.bottom--;
		view.SetPenSize( 2 );
		view.StrokeRoundRect( bounds.InsetByCopy( -2.0, -2.0), 5, 5 );
		bounds.InsetBy( -1.0, -1.0);
		view.StrokeLine( BPoint( bounds.right, bounds.bottom ), BPoint(bounds.right - 1, bounds.bottom - 1) );

		// Schatten
		view.SetHighColor( 0, 0, 0, 30 );
		view.SetPenSize( 1.0 );
		view.StrokeLine( BPoint( bounds.right, 4.0 ), BPoint( bounds.right, bounds.bottom - 3.0 ) );
		view.StrokeLine( BPoint( bounds.right - 3.0, bounds.bottom ) );
		view.StrokeLine( BPoint( 4.0, bounds.bottom) );

		bounds.right++; bounds.bottom++;
		view.SetHighColor( 0, 0, 0, 10 );
		view.StrokeLine( BPoint( bounds.right, 4.0 ), BPoint( bounds.right, bounds.bottom - 4.0 ) );
		view.StrokeLine( BPoint( bounds.right - 4.0, bounds.bottom ) );
		view.StrokeLine( BPoint( 4.0, bounds.bottom) );

		view.StrokeLine( BPoint( bounds.right, bounds.bottom - 2.0 ), BPoint( bounds.right - 2.0, bounds.bottom ) );

		dragBitmap->RemoveChild( &view );
		dragBitmap->Unlock();

		BMessage	drag_message('drpt');
		drag_message.AddInt32("source", Value() );
		drag_message.AddInt32("be:actions", B_TRASH_TARGET);

		DragMessage(&drag_message, dragBitmap, B_OP_ALPHA, BPoint( where.x - (Value() - 1) * 17.0, where.y ) );

	}
}

void PatternNrButton::MouseDown(BPoint where) {
	
	if (!IsEnabled()) return;
	
	SetValue( FindButtonNr( where ) );
	fMousePressed = true;
	
	SetMouseEventMask(B_POINTER_EVENTS, B_SUSPEND_VIEW_FOCUS|B_LOCK_WINDOW_FOCUS );

};

void PatternNrButton::MouseUp(BPoint where) {

	if ( !Value() ) return;
	
	fMarked = -1;
	fMousePressed = false;

};

void PatternNrButton::AttachedToWindow() {
	SetViewColor( B_TRANSPARENT_COLOR );
	SetDrawingMode( B_OP_ALPHA );
	BControl::AttachedToWindow();
}

void PatternNrButton::Draw(BRect updateRect) {

	for (int i=0; i<12; ++i) {
		DrawBitmap( fBitmap[i][ IsEnabled() && Value()-1 == i ? 1 : 0], BPoint(i * 17., 0.0) );
	}

}

void PatternNrButton::MessageReceived( BMessage *message ) {

	if (!IsEnabled()) return;

	switch( message->what ) {
		case B_TRASH_TARGET: {
			if (message->IsReply()) {
				BControl *trash = (BControl *)Window()->FindView("patterntrashbutton");
				if (trash) trash->Invoke();
			}
		} break;
	
		case 'drpt': {

			BPoint	where;
			uint32	buttons;
		
			GetMouse( &where, &buttons );
			int32	wo = FindButtonNr( where );
			if (wo==Value()) return;

			gSong->Lock();
			bool containsdata = gSong->fPattern->PartContainsData( wo-1 );
			gSong->Unlock();

			if (!containsdata || !(new BAlert("", "This part already contains data!", "What shalls?", "Don't overwrite!"))->Go()) {
				message->AddInt32("destination", wo );
				Window()->PostMessage( message );
			}
			Draw( Bounds() );
		}
	
		default:
		
			BControl::MessageReceived(message);

	}

}

void PatternNrButton::SetValue(int32 value) {

	if ( (!IsEnabled() && Value()) || Value()==value ) return;
	
	BControl::SetValue( value );
	Invoke();

}

char PatternNrButton::FindButtonNr( BPoint where ) {

	if (!Bounds().Contains( where )) return 0;
	
	return (char)(where.x / 17 + 1);
	
};

/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */


#include "Skala.h"

#include <Bitmap.h>

#include <iostream>

#include "App.h"
#include "Song.h"

Skala::Skala( BRect frame)
:	BView( frame, "Skala", B_FOLLOW_LEFT_RIGHT, B_WILL_DRAW | B_NAVIGABLE | B_FULL_UPDATE_ON_RESIZE),
	fSkalaBackground( ((App *)be_app)->FetchResourceBitmap("skala") ),
	fSkalaPfeil( ((App *)be_app)->FetchResourceBitmap("cursor_pfeil") )
{
	frame = BRect( 0.0, 0.0, Bounds().right - 2.0, Bounds().bottom - 2.0 );
	fBgBitmap	= new BBitmap( frame, B_RGB32, true);
	fBgView		= new BView( frame, NULL, B_FOLLOW_ALL, B_WILL_DRAW);

	fBgBitmap->Lock();
	fBgBitmap->AddChild(fBgView);
	fBgView->SetDrawingMode(B_OP_ALPHA);
	fBgBitmap->Unlock();
	
}

Skala::~Skala() {
	delete fSkalaBackground;
	delete fSkalaPfeil;
	delete fBgBitmap;
}

void Skala::AttachedToWindow() {

	// Alpha
	SetViewColor( B_TRANSPARENT_COLOR );
}

void Skala::Draw( BRect updateRect ) {

	// Rand	
	BRect	rect( Bounds() );
	SetHighColor( 0, 0, 0 );
	StrokeLine( BPoint( rect.left, rect.bottom ), BPoint( rect.left, rect.top + 1.0 ) );
	StrokeLine( BPoint( rect.left + 1.0, rect.top ), BPoint( rect.right - 1.0, rect.top ) );
	StrokeLine( BPoint( rect.right, rect.top + 1.0 ), BPoint( rect.right, rect.bottom ) );

	// Buffer
	fBgBitmap->Lock();

	// Skala
	rect = BRect(0.0, 0.0, Bounds().right - 2.0, Bounds().bottom - 2.0 );
	fBgView->DrawBitmap (fSkalaBackground, BRect( 0.0, 0.0, 101.0, 5.0), rect);
	
	// Cursor
	int32	cursorpos = gSong->CursorPosition();

	gSong->Lock();
	bool	isPlaying = gSong->IsPlaying();
	int32	activePart = gSong->ActivePart();
	gSong->Unlock();

	if ( isPlaying && (cursorpos>>8 == activePart) ) {
		cursorpos = cursorpos & 0xff;
		float cursor_left = cursorpos  * (rect.Width() + 1.0) / 256 + 4.0;
		fBgView->SetHighColor( 255, 180 - cursorpos / 2, 0, 40 + cursorpos/4);
		fBgView->FillRect( BRect( 0.0, 0.0, cursor_left, 5.0 ) );
		fBgView->Flush();
		fBgView->DrawBitmap (fSkalaPfeil, BPoint(cursor_left - 9.0, -1.0));
	}
	DrawBitmap(fBgBitmap, BPoint(1.0, 1.0));
	
	// Free buffer
	fBgBitmap->Unlock();
	
}

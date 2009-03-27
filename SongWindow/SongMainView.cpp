/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#include "SongMainView.h"

#include <Bitmap.h>
#include <Window.h>

#include <iostream>

#include "App.h"
#include "PlayListField.h"
#include "SongToolBar.h"

SongMainView::SongMainView( BRect frame )
:	BView( frame, "main", B_FOLLOW_ALL, B_WILL_DRAW|B_DRAW_ON_CHILDREN|B_FRAME_EVENTS )
{
}

void SongMainView::AttachedToWindow() {

	// Background Song
	be_app->Lock();
	SetViewBitmap(((App *)be_app)->fHGPattern);
	be_app->Unlock();

	AddChild( new PlayListField( BRect( 0.0, 0.0, 15.0 * 16, 17.0 * 4 ).OffsetToCopy( 8.0, 8.0 ), new BMessage('plls') ) );
}

void SongMainView::MessageReceived( BMessage *msg ) {

	switch (msg->what) {
		case 'crsr': {
		} break;

		default:
			BView::MessageReceived( msg );
	}
		
}

SongMainView::~SongMainView() {
}

/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#include "SongWindow/SongToolBar.h"

#include <iostream>

#include <InterfaceDefs.h>
#include <PictureButton.h>
#include <string.h>

#include "App.h"
#include "Knob.h"
#include "PlayButton.h"
#include "SpeedDisplayButton.h"
#include "VolumeDisplayButton.h"

SongToolBar::SongToolBar( BRect frame )
	:	BView( frame, "toolbar", B_FOLLOW_NONE, B_WILL_DRAW|B_DRAW_ON_CHILDREN)
{
}

void SongToolBar::AttachedToWindow () {

	be_app->Lock();
	SetViewBitmap(((App *)be_app)->FetchResourceBitmap("toolbar_hg"));
	be_app->Unlock();

	BRect	bounds;

	BView::AttachedToWindow();

	// Play Button
	bounds = BRect( 0.0, 0.0, 24.0, 25.0);
	AddChild( new PlayButton( bounds.OffsetToCopy(10.0, 0.0), "play", PLAY_SONG ) );

	bounds = BRect( 0.0, 0.0, 53.0, 25.0 );
	BControl	*displayButton;
	
	displayButton = new SpeedDisplayButton( bounds.OffsetToCopy( 100.0, 0.0), "speed", new BMessage('sped') );
	AddChild( displayButton );
	
	displayButton = new VolumeDisplayButton( bounds.OffsetToCopy( 200.0, 0.0), "volume", new BMessage('volu') );
	AddChild( displayButton );

}

void SongToolBar::SetEnabled( bool enable ) {

	if (enable) {
		((BControl *)FindView("play"))->SetEnabled( true );
	}
	else {
		((BControl *)FindView("play"))->SetEnabled( false );
	}
}

void SongToolBar::MessageReceived(BMessage *msg) {
	BView::MessageReceived( msg );
}

SongToolBar::~SongToolBar() {
}

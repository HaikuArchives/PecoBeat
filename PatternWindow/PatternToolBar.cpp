/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#include "PatternToolBar.h"

#include <iostream>

#include "App.h"
#include "MouseButton.h"
#include "PatternNrButton.h"
#include "PatternTrashButton.h"
#include "PlayButton.h"

PatternToolBar::PatternToolBar( BRect frame )
	:	BView( frame, "toolbar", B_FOLLOW_NONE, B_WILL_DRAW|B_DRAW_ON_CHILDREN) {
}

void PatternToolBar::AttachedToWindow () {

	SetViewBitmap(((App *)be_app)->FetchResourceBitmap("toolbar_hg"));

	SetViewColor( B_TRANSPARENT_COLOR );
	SetLowColor(210, 210, 210);

	BRect	bounds;

	// Play Button
	bounds = BRect( 0.0, 0.0, 24.0, 25.0);
	PlayButton	*playbutton = new PlayButton( bounds.OffsetToCopy(140.0, 0.0), "play", PLAY_PATTERN );
	AddChild(playbutton);

	// PatternNr Button
	bounds	= BRect( 0.0, 0.0, 12 * 17.0 - 2.0, 17.0);
	PatternNrButton	*nrbutton = new PatternNrButton( bounds.OffsetToCopy(250.0, 4.0), new BMessage('ptnr') );
	AddChild(nrbutton);

	// PatternTrash Button
	bounds	= BRect( 0.0, 0.0, 19.0, 20.0);
	PatternTrashButton	*trashbutton = new PatternTrashButton( bounds.OffsetToCopy(456.0, 4.0), new BMessage('pttr') );
	AddChild(trashbutton);

	// Mouse Buttons
	int32	numButtons;
	get_mouse_type(&numButtons);

	for( int i = 0; i<3; i++ ) {
		fMouseButton[2-i] =	(i < numButtons )	?
							new MouseButton( BRect(0, 0, 34, 24).OffsetToCopy(Bounds().right - 39 - 38 * i, 1.0),
								numButtons - i, new BMessage('mbut'))
							: 0;
		if (fMouseButton[2-i]) AddChild ( fMouseButton[2-i] );
	}
}

void PatternToolBar::SetEnabled( bool enable ) {

	if (enable) {
		((BControl *)FindView("play"))->SetEnabled( true );
		((BControl *)FindView("patternnrbutton"))->SetEnabled( true );
		((BControl *)FindView("patterntrashbutton"))->SetEnabled( true );
	}
	else {
		PlayButton	*playButton = ((PlayButton *)FindView("play"));
		playButton->SetEnabled( false );
		
		((BControl *)FindView("patternnrbutton"))->SetEnabled( false );
		((BControl *)FindView("patterntrashbutton"))->SetEnabled( false );
	}
}

void PatternToolBar::MessageReceived(BMessage *msg) {
	BView::MessageReceived( msg );
}

PatternToolBar::~PatternToolBar() {
}

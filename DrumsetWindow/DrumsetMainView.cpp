/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */


#include "DrumsetMainView.h"

#include <Window.h>

#include "App.h"
#include "DrumsetButton.h"
#include "Instruments.h"

DrumsetMainView::DrumsetMainView( BRect frame )
:	BView( frame, "main", B_FOLLOW_TOP_BOTTOM | B_FOLLOW_LEFT_RIGHT, B_WILL_DRAW)
{
}

void DrumsetMainView::AttachedToWindow() {

	for (int i=kFirstMidiID; i<kFirstMidiID+60; i++ ) {
		AddChild ( new DrumsetButton(kDrumsetButtonBounds, i, new BMessage('psnd')) );
	}

	SetViewBitmap(((App *)be_app)->fHGPattern);

}

void DrumsetMainView::MouseDown( BPoint where ) {
	Window()->Activate();
	BView::MouseDown( where );
}

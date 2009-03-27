/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#ifndef _SONG_TOOLBAR_H
#define _SONG_TOOLBAR_H

#include <View.h>

#include "SongWindow/SongWindow.h"

class SongToolBar : public BView {

public:
			SongToolBar( BRect frame );

void		AttachedToWindow();
void		MessageReceived(BMessage *msg);

void		SetEnabled( bool status = true );
			~SongToolBar();

private:
};

#endif

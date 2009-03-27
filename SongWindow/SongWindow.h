/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#ifndef _SONG_WINDOW_H
#define _SONG_WINDOW_H

#include <Window.h>
#include <List.h>

class BView;
class SongToolBar;
class SongRowView;

class SongWindow : public BWindow {

public:
				SongWindow( BRect frame );
				~SongWindow();
				
bool			QuitRequested();
void 			MessageReceived(BMessage *message);

private:
BView			*fMainView;
SongToolBar		*fToolBar;
};

#endif

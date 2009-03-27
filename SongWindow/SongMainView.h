/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#ifndef _SONG_MAIN_VIEW_H
#define _SONG_MAIN_VIEW_H

#include <List.h>
#include <Message.h>
#include <View.h>

class SongMainView : public BView {

public:
				SongMainView( BRect frame );
				~SongMainView();

void			AttachedToWindow();
void			MessageReceived( BMessage *msg );

};

#endif

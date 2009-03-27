/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#ifndef _PATTERN_WINDOW_H
#define _PATTERN_WINDOW_H

#include <Window.h>
#include <List.h>

#include "Pattern.h"

class PatternMainView;
class PatternToolBar;
class PatternRowView;

class PatternWindow : public BWindow {

public:
				PatternWindow( BRect frame );
				~PatternWindow();
				
bool			QuitRequested();
void 			MessageReceived(BMessage *message);
int32			DrawMode( int32 mousebuttons );

private:
PatternMainView		*fMainView;
PatternToolBar		*fToolBar;
int32				fDrawMode[3];
};

#endif

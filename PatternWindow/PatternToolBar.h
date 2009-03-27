/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#ifndef _PATTERN_TOOLBAR_H
#define _PATTERN_TOOLBAR_H

#include <View.h>

#include "PatternWindow.h"

class MouseButton;

class PatternToolBar : public BView {

//friend class PatternWindow;

public:
			PatternToolBar( BRect frame );

void		AttachedToWindow();
void		MessageReceived(BMessage *msg);

void		SetEnabled( bool status = true );
			~PatternToolBar();

private:
BPictureButton	*fSelectedButton;
MouseButton		*fMouseButton[3];
};

#endif

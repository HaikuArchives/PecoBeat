/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */


#ifndef _SIDE_GRIP_H
#define _SIDE_GRIP_H

#include <Control.h>

#include "PatternWindow.h"

class BBitmap;
class PatternWindow;

class SideGrip : public BControl {

public:
				SideGrip(BRect frame, BMessage *message);
void			AttachedToWindow();
void			Draw(BRect updateRect);
void			MouseDown(BPoint where);
void			MouseMoved(BPoint pt, uint32 code, const BMessage *msg);
				~SideGrip();

private:
BBitmap			*fBitmap0, *fBitmap1;
bool			fIsDragged;
};

#endif

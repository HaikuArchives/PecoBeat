/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

/* Dieser Button repräsentiert eine Taste der Maus */

#ifndef _MOUSE_BUTTON_H
#define _MOUSE_BUTTON_H

#include <Control.h>

class BBitmap;

class MouseButton : public BControl {

public:
				MouseButton( BRect frame, int32 nr, BMessage *msg);
				~MouseButton();
void			Draw(BRect updateRect);
int32			Mode() { return fMode; }
void			SetMode( int32 mode );
void 			MouseDown(BPoint where);
void			AttachedToWindow();

private:
BBitmap			*fBackgroundPicture;
BBitmap			*fModePicture;
int32			fMode;
int32			fNr;			// Welche Taste (v.l.n.r.)
int32			fMouseButton;	// gemapped auf PRIMARY, SECONDARY...
};

#endif

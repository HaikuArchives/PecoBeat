/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#ifndef _PLAY_BUTTON_H
#define _PLAY_BUTTON_H

#include <PictureButton.h>

#include "PlayModes.h"

class PlayButton : public BPictureButton {

public:
				PlayButton(BRect frame, const char* name, uint32 playmode);
				~PlayButton();

void			AttachedToWindow();
void			ChangeState( bool buttondown );
status_t		Invoke(BMessage *msg = NULL);

void			MouseDown(BPoint where);
void			MouseUp(BPoint where);
void			MouseMoved(BPoint pt, uint32 code, const BMessage *msg);

void			SetEnabled(bool on);

void			SetAsStopButton( bool doit = true );

private:

BPicture		*fPlay0, *fPlay1, *fStop0, *fStop1;
bool			fMousePressedOverButton;
bool			fIsPlayButton;
uint32			fPlayMode;
};

#endif

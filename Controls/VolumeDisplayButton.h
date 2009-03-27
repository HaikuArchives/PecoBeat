/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */


#ifndef _VOLUME_DISPLAY_BUTTON_H
#define _VOLUME_DISPLAY_BUTTON_H

#include "DisplayButton.h"

class VolumeDisplayButton : public DisplayButton {

public:
		VolumeDisplayButton( BRect frame, const char* name,	BMessage *message);

void	Draw(BRect updateRect);
};

#endif

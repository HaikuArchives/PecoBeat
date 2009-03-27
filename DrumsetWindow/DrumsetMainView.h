/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */


#ifndef _DRUMSET_MAIN_VIEW_H
#define _DRUMSET_MAIN_VIEW_H

#include <View.h>

class DrumsetMainView : public BView {

public:
				DrumsetMainView( BRect frame );

void			AttachedToWindow();
void			MouseDown( BPoint where );

};

#endif

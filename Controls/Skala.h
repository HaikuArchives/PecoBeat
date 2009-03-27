/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */


#ifndef _SKALA_H
#define _SKALA_H

#include <View.h>

class BBitmap;

class Skala : public BView {

public:
				Skala(	BRect frame );
				~Skala();

void			AttachedToWindow();
void			Draw( BRect updateRect );

private:

BBitmap			*fSkalaBackground;
BBitmap			*fSkalaPfeil;

BBitmap			*fBgBitmap;
BView			*fBgView;
};

#endif

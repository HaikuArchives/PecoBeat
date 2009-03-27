/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */


#include "VolumeDisplayButton.h"

VolumeDisplayButton::VolumeDisplayButton(BRect frame, const char* name, BMessage *message)
:	DisplayButton( frame, name, "Volume", message )
{
	fBgBmp->Lock();
	fBgView->SetHighColor( 0, 0, 0, 25 );
	fBgBmp->Unlock();
}

void VolumeDisplayButton::Draw(BRect updateRect)
{

	fBgBmp->Lock();
	fBgView->DrawBitmap( fDisplayBmp, BPoint( 0, 0 ) );

	BRect rect = fBgBmp->Bounds().InsetByCopy( 3.0, 3.0 );
	for (int i = 0; i<Value(); ++i ) {
		int x = ((int)((float)i / 128 * rect.Width()) >> 1) << 1;
		fBgView->StrokeLine( BPoint( 4.0 + x, 3.0 + rect.Height()), BPoint( 4.0 + x, 3.0 + rect.Height() - (float)i / 128 * rect.Height() ));
	}
	fBgView->Flush();
	fBgBmp->Unlock();

	DisplayButton::Draw( updateRect );
}

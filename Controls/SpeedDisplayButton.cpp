/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */


#include "SpeedDisplayButton.h"

#include <Font.h>
#include <String.h>

SpeedDisplayButton::SpeedDisplayButton(BRect frame, const char* name, BMessage *message)
:	DisplayButton( frame, name, "Speed", message )
{

	fBgBmp->Lock();
	BFont font;
	font.SetFamilyAndStyle("Swis721 BT","Bold"); 
	font.SetSize( 14 );
	fBgView->SetFont(&font);
	fBgBmp->Unlock();
	
	SetMinValue( 40 );
	SetMaxValue( 600 );
}

void SpeedDisplayButton::Draw(BRect updateRect)
{

	fBgBmp->Lock();
	fBgView->DrawBitmap( fDisplayBmp, BPoint( 0, 0 ) );
	fBgView->SetHighColor( 0, 0, 0, 140 );

	BRect rect = fBgBmp->Bounds().InsetByCopy( 2.0, 3.0 );

	BString	string;
	string << Value();
	fBgView->DrawString( string.String(), BPoint(rect.right - fBgView->StringWidth( string.String() ), 14.0) );

	fBgView->Flush();
	fBgBmp->Unlock();

	DisplayButton::Draw( updateRect );
}

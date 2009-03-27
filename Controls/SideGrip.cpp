/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */


#include "SideGrip.h"

#include <Bitmap.h>
#include <Region.h>
#include <Roster.h>
#include <Screen.h>
#include <Window.h>

#include <iostream>

#include "App.h"
#include "PatternRowView.h"
#include "PatternMainView.h"
#include "PatternWindow.h"

SideGrip::SideGrip(BRect frame, BMessage *message)
:	BControl(frame, "SideGrip", NULL, message, B_FOLLOW_NONE, B_WILL_DRAW),
	fBitmap0( ((App *)be_app)->FetchResourceBitmap("sidegrip0") ),
	fBitmap1( ((App *)be_app)->FetchResourceBitmap("sidegrip1") )
{
}

void SideGrip::AttachedToWindow() {

	SetViewColor(B_TRANSPARENT_COLOR);
	SetDrawingMode(B_OP_ALPHA);

	SetTarget( Parent() );

}

void SideGrip::Draw(BRect updateRect) {

	BPoint	where;
	uint32	buttons;

	GetMouse( &where, &buttons, false );
	bool drawActive = ((!buttons && Window()->IsActive() && Bounds().Contains(where)) || !Parent()->Parent());
	DrawBitmap( drawActive ? fBitmap1 : fBitmap0 );
}

void SideGrip::MouseDown(BPoint where)
{

	if (!IsEnabled()) return;
	
	BMessage	*msg = Message();
	if ( !msg->HasPointer("instrumentRow") )
		msg->AddPointer( "instrumentRow", Parent() );
	
	Invoke();

}

void SideGrip::MouseMoved(BPoint pt, uint32 code, const BMessage *msg)
{

	if (!IsEnabled() || msg || !Window()->IsActive()) return;
	
	Draw( Bounds() );
}

SideGrip::~SideGrip() {
	delete fBitmap0;
	delete fBitmap1;
}

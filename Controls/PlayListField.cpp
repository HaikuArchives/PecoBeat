/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */


#include "PlayListField.h"

#include <Beep.h>
#include <MessageRunner.h>
#include <String.h>
#include <Window.h>

#include <iostream>

#include "App.h"
#include "Song.h"

#include "SongWindow/SongToolBar.h"

PlayListField::PlayListField(BRect rect, BMessage *message)
	: BControl(rect, "playlistfield", NULL, message, B_FOLLOW_NONE, B_WILL_DRAW),
	fBgView( new BView( rect.OffsetToCopy( 0.0, 0.0 ), 0, 0, 0) ),
	fBgBitmap( new BBitmap( rect.OffsetToCopy( 0.0, 0.0 ), B_RGBA32, true ) ),
	fMessageRunner( 0 ),
	fMouseMoved( false )

{

	fBgBitmap->Lock();

	fBgBitmap->AddChild( fBgView );

	BFont font;
	font.SetFamilyAndStyle("Swis721 BT","Roman"); 
	font.SetSize( 8 );
	fBgView->SetFont(&font);

	fBgBitmap->Unlock();

	be_app->Lock();

	fNrBitmap[0][0] = ((App *)be_app)->FetchResourceBitmap("nr_leer");
	fNrBitmap[0][1] = ((App *)be_app)->FetchResourceBitmap("nr_leer_active");

	for (int i=1; i<=12; ++i) {
		BString	resName;

		resName << i;
		resName << "_normal";
		fNrBitmap[i][0] = ((App *)be_app)->FetchResourceBitmap(resName.String());

		resName = "";
		resName << i;
		resName << "_active";
		fNrBitmap[i][1] = ((App *)be_app)->FetchResourceBitmap(resName.String());
	}

	be_app->Unlock();
	
	fMouseBeforeNr = -1;

	fMousePressed = false;
	fDragPosition = -1;
	fDragging = false;
	
	SetDrawingMode( B_OP_ALPHA );

}

PlayListField::~PlayListField() {

	delete fMessageRunner;

	for (int i=0; i<13; ++i)
		for (int j=0; j<2; ++j)
			delete fNrBitmap[i][j];
	
	fBgBitmap->Lock();
	delete fBgBitmap;
}

void PlayListField::AttachedToWindow() {

	BControl::AttachedToWindow();	
	Update( true );
	
	SetViewColor( B_TRANSPARENT_COLOR );

}

void PlayListField::MouseMoved( BPoint where, uint32 code, const BMessage *a_message) {

	static BPoint lastPoint;
	if (where == lastPoint) return;
	lastPoint = where;
	
	fMouseMoved = true;
	
	int32 buttons;
	Window()->CurrentMessage()->FindInt32("buttons", &buttons);
	if (!buttons) {
		if (fMouseBeforeNr!=-1) MouseUp( where );
		return;
	}

	if ( a_message && (a_message->what=='drpt' || a_message->what=='remp') ) {
	
		SetMouseEventMask(B_POINTER_EVENTS, B_SUSPEND_VIEW_FOCUS|B_LOCK_WINDOW_FOCUS );
		
		if (!UpdateMousePosition( where ) ) return;

		int32 newDragPosition = fMouseBeforeNr;
		
		if (newDragPosition==fDragPosition) return;
		
		int32	nr;
		if (a_message->FindInt32( a_message->what=='drpt' ? "source" : "nr", &nr ) != B_OK) return;
		
		if (fDragPosition==-1) { // add

			gSong->Lock();
			int32	lastFilledNr = gSong->LastFilledNr();
			gSong->Unlock();

			if (lastFilledNr==63) { beep(); return; }
			
			if (newDragPosition>lastFilledNr+1) newDragPosition = lastFilledNr+1;
			
			gSong->Lock();
			for (int i = lastFilledNr; i>=newDragPosition; --i)
				gSong->SetPlayListEntry(i+1, gSong->PlayListEntry(i) );
			
			if (lastFilledNr>-1 && gSong->ActiveNr()>=newDragPosition)
				gSong->SetActiveNr(gSong->ActiveNr()+1);

			gSong->SetLastFilledNr( gSong->LastFilledNr() + 1);
			
			gSong->SetPlayListEntry(newDragPosition, nr-1);
			gSong->Unlock();

			if (lastFilledNr==-1) // Dies ist noch der vorherige Wert!
				((SongToolBar *)Window()->FindView("toolbar"))->SetEnabled();

		}
		else if (newDragPosition==-1) { // remove

			gSong->Lock();
			for (int i = fDragPosition; i<gSong->LastFilledNr(); ++i)
				gSong->SetPlayListEntry(i, gSong->PlayListEntry(i+1) );

			gSong->SetPlayListEntry(gSong->LastFilledNr(), -1);
			gSong->SetLastFilledNr( gSong->LastFilledNr() - 1 );

			if (gSong->LastFilledNr()==-1 || fDragPosition == gSong->ActiveNr() || fDragPosition == gSong->LastFilledNr()) {
				be_app->PostMessage('stop');

				gSong->SetActiveNr(0);
				if (gSong->LastFilledNr()==-1 ) {
					((SongToolBar *)Window()->FindView("toolbar"))->SetEnabled( false );
				}
			}
			else {
				if (gSong->ActiveNr()>fDragPosition)
					gSong->SetActiveNr(gSong->ActiveNr()-1);
			}
			gSong->Unlock();
			
		}
		else { // move

			gSong->Lock();
			if (newDragPosition>gSong->LastFilledNr()) newDragPosition = gSong->LastFilledNr();
			gSong->Unlock();

			if (newDragPosition==fDragPosition) return;
			
			gSong->Lock();
			if (newDragPosition<fDragPosition) {
				for (int i = fDragPosition; i>newDragPosition; --i)
					gSong->SetPlayListEntry(i, gSong->PlayListEntry(i-1) );
				if (gSong->ActiveNr()==newDragPosition)
					gSong->SetActiveNr(gSong->ActiveNr()+1);
			}
			else {
				for (int i = fDragPosition; i<newDragPosition; ++i)
					gSong->SetPlayListEntry(i, gSong->PlayListEntry(i+1) );
				if (gSong->ActiveNr()==newDragPosition)
					gSong->SetActiveNr(gSong->ActiveNr()-1);
			}

			gSong->SetPlayListEntry(newDragPosition, nr-1);
			gSong->Unlock();
			
			Update( true );

		}
		fDragPosition = newDragPosition;
		
		Update();
		
	}
	else if (fMousePressed) {
		
		fMousePressed = false;

		int32	nr = FindButtonAt(where);

		BBitmap		*dragBitmap = new BBitmap( BRect( 0.0, 0.0, 16.0, 19.0 ), B_RGBA32, true );
		BView		view( dragBitmap->Bounds(), 0, 0, 0 );
		dragBitmap->Lock();
		dragBitmap->AddChild( &view );
		view.SetHighColor( B_TRANSPARENT_COLOR );
		view.FillRect( view.Bounds() );
	
		// Bitmap
		view.SetDrawingMode(B_OP_ALPHA);
		view.SetHighColor(255, 0, 0, 180); 
		view.SetBlendingMode(B_CONSTANT_ALPHA, B_ALPHA_COMPOSITE);
		gSong->Lock();
		view.DrawBitmap( fNrBitmap[gSong->PlayListEntry(nr)+1][0], BPoint( 0.0, 0.0 ) );
		gSong->Unlock();
		
		// Transparenz
		view.SetDrawingMode(B_OP_COPY);
		view.SetHighColor( B_TRANSPARENT_COLOR );
		BRect	bounds( Bounds() );
		bounds.right = 14; bounds.bottom = 17.0;
		view.SetPenSize( 2 );
		view.StrokeRoundRect( bounds.InsetByCopy( -2.0, -2.0), 5, 5 );
		bounds.InsetBy( -1.0, -1.0);
		view.StrokeLine( BPoint( bounds.right, bounds.bottom ), BPoint(bounds.right - 1, bounds.bottom - 1) );

		// Schatten
		view.SetHighColor( 0, 0, 0, 30 );
		view.SetPenSize( 1.0 );
		view.StrokeLine( BPoint( bounds.right, 4.0 ), BPoint( bounds.right, bounds.bottom - 3.0 ) );
		view.StrokeLine( BPoint( bounds.right - 3.0, bounds.bottom ) );
		view.StrokeLine( BPoint( 4.0, bounds.bottom) );
	
		bounds.right++; bounds.bottom++;
		view.SetHighColor( 0, 0, 0, 10 );
		view.StrokeLine( BPoint( bounds.right, 4.0 ), BPoint( bounds.right, bounds.bottom - 4.0 ) );
		view.StrokeLine( BPoint( bounds.right - 4.0, bounds.bottom ) );
		view.StrokeLine( BPoint( 4.0, bounds.bottom) );
	
		view.StrokeLine( BPoint( bounds.right, bounds.bottom - 2.0 ), BPoint( bounds.right - 2.0, bounds.bottom ) );

		dragBitmap->RemoveChild( &view );
		dragBitmap->Unlock();

		BMessage	drag_message('remp');
		gSong->Lock();
		drag_message.AddInt32("nr", gSong->PlayListEntry(nr)+1 );
		gSong->Unlock();
		drag_message.AddMessenger("be:sender", BMessenger(this) );

		DragMessage(&drag_message, dragBitmap, B_OP_ALPHA, where - BPoint( 3.0 + 17.0 * (nr % 16), 3.0 + 20.0 * (int)(nr / 16 ) ) );

		fDragPosition = nr;
		fDragging = true;
	}
}

void PlayListField::MouseDown(BPoint where) {
	
	SetMouseEventMask(B_POINTER_EVENTS, B_SUSPEND_VIEW_FOCUS|B_LOCK_WINDOW_FOCUS );
	fMousePressed = (FindButtonAt(where)!=-1);
	fMouseMoved = false;

};

void PlayListField::MouseUp(BPoint where) {

	if (!fMouseMoved) {
		int32	newPosition = FindButtonAt(where);
		gSong->Lock();
		if (newPosition!=-1 && newPosition!=gSong->ActiveNr()) {
			gSong->SetActiveNr(newPosition);
			if (gSong->IsPlaying() && gSong->PlayMode()==PLAY_SONG) {
				gSong->SetCursorPosition((gSong->PlayListEntry(newPosition)<<8) + (gSong->CursorPosition() & 255) );
			}
		}
		gSong->Unlock();
	}
	
	fMouseBeforeNr = -1;

	fMousePressed = false;
	fDragPosition = -1;
	fDragging = false;
	
	Update();

};

int32 PlayListField::FindButtonAt( BPoint where ) {

	int32 of_x = ((int)(where.x - 4.0)) % 17;
	int32 of_y = ((int)(where.y - 4.0)) % 20;
	
	int32 x = ((int)(where.x - 4.0)) / 17;
	int32 y = ((int)(where.y - 4.0)) / 20;
	
	int32 nr = y * 16 + x;
	gSong->Lock();
	if ( nr>gSong->LastFilledNr() || y < 0 || y > 3 || of_y>18 || x<0 || x>=16 || of_x>16 ) nr = -1;
	gSong->Unlock();
	return nr;
}

bool PlayListField::UpdateMousePosition( BPoint where, bool limit ) {

	int32 mouseBeforeNr = -1;
	
	if ( Bounds().InsetByCopy( 0.0, 3.0).Contains(where) ) {

		int32 x = ((int)(where.x)) / 17;
		int32 y = ((int)(where.y - 4.0)) / 20;

		mouseBeforeNr = x + 16 * y;
		gSong->Lock();
		if (gSong->LastFilledNr()>=63 || mouseBeforeNr<0 || mouseBeforeNr>63)
			mouseBeforeNr = -1;
		gSong->Unlock();
	}

	if (fMouseBeforeNr==mouseBeforeNr ) return false;
	
	fMouseBeforeNr = mouseBeforeNr;

	return true;

}

void PlayListField::MessageReceived( BMessage *message ) {
	
	switch (message->what) {
		case 'upda': {
			Update();
		}
		default:
			BControl::MessageReceived( message );

	}

}

void PlayListField::Update( bool drawAll ) {

	static int32 lastMouseBefore = -1;
	static int32 lastActive = -1;
	static int32 lastLastFilledNr = -1;
	
	gSong->Lock();
	int32 lastFilledNr = gSong->LastFilledNr();
	gSong->Unlock();
	
	if (lastLastFilledNr!=lastFilledNr) drawAll = true;

	gSong->Lock();
	int32	activeNr = gSong->ActiveNr();
	gSong->Unlock();
	
	if( !drawAll
		&& lastActive==activeNr
		&& lastMouseBefore==fMouseBeforeNr ) {
		Draw( Bounds() ); return;
	}

	fBgBitmap->Lock();
	
	if (drawAll) {

		BRect bounds = Bounds();
		
		fBgView->SetHighColor( B_TRANSPARENT_COLOR );
		fBgView->FillRect( bounds );
		
		fBgView->SetDrawingMode( B_OP_ALPHA );
	
		fBgView->SetHighColor( 0, 0, 0 );
		fBgView->StrokeRoundRect( bounds, 2.0, 2.0 );
		
		bounds.InsetBy( 1.0, 1.0 );
		fBgView->SetHighColor( 255, 255, 255 );
		fBgView->FillRect( bounds );
	
		fBgView->SetHighColor( 195, 195, 195 );
		fBgView->StrokeLine( bounds.RightTop(), bounds.LeftTop() );
		fBgView->StrokeLine( bounds.LeftBottom() );
	
		bounds.InsetBy( 1.0, 1.0 );
		fBgView->SetHighColor( 225, 225, 225 );
		fBgView->StrokeLine( bounds.RightTop(), bounds.LeftTop() );
		fBgView->StrokeLine( bounds.LeftBottom() );
	}

	if (lastMouseBefore!=fMouseBeforeNr) {
		fBgView->SetHighColor( 255, 255, 255 );
		int x = (int)(lastMouseBefore % 16 * 17.0 + 3.0);
		int y = (int)((int)(lastMouseBefore / 16) * 20.0 + 4.0);
		fBgView->FillRect( BRect(x, y, x + 1.0, y + 18.0 ) );
	}

	fBgView->SetHighColor( 150, 150, 150);
	fBgView->SetLowColor( B_TRANSPARENT_COLOR );

	for (int x = 0; x<16; ++x) {
		for (int y = 0; y<4; ++y) {

			int32 nr = y * 16 + x;

			if (	drawAll
				||	(activeNr!=lastActive && (nr==lastActive || nr==activeNr) )
				||	(fMouseBeforeNr!=lastMouseBefore && (nr==lastMouseBefore) )
				)
			{
				gSong->Lock();
				fBgView->DrawBitmap( fNrBitmap[gSong->PlayListEntry(nr)+1][(int)(nr==gSong->ActiveNr()||nr==gSong->LastFilledNr()+1)],
					BPoint( x * 17.0 + 4.0, y * 20.0 + 4.0) );
				gSong->Unlock();
			}
		}

	}
	
	lastMouseBefore = fMouseBeforeNr;

	gSong->Lock();
	lastLastFilledNr = gSong->LastFilledNr();
	lastActive = gSong->ActiveNr();
	gSong->Unlock();

	fBgView->Flush();
	fBgBitmap->Unlock();	

	DrawBitmap( fBgBitmap, BPoint( 0.0, 0.0 ) );
	
}

void PlayListField::Draw(BRect updateRect) {

	DrawBitmap( fBgBitmap, BPoint( 0.0, 0.0 ) );
}

void PlayListField::FullUpdate() {
	Update(true);
}

void PlayListField::PlaySong() {
	StartMessageRunner();
}

void PlayListField::StartMessageRunner() {
	delete fMessageRunner;
	fMessageRunner = new BMessageRunner( (BView *)this, new BMessage('upda'), 50000 );
}

void PlayListField::Stop() {
	delete fMessageRunner;
	fMessageRunner = 0;
	Update();
}

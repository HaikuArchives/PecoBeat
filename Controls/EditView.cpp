/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#include "EditView.h"

#include <Message.h>
#include <Window.h>

#include <iostream>
	
#include "App.h"
#include "PatternDefs.h"
#include "Song.h"
#include "PatternRowView.h"
#include "PatternWindow.h"

/* -------------------- EditView ------------------------------------ */

EditView::EditView( BRect rect )
:	BControl( rect, "EditView", NULL, new BMessage('setn'), B_FOLLOW_LEFT_RIGHT, B_WILL_DRAW|B_FRAME_EVENTS),
	fBgBufferBitmap(0),
	fBgBuffer(0),
	fDrawBufferBitmap(0),
	fDrawBuffer(0)
{
	Init();
}

void EditView::Init() {

	fRemovedFromCol	= -1;
	fMouseButtons	= 0;
	
	CreateBuffer();
	
}

void EditView::SaveToMessage( BMessage &msg ) const {

	for (int i=0; i<256; ++i) msg.AddInt8( "note", fBgBuffer->Note( i ) );
	
};

void EditView::LoadFromMessage(BMessage &msg) 
{
	for (int i=0; i<256; ++i) {
		uint8	note = 0;
		msg.FindInt8( "note", i, (int8 *)&note );
		SetNote( i, note, true);
	}
	Draw( true );
}

void EditView::CreateBuffer() {

	BRect rect( 0.0, 0.0, Bounds().right - 2.0, Bounds().bottom - 2.0 );

	// Buffer 1
	fBgBuffer			= new EditViewBgBuffer( rect );
	fBgBufferBitmap 	= new BBitmap( rect, B_RGB32, true);
	fBgBufferBitmap->AddChild( fBgBuffer );

	// Buffer 2
	fDrawBuffer			= new BView( rect, NULL, B_FOLLOW_ALL, B_WILL_DRAW);
	fDrawBufferBitmap 	= new BBitmap( rect, B_RGB32, true);
	fDrawBuffer->SetDrawingMode(B_OP_ALPHA);
	fDrawBufferBitmap->AddChild( fDrawBuffer );

}

void EditView::DeleteBuffer() {

	// Buffer 1
	fBgBufferBitmap->Lock();
	fBgBufferBitmap->RemoveChild( fBgBuffer );
	
	delete fBgBufferBitmap;
	fBgBufferBitmap = 0;

	delete fBgBuffer;
	fBgBuffer = 0;

	// Buffer 2
	fDrawBufferBitmap->Lock();
	fDrawBufferBitmap->RemoveChild( fDrawBuffer );

	delete fDrawBufferBitmap;
	fDrawBufferBitmap = 0;
	
	delete fDrawBuffer;
	fDrawBuffer = 0;

}

void EditView::AttachedToWindow() {

	fWindow = (PatternWindow *)Window();
	SetViewColor( B_TRANSPARENT_COLOR );

	SetTarget( Parent() );

}

void EditView::FrameResized(float new_width, float new_height) {
	
	uint8	notes[256];

	fBgBufferBitmap->Lock();
	for (int i=0; i<256; ++i) notes[i] = Note( i );
	fBgBufferBitmap->Unlock();
	DeleteBuffer();
	
	CreateBuffer();
	for (int i=0; i<256; ++i) SetNote( i, notes[i], true);

	Draw( true );

}

void EditView::Draw(BRect updateRect) {

	fDrawBufferBitmap->Lock();
	fDrawBuffer->DrawBitmap( fBgBufferBitmap, BPoint(0.0, 0.0) );
	
	if ( fMouseButtons) {

		BPoint	where;
		uint32	buttons;
		
		GetMouse( &where, &buttons );
		CutCoordinate( where );

		char*	imgname = 0;
		switch (fWindow->DrawMode( fMouseButtons )) {
			case NOTE_ON:		imgname = "note_on"; break;
			case NOTE_OFF:		imgname = "note_off"; break;
			case NOTE_RUBBER:	imgname = "rubber"; break;
		}
		BBitmap	*bmp = ((App *)be_app)->FetchResourceBitmap(imgname);

		float	step = (Bounds().right - 1) / 256; // Wie weit sind Noten minimal auseinander?
		fDrawBuffer->DrawBitmapAsync (bmp, BPoint(step * GetXCoordinate(where.x), 2.0));
		
	}

	fDrawBuffer->Sync();
	DrawBitmap( fDrawBufferBitmap, BPoint(1.0, 1.0) );
	fDrawBufferBitmap->Unlock();

	StrokeRoundRect(Bounds(), 2.0, 2.0 );

}

void EditView::SetNote(uint8 note, uint8 mode, bool quiet) {
	fBgBufferBitmap->Lock();
	fBgBuffer->SetNote( note, mode, quiet);
	fBgBufferBitmap->Unlock();
}

uint8 EditView::Note(uint8 note) {
	fBgBufferBitmap->Lock();
	uint8 value = fBgBuffer->Note( note );
	fBgBufferBitmap->Unlock();
	return value;
}

void EditView::CutCoordinate( BPoint &where ) {
	if (where.x<1) where.x = 1;
	else if (where.x>(Bounds().right-1)) where.x = Bounds().right-1;
}

inline uint8 EditView::GetXCoordinate( float x ) {
	const float oneStep = (Bounds().right - 1.0) / 256;
	const int divider = 4;
	return int( int((x - 4) / (oneStep * divider)) * divider );
}

void EditView::PlaySound( bool note_on ) {
	BMessage	msg	('psnd');
	msg.AddBool("be:value", note_on);
	Invoke( &msg );
}

void EditView::UpdatePatternRow(int tonNr, int drawMode ) {

	gSong->Lock();
	PatternRow	*row = (PatternRow *)gSong->fPattern->ItemAt( ((PatternRowView *)Parent())->RowIndex() );
	row->SetNote( (gSong->fPattern->ActivePart()<<8)|tonNr, drawMode );
	gSong->Unlock();

}

void EditView::MouseDown(BPoint where) {

	if (!IsEnabled()) return;
	
	GetMouse( &where, &fMouseButtons );

	CutCoordinate( where );
	int tonNr = GetXCoordinate( where.x );
	
	int32	drawMode = fWindow->DrawMode( fMouseButtons );

	fRemovedFromCol = -1;
	
	fBgBufferBitmap->Lock();
	if (drawMode != NOTE_NONE ) {
		if (fBgBuffer->Note(tonNr) == drawMode) { // Note bewegen oder löschen
			SetNote(tonNr, NOTE_NONE);
			UpdatePatternRow( tonNr, NOTE_NONE );
			fRemovedFromCol = tonNr;
		}
		// Note neu setzen
		else {
			PlaySound( drawMode == NOTE_ON );
		}
	}
	else {
		SetNote(tonNr, drawMode);
		UpdatePatternRow( tonNr, drawMode );
	}

	Draw();
	fBgBufferBitmap->Unlock();

	SetMouseEventMask( B_POINTER_EVENTS );

}

void EditView::MouseUp(BPoint where) {

	if (!IsEnabled() || !fMouseButtons) return; 

	CutCoordinate( where );
	int tonNr = GetXCoordinate( where.x );
	
	int32	drawMode = fWindow->DrawMode( fMouseButtons );

	fBgBufferBitmap->Lock();

	if (tonNr!=fRemovedFromCol) { // löschen
		PlaySound( false );
		SetNote(tonNr, drawMode);
		UpdatePatternRow( tonNr, drawMode );
	}
	fMouseButtons = 0;

	Draw();
	fBgBufferBitmap->Unlock();

}

void EditView::MouseMoved( BPoint where, uint32 code, const BMessage *a_message) {

	if (!IsEnabled()) return;
	
	uint32	buttons;
	GetMouse( &where, &buttons);

	if (!buttons || !fMouseButtons) {
		if (fMouseButtons) MouseUp( where );
		return;
	}

	CutCoordinate(where);
	int tonNr = GetXCoordinate( where.x );

	if (fRemovedFromCol>=0 && tonNr!=fRemovedFromCol) fRemovedFromCol = -1;

	int drawMode = fWindow->DrawMode( fMouseButtons );
	
	fBgBufferBitmap->Lock();
	if (drawMode == NOTE_RUBBER) {
		SetNote(tonNr, drawMode);
		UpdatePatternRow( tonNr, drawMode );
		PlaySound( drawMode==NOTE_ON );
	}

	Draw();
	fBgBufferBitmap->Unlock();
}

void EditView::Draw( bool deep_draw ) {

	if ( deep_draw ) {
		fBgBufferBitmap->Lock();
		fBgBuffer->Draw();
		fBgBufferBitmap->Unlock();
	}
	Draw( Bounds() );
}

EditView::~EditView() {
	DeleteBuffer();
}

/* -------------------- EditViewBgBuffer ------------------------------------ */

EditViewBgBuffer::EditViewBgBuffer( BRect rect )
:	BView( rect, NULL, B_FOLLOW_NONE, B_WILL_DRAW)
{

	for (int i=0; i<256; ++i) fNotes[i] = NOTE_NONE;

}

void EditViewBgBuffer::AttachedToWindow() {

	Draw();

}

void EditViewBgBuffer::Draw() {

	SetDrawingMode(B_OP_COPY);

	// Edit Bereich
	BRect	bounds = Bounds();

	SetHighColor( 255, 255, 255 );
	FillRect(bounds);

	// Zeichne Zeilen
	SetHighColor( 225, 225, 225 );
	StrokeLine( BPoint(0.0, 1.0), BPoint(bounds.right, 1.0));
	StrokeLine( BPoint(1.0, 0.0), BPoint(1.0, bounds.bottom));

	SetHighColor( 195, 195, 195 );
	StrokeLine( BPoint(0.0, 0.0), BPoint(bounds.right, 0.0));
	StrokeLine( BPoint(0.0, 0.0), BPoint(0.0, bounds.bottom));

	// Zeichne Spalten
	const int	TAKTE = 8;
	const int	UNTERTEILUNG = 4;
	float		abstand = (bounds.right + 1.0 ) / ( TAKTE * UNTERTEILUNG );
	
	float		x_position;
	
	SetDrawingMode(B_OP_ALPHA);

	SetHighColor( 0, 0, 0, 255);
	for (int i=0; i<TAKTE*UNTERTEILUNG; ++i) {
		x_position = i * abstand - 1.0;

		if (!(i%TAKTE))				SetHighColor( 0, 0, 0, 175);
		else if (!((i<<1)%TAKTE))	SetHighColor( 0, 0, 0, 100);
		else						SetHighColor( 0, 0, 0, 30);

		StrokeLine( BPoint(x_position, 0.0), BPoint( x_position, bounds.bottom ));
	}
	
	// Hier die einzelnen Noten zeichnen
	float			step = (Bounds().right + 1) / 256; // Wie weit sind Noten minimal auseinander?

	BBitmap			*note_on = ((App *)be_app)->FetchResourceBitmap("note_on");
	BBitmap			*note_off = ((App *)be_app)->FetchResourceBitmap("note_off");

	for (int i=0; i<256; i += 4) {
		switch ( fNotes[i] ) {
			case NOTE_ON:
				DrawBitmapAsync (note_on, BPoint(step * i, 2.0)); break;
			case NOTE_OFF:
				DrawBitmapAsync (note_off, BPoint(step * i, 2.0)); break;
		}
	}
	Sync();
}

inline void EditViewBgBuffer::SetNote(uint8 note, uint8 mode, bool quiet) {
	fNotes[note] = mode;
	if (!quiet) Draw();
}

inline uint8 EditViewBgBuffer::Note(uint8 note) {
	return fNotes[note];
}

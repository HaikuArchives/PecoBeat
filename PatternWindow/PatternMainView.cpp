/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#include "PatternMainView.h"

#include <Alert.h>
#include <Bitmap.h>
#include <MessageRunner.h>

#include <iostream>

#include "App.h"
#include "DrumsetDropField.h"
#include "PatternToolBar.h"
#include "PatternRowView.h"
#include "Skala.h"
#include "Song.h"

PatternMainView::PatternMainView( BRect frame )
:	BView( frame, "main", B_FOLLOW_ALL, B_WILL_DRAW|B_DRAW_ON_CHILDREN|B_FRAME_EVENTS ),
	fSkala( 0 ),
	fDragWindow( 0 ),
	fRemovedRow( 0 ),
	fMessageRunner( 0 )
{
}

PatternMainView::~PatternMainView() {
	delete fMessageRunner;
	delete fSkala;
	delete fRemovedRow;
}

void PatternMainView::UpdateRowViews() {

	for (int i=0; i<gSong->fPattern->CountItems(); ++i) {
		BMessage msg;

		gSong->Lock();
		((PatternRow *)gSong->fPattern->ItemAt( i ))->SaveActivePartToMessage( msg );
		gSong->Unlock();
		
		Window()->Lock();
		Window()->DisableUpdates();
		((PatternRowView *)fInstrumentList.ItemAt( i ))->LoadFromMessage( msg );
		Window()->EnableUpdates();
		Window()->Unlock();
	}
}

void PatternMainView::AttachedToWindow() {

	// Background Pattern
	SetViewBitmap(((App *)be_app)->fHGPattern);

	SetDrawingMode(B_OP_ALPHA);
	
	AddChild( new DrumsetDropField( BRect(0.0, 0.0, 97.0, 17.0).OffsetToCopy(16.0, Bounds().bottom - 22.0 ), 
		NULL, new BMessage('drop'),  B_FOLLOW_BOTTOM, B_WILL_DRAW) );

}

void PatternMainView::MouseMoved(BPoint pt, uint32 code, const BMessage *msg) {

	// Wenn wir keine gezogene Reihe haben, wird MouseMoved nicht benötigt
	if (!fDragWindow ) { BView::MouseMoved( pt, code, msg ); return; }
	
	// point enthält die Koordinate, an die das DragWindow gezogen wird
	pt.x = Bounds().Contains( pt ) ? 2.0 : pt.x;

	if (Bounds().Contains(pt)) {
		
		// Hier finden wir heraus, über welcher Row wir uns gerade befinden
		int32			current_index = -1;

		if ( fInstrumentList.CountItems() ) {
	
			// Wenn wir uns ausserhalb dieses Bereichs befinden, wird die neue Row entweder oben oder unten hinzugefügt
			BRect	dragRange (	0,
								((BView *)fInstrumentList.FirstItem())->Frame().top + (fDragWindow->Bounds().IntegerHeight() >> 1) + 1,
								Bounds().Width(),
								((BView *)fInstrumentList.LastItem())->Frame().bottom - (fDragWindow->Bounds().IntegerHeight() >> 1) - 1);
			
			if ( !dragRange.Contains( pt ) ) {
				current_index = (pt.y<dragRange.top) ? 0 : -1;
			}
			else {
				for (int i=0; i<fInstrumentList.CountItems(); ++i) {
					if ( ((BView *)fInstrumentList.ItemAt(i))->Frame().Contains( pt ) ) {
						current_index = i; break;
					}
				}
			}
		}

		// Row wird wieder in Fenster gezogen
		if ( fRemovedRow ) {
			
			int32 index = AddPatternRowToSong( *fRemovedRow, current_index );
			AddPatternRowView( index );

			fRemovedRow = 0;
			fRowOverIndex = current_index == -1 ? fInstrumentList.CountItems() - 1 : current_index;
			
			// Als zu verfolgende Zeile Markieren
			fFollowingRow = (PatternRowView	*)fInstrumentList.ItemAt( fRowOverIndex );

		}
			
		// Verschieben
		else if (current_index!=fRowOverIndex && current_index!=-1 && fRowOverIndex!=-1 && !fFollowingRow->Frame().Contains( pt )) {

			float	y_offset = fDragWindow->Bounds().Height();
			if (current_index < fRowOverIndex) {

				BPoint new_pos( ((PatternRowView *)fInstrumentList.ItemAt( current_index ))->Frame().LeftTop() );
				for (int i=current_index; i<fRowOverIndex; ++i) {
					((BView *)fInstrumentList.ItemAt(i))->MoveBy(0.0, y_offset);
				}
				fFollowingRow->MoveTo( new_pos );
			}
			else {

				BPoint new_pos( ((PatternRowView *)fInstrumentList.ItemAt( current_index ))->Frame().LeftBottom() - BPoint( 0.0, fFollowingRow->Frame().Height() ) );
				for (int i=fRowOverIndex+1; i<=current_index; ++i) {
					((BView *)fInstrumentList.ItemAt(i))->MoveBy(0.0, -y_offset);
				}
				fFollowingRow->MoveTo( new_pos );
			}
			
			fInstrumentList.MoveItem(fRowOverIndex, current_index);
			
			// Weiter geben an Pattern
			gSong->Lock();
			gSong->fPattern->MovePatternRow(fRowOverIndex, current_index);
			gSong->Unlock();
			
			fRowOverIndex = current_index;
		}
	}
	
	// Row aus dem Fenster ziehen
	else if ( !fRemovedRow ) {

		fRemovedRow = new BMessage();
		gSong->Lock();
		((PatternRow *)gSong->fPattern->ItemAt( fRowOverIndex ))->SaveToMessage( *fRemovedRow );
		gSong->Unlock();

		// löschen
		int32	index = fInstrumentList.IndexOf( fFollowingRow );

		gSong->Lock();
		gSong->fPattern->DeletePatternRow( index );
		gSong->Unlock();
		RemovePatternRowView( index );

		pt.x -= 8.0;
	}
	pt.y -= fDragWindow->Bounds().Height() / 2;
	fDragWindow->MoveTo( ConvertToScreen(pt));

}

void PatternMainView::MouseUp(BPoint where) {

	// DragWindow schließen, evtl. Sicherheitsabfrage und Undo
	if (fDragWindow) {
		
		fDragWindow->SetFeel(B_NORMAL_WINDOW_FEEL);
		if ( fRemovedRow && (new BAlert(B_EMPTY_STRING, "Really delete that track?", "Sure.", "Ooops!"))->Go() ) {

			int32 index = AddPatternRowToSong( *fRemovedRow, fRowOverIndex );
			AddPatternRowView( index );

		}
		
		DeleteDragWindow();

		delete fRemovedRow;
		fRemovedRow = 0;
	
	}
	
	BView::MouseUp( where );
	
}

void PatternMainView::DragRow( PatternRowView *row )
{

	BPoint	where;
	uint32	buttons;
		
	GetMouse( &where, &buttons );

	BRect			rect = row->ConvertToScreen(row->Bounds());
	
	PatternRowView	*rowCopy = new PatternRowView( *row, true );

	// diese Variable gibt an, über welcher Row sich die gezogene View momentan aufhält
	fRowOverIndex			= fInstrumentList.IndexOf( row );
	
	// fFollowingRow ist ein Zeiger auf die View, deren Kopie gezogen wird
	fFollowingRow			= row;

	DeleteDragWindow();
	fDragWindow		= new BWindow( rect, "Dragged Row", B_NO_BORDER_WINDOW_LOOK,
		B_FLOATING_ALL_WINDOW_FEEL, B_AVOID_FOCUS);
	
	fDragWindow->AddChild( rowCopy );
	fDragWindow->Show();
	
	SetMouseEventMask( B_POINTER_EVENTS, B_LOCK_WINDOW_FOCUS );

}

void PatternMainView::DeleteDragWindow() {

	if (!fDragWindow) return;
	
	// Move to new position
	if (!fRemovedRow) {
		int32	steps = 10;
		PatternRowView	*orig_row =
			(PatternRowView *)fInstrumentList.ItemAt( fRowOverIndex );
		BPoint	orig_pos	= ConvertToScreen( orig_row->Frame().LeftTop() );
		
		for (int i=steps; i>0; --i) {
			snooze( 2000 );
			fDragWindow->MoveTo( 
				orig_pos.x + (fDragWindow->Frame().left - orig_pos.x) / steps * i,
				orig_pos.y + (fDragWindow->Frame().top - orig_pos.y) / steps * i
			);
			Window()->UpdateIfNeeded();
		}
	}
	
	fDragWindow->Lock();
	fDragWindow->Quit();
	fDragWindow = 0;
	
}

void PatternMainView::Draw(BRect updateRect) {
	
	SetLowColor( 55, 96, 139 );
	SetHighColor( 255, 255, 255 );
	
	BFont	font(be_plain_font);
	font.SetFamilyAndStyle("Swis721 BT","Roman");
	font.SetSize( 10.0 );
	SetFont(&font);
	
	// labels/images
	DrawString("instrument", BPoint(21.0, 12.0));

	BBitmap *bmp = ((App *)be_app)->FetchResourceBitmap("velocity");
	DrawBitmap(	bmp, BPoint(121.0, 4.0) );
	delete bmp;
	
	// Dummy row at the bottom
	SetHighColor( 0, 0, 0, 50 );
	FillArc( BRect(0.0, 0.0, 16.0, 16.0).OffsetByCopy(4.0, Bounds().bottom - 21.0), 90, 180);
	FillRect( BRect(0.0, 0.0, 0.0, 16.0).OffsetByCopy(13.0, Bounds().bottom - 21.0) );
	FillEllipse( BRect(0.0, 0.0, 17.0, 17.0).OffsetByCopy(117.0, Bounds().bottom - 22.0));
	FillRoundRect( BRect(137.0, Bounds().bottom - 21.0, Bounds().right - 5.0, Bounds().bottom - 5.0), 2.0, 2.0);

	StrokeArc(BRect(0.0, 0.0, 16.0, 16.0).OffsetByCopy(4.0, Bounds().bottom - 21.0), 85, 200);
	StrokeLine( BPoint(13.0, Bounds().bottom - 20.0), BPoint( 13.0, Bounds().bottom - 6.0 ));
	StrokeEllipse( BRect(0.0, 0.0, 17.0, 17.0).OffsetByCopy(117.0, Bounds().bottom - 22.0));
	StrokeRoundRect( BRect(137.0, Bounds().bottom - 21.0, Bounds().right - 5.0, Bounds().bottom - 5.0), 2.0, 2.0);

};

void PatternMainView::UpdateChildren() {

	if (fSkala) fSkala->Draw(fSkala->Bounds());
	
	if (gSong->PlayMode()==PLAY_SONG) {

		BPoint	where;
		uint32	buttons;
		GetMouse( &where, &buttons );

		where = ConvertToScreen(where);
		BView *patternNrButton = Window()->FindView("patternnrbutton");

		if (!buttons || !patternNrButton->ConvertToScreen(patternNrButton->Bounds()).Contains(where)) {
			BControl *control = ((BControl *)Window()->FindView("patternnrbutton"));
			int32 newPart = gSong->ActivePlayListEntry() + 1;
			if (control->Value() != newPart) control->SetValue( newPart );
		}
	}
}			

int32 PatternMainView::AddPatternRowToSong( BMessage &msg, int32 index ) {
	
	// Daten vervollständigen
	msg.what = 'newr';

	if (!msg.HasInt8("velocity"))	msg.AddInt8("velocity", 100 );
	if (!msg.HasInt32("index"))	msg.AddInt32("index", index );

	// an Pattern schicken
	gSong->Lock();
	index = gSong->fPattern->AddPatternRow( msg, index );
	gSong->Unlock();

	return index;
}

void PatternMainView::RemovePatternRowFromSong( int32 index ) {
	
	gSong->Lock();
	gSong->fPattern->DeletePatternRow( index );
	gSong->Unlock();

}


void PatternMainView::AddPatternRowView( int32 index ) {

	// Die entsprechende Row muss im Song bereits vorhanden sein!!!!
	PatternRowView *row = new PatternRowView();

	BMessage msg;
	gSong->Lock();
	((PatternRow *)gSong->fPattern->ItemAt( index ))->SaveActivePartToMessage( msg );
	gSong->Unlock();
	row->LoadFromMessage( msg );
	
	// index bestimmen
	int32	item_count = fInstrumentList.CountItems();

	// Toolbar aktivieren und Skala hinzufügen, falls notwendig
	if (fInstrumentList.CountItems()==0) {
		((PatternToolBar *)Window()->FindView("toolbar"))->SetEnabled( true );
		AddSkala();
	}

	// offset bestimmen und positionieren
	float	y_offset = index==0 ? 14.0 :
		((PatternRowView *)fInstrumentList.ItemAt(index - 1))->Frame().bottom;
	row->MoveTo(2.0, y_offset);

	// nachfolgende rows verschieben
	y_offset = row->Bounds().Height();

	for (int i = index; i<item_count; ++i ) {
		((PatternRowView *)fInstrumentList.ItemAt( i ))->MoveBy( 0.0, y_offset );
	}

	// In Liste und Fenster einfügen
	if (index==item_count)	fInstrumentList.AddItem( row );
	else					fInstrumentList.AddItem( row, index );
	AddChild( row );
	
	// Fenstergröße anpassen und neu zeichnen
	Window()->ResizeBy(0.0, row->Bounds().Height() );
	Invalidate();

}

void PatternMainView::RemovePatternRowView( int32 index ) {

	int32	item_count	= fInstrumentList.CountItems();
	if (item_count<=index) return;

	// nachfolgende rows verschieben
	PatternRowView	*row = (PatternRowView *)fInstrumentList.ItemAt( index );
	PatternRowView	*temp_row;
	float			y_offset = row->Frame().Height();
	
	for (int i = index + 1; i<item_count; ++i ) {
		temp_row = (PatternRowView *)fInstrumentList.ItemAt( i );
		temp_row->MoveBy( 0.0, -y_offset );
		y_offset = temp_row->Frame().Height();
	}
		
	// row entfernen
	fInstrumentList.RemoveItem( row );

	if (item_count==1) {
		((PatternToolBar *)Window()->FindView("toolbar"))->SetEnabled( false );
		RemoveSkala();
	}
	
	// neu zeichnen
	RemoveChild( row );
	Window()->ResizeBy(0.0, -row->Bounds().Height() );
	Invalidate();
	
}

void PatternMainView::AddSkala() {
	fSkala = new Skala( BRect(137.0, 7.0, Bounds().right - 5.0, 14.0) );
	AddChild( fSkala );
}

void PatternMainView::RemoveSkala() {
	RemoveChild( fSkala );
	fSkala = 0;
}

void PatternMainView::MessageReceived( BMessage *msg ) {

	switch (msg->what) {
		case 'drop': {
			int32 key;
			if ( msg->FindInt32( "be:value", &key ) == B_OK ) {
				msg->AddInt8("key", key );
				int32 index = AddPatternRowToSong( *msg, -1 );
				AddPatternRowView( index );
			}
			
		} break;

		case 'drgr': {
			PatternRowView	*row;
			if (msg->FindPointer("instrumentRow", (void **)&row ) == B_OK
				&& fInstrumentList.HasItem( row ))
					DragRow( row );
			
		} break;
		
		case 'upda': {

			UpdateChildren();

		} break;

		default:
			BView::MessageReceived( msg );
	}
		
}

void PatternMainView::FullUpdate() {

	Stop();
	while (fInstrumentList.CountItems()>0) {
		PatternRowView	*row = (PatternRowView *)fInstrumentList.ItemAt(0);
		RemovePatternRowView(0);
		delete row;
	}
	
	for (int i=0; i<gSong->fPattern->CountItems(); ++i) {
		AddPatternRowView( i );
	}
	
	UpdateRowViews();
}

void PatternMainView::PlaySong() {
	gSong->Lock();
	gSong->SetActivePart(gSong->ActivePlayListEntry());
	gSong->Unlock();
	StartMessageRunner();
}

void PatternMainView::PlayPattern() {
	StartMessageRunner();
}

void PatternMainView::StartMessageRunner() {
	delete fMessageRunner;
	fMessageRunner = new BMessageRunner( (BView *)this, new BMessage('upda'), 2500 );
}

void PatternMainView::Stop() {
	delete fMessageRunner;
	fMessageRunner = 0;

	UpdateChildren();
}

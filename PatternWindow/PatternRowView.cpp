/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#include "PatternRowView.h"

#include <Window.h>

#include <iostream>

#include "App.h"
#include "DrumsetButton.h"
#include "EditView.h"
#include "Knob.h"
#include "MIDIProducer.h"
#include "PatternDefs.h"
#include "PatternMainView.h"
#include "SideGrip.h"
#include "Song.h"

#define VIEW BView( BRect( 0.0, 0.0, 500.0, 19.0), "pattern row view", B_FOLLOW_LEFT_RIGHT, B_WILL_DRAW|B_DRAW_ON_CHILDREN)

PatternRowView::PatternRowView( int key, int velocity )
:	VIEW
{

	Init();

	// Voreinstellungen
	fDrumsetButton->SetKey( key );
	fKnob->SetValue( velocity );
}

PatternRowView::PatternRowView( const PatternRowView& copy, bool controlsDisabled )
:	VIEW
{

	Init( controlsDisabled );
	
	BMessage message;
	copy.SaveToMessage( message );
	LoadFromMessage( message );
	
}

void PatternRowView::Init( bool controlsDisabled ) {

	// SideGrip
	BRect rect		= BRect(0.0, 0.0, 12.0, 18.0).OffsetToCopy(0.0, 1.0);
	fSideGrip		= new SideGrip(rect, new BMessage('drgr'));
	AddChild(fSideGrip);

	// DrumsetButton
	rect 			= kDrumsetButtonBounds;
	rect.OffsetTo(14.0, 1.0);
	fDrumsetButton	= new DrumsetButton(rect, 0, new BMessage('psnd'), false, true);
	AddChild(fDrumsetButton);

	// Knob
	rect = BRect(0.0, 0.0, 17.0, 17.0).OffsetToCopy(115.0, 1.0);
	fKnob		= new Knob( rect, NULL, new BMessage('setv'), 0, B_WILL_DRAW);
	AddChild(fKnob);

	// EditView
	rect		= BRect(135.0, 0.0, Bounds().right, Bounds().bottom);
	fEditView	= new EditView( rect );
	AddChild(fEditView);

	if (controlsDisabled) {
		fSideGrip->SetEnabled( false );
		fDrumsetButton->SetEnabled( false );
		fKnob->SetEnabled( false );
		fEditView->SetEnabled( false );
	}

}

void PatternRowView::LoadFromMessage( BMessage &msg )
{
	
	int8	value;
	
	if (msg.FindInt8("key", &value) == B_OK ) fDrumsetButton->SetKey( value );
	if (msg.FindInt8("velocity", &value) == B_OK ) fKnob->SetValue( value );

	fEditView->LoadFromMessage( msg );

}

void PatternRowView::SaveToMessage( BMessage &msg ) const
{

	msg.AddInt8("key", fDrumsetButton->Key() );
	msg.AddInt8("velocity", fKnob->Value() );
	fEditView->SaveToMessage( msg );
	
}

void PatternRowView::AttachedToWindow() {

	if (Parent()) ResizeTo( Parent()->Bounds().right - 7.0, 19.0 );
	else ResizeTo( Window()->Bounds().Width(), Window()->Bounds().Height() );

	// Background Pattern
	SetViewBitmap(((App *)be_app)->fHGPattern);

	BView::AttachedToWindow();

}

void PatternRowView::MessageReceived(BMessage *msg) {

	int32		row = RowIndex();

	gSong->Lock();
	PatternRow	*patternRow = (PatternRow *)gSong->fPattern->ItemAt( row );
	gSong->Unlock();
			
	// Infos an Pattern senden
	switch( msg->what ) {

		case 'setk': {
			int8	key;
			msg->FindInt8( "key", &key );
			patternRow->SetKey( key );
		} break;
		
		case 'setv': {
			int32	value;
			msg->FindInt32( "be:value", &value );
			patternRow->SetVelocity( value );
		} break;

		case 'psnd': 
		break;
		
		default:
			BView::MessageReceived( msg );
			return;
	}

	// Ton spielen
	switch( msg->what ) {
		case 'setv':
		case 'psnd': 
			
			int8 key;
			bool note_on;
		
			if (msg->FindInt8("key", &key)!=B_OK)
				key = fDrumsetButton->Key();
		
			msg->FindBool("be:value", &note_on);
			
			// Ton spielen
			int	velocity = fKnob->Value();
			gMidiProducer->SprayNoteOff( 10, key, velocity );
			if (note_on) gMidiProducer->SprayNoteOn( 10, key, velocity );
			
		break;
	}

}

void PatternRowView::Draw(BRect updateRect) {

	BView::Draw(updateRect);

	if (!Parent()) {

		BRect	rect( Bounds() );
		SetDrawingMode(B_OP_ALPHA);

		SetHighColor( 255, 255, 255, 40);
		FillRect( rect );

		SetHighColor( 255, 255, 255, 100);
		StrokeLine( rect.RightTop(), rect.LeftTop() );

		SetHighColor( 255, 255, 255, 60);
		StrokeLine( rect.LeftBottom() );

		SetHighColor( 0, 0, 0, 150);
		StrokeLine( rect.RightBottom() );
	}
}

int32 PatternRowView::RowIndex() {
	return ((PatternMainView *)Parent())->fInstrumentList.IndexOf( this );
}


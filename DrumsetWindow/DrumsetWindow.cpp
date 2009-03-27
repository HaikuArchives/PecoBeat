/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */


#include "DrumsetWindow.h"

#include <Alert.h>
#include <iostream>
#include <MenuBar.h>
#include <MenuItem.h>
#include <MessageRunner.h>
#include <MidiRoster.h>
#include <MidiConsumer.h>
#include <Roster.h>
#include <Screen.h>

#include "App.h"
#include "DrumsetButton.h"
#include "DrumsetMainView.h"
#include "HelpMenu.h"
#include "MIDIProducer.h"
#include "Prefs.h"
#include "WindowsMenu.h"

#define INTERNALMIDI_SIGNATURE "application/x-vnd.pecora-internal_midi"
#define INTERNALMIDI_NAME "InternalMIDI"
#define NUMBER_OF_INSTRUMENTS 60

DrumsetWindow::DrumsetWindow( BRect frame )
	:	BWindow( frame, "Drumset", B_TITLED_WINDOW, B_WILL_ACCEPT_FIRST_CLICK|B_NOT_H_RESIZABLE|B_ASYNCHRONOUS_CONTROLS),
	fMessageRunner( 0 )
{

	// Read Prefs
	if (gPrefs.FindRect("drumsetwindow_frame", &frame)==B_OK) {
		MoveTo( frame.LeftTop() );
		ResizeTo( frame.Width(), frame.Height() );
	}
	
	BMenuBar	*menubar;
	BMenu		*menu;
	BMenuItem	*menuitem;
	
	menubar	= new BMenuBar( BRect(0,0,10000,0), "menubar", B_FOLLOW_TOP | B_FOLLOW_LEFT_RIGHT );
	menu	= new BMenu("File");
	menu->AddItem(new BMenuItem("Close", new BMessage(B_QUIT_REQUESTED), 'W'));
	menu->AddSeparatorItem();
	menuitem = new BMenuItem("Quit", new BMessage(B_QUIT_REQUESTED), 'Q');
	menuitem->SetTarget(be_app);
	menu->AddItem(menuitem);
	menubar->AddItem(menu);
	
	menu = new BMenu(INTERNALMIDI_NAME);
	menu->AddItem(new BMenuItem("Settings...", new BMessage('IMPr'), 'P'));
	menu->AddItem(new BMenuItem("Start InternalMIDI", new BMessage('IMSt'), 'I'));
	
	menubar->AddItem(menu);

	menu	= new WindowsMenu(2);
	menubar->AddItem(menu);

	menu	= new HelpMenu();
	menubar->AddItem(menu);

	AddChild( menubar );

	BView	*main_view = new DrumsetMainView( Bounds().OffsetByCopy(0, menubar->Bounds().bottom + 1) );
	AddChild( main_view );

	fWidthOffset = 11;
	fHeightOffset = 11 + (int)menubar->Bounds().bottom;
	
	SetWindowAlignment(B_PIXEL_ALIGNMENT,
		0, 0, kDrumsetButtonBounds.Width(), fWidthOffset,
		0, 0, kDrumsetButtonBounds.Height(), fHeightOffset % kDrumsetButtonBounds.IntegerHeight()
	);
	
	UpdateSize();

	fMessageRunner = new BMessageRunner( this, new BMessage('chkc'), 100000 );

	FindInternalMIDI();
}

DrumsetWindow::~DrumsetWindow() {

	delete fMessageRunner;
	
	gPrefs.RemoveName("drumsetwindow_frame");
	gPrefs.AddRect("drumsetwindow_frame", Frame());
}

void DrumsetWindow::MessageReceived(BMessage *message) {

	switch (message->what) {
		case 'psnd': {
			int8	key;
			message->FindInt8("key", &key);
			bool	note_on = 0;
			message->FindBool("be:value", &note_on);

			gMidiProducer->SprayNoteOff( 10, key, 100 );
			if (note_on) gMidiProducer->SprayNoteOn( 10, key, 100 );

		} break;

		case 'chkc': {
			FindInternalMIDI();
		} break;
		
		case B_ABOUT_REQUESTED: {
			be_app->PostMessage( message );
		} break;
		
		case 'Work': {
			message->AddInt32("workspaces", Workspaces());
			be_app->PostMessage(message);
		} break;
		
		case 'IMPr': {

			BMessage        msg('Sett');
	        BMessenger( INTERNALMIDI_SIGNATURE ).SendMessage( &msg );

		} break;
		
		case 'IMSt': {
			if ( !be_roster->IsRunning( INTERNALMIDI_SIGNATURE ) ) {
				if ( be_roster->Launch( INTERNALMIDI_SIGNATURE ) != B_OK ) {
					(new BAlert("", "FATAL ERROR: I wasn't able to find and start InternalMIDI. Is it not installed?", "Ooops...", NULL, NULL, B_WIDTH_AS_USUAL, B_STOP_ALERT))->Go();
				}
			}
		} break;

		case 'Help': {
			message->AddInt32("helppage", DOCU_DRUMSETWINDOW);
			be_app->PostMessage(message);
		} break;

		default:
			BWindow::MessageReceived(message);
	}
}

void DrumsetWindow::FindInternalMIDI() {

	bool connected = false;
	
	BMidiEndpoint	*endPoint;
	for (int i=0; i<gMidiProducer->Connections()->CountItems(); ++i) {
		endPoint = (BMidiEndpoint *)gMidiProducer->Connections()->ItemAt(i);
		if (endPoint && strcmp( endPoint->Name(), INTERNALMIDI_NAME)==0 && endPoint->IsValid()) {
			connected = true;
		}
	}
	if (connected) return;

	int32			id = 0;
	BMidiConsumer	*consumer = 0;
	while (	( consumer = BMidiRoster::NextConsumer( &id ) ) ) {
		if ( strcmp( consumer->Name(), INTERNALMIDI_NAME) == 0 && consumer->IsValid() ) {
			gMidiProducer->Connect( consumer );
		}
		else {
			consumer->Release();
		}
	}
}

int32 DrumsetWindow::HeightToRows( float height ) {
	return ((int)height - fHeightOffset + 1) / kDrumsetButtonBounds.IntegerHeight();
}

float DrumsetWindow::RowsToHeight( int32 rows ) {
	return kDrumsetButtonBounds.Height() * rows + fHeightOffset;
}

float DrumsetWindow::WidthForRows( int32 rows ) {
	
	int32	cols = NUMBER_OF_INSTRUMENTS / rows + (NUMBER_OF_INSTRUMENTS % rows ? 1 : 0);

	return kDrumsetButtonBounds.Width() * cols + fWidthOffset;
}

void DrumsetWindow::FrameResized(float new_width, float new_height) {
	UpdateSize();
}

void DrumsetWindow::UpdateSize( bool zoom ) {

	BScreen	screen(this);
	
	// Größe aktualisieren
	SetSizeLimits( 0.0, 10000.0, 0.0, 10000.0);
	int32 numRows = max_c(1, HeightToRows( Frame().Height() ) );

	if (zoom) ++numRows;

	while ( WidthForRows(numRows) > screen.Frame().Width()
		|| (RowsToHeight(numRows) > screen.Frame().Height())
		|| (zoom && (NUMBER_OF_INSTRUMENTS % numRows)) )
	{
		if (RowsToHeight(numRows) > screen.Frame().Height()) numRows = 1;
		else ++numRows;
	} 

	ResizeTo( WidthForRows(numRows), RowsToHeight(numRows) );
	
	// Größenlimit
	float width = screen.Frame().Width();

	int32 cols = max_c(1, (int)((width - fWidthOffset) / kDrumsetButtonBounds.IntegerWidth()));
	int32 rows = NUMBER_OF_INSTRUMENTS / cols + (NUMBER_OF_INSTRUMENTS % cols ? 1 : 0);
	float minHeight = RowsToHeight( rows );
	
	int32 maxRows = min_c( HeightToRows(screen.Frame().Height()-fHeightOffset), NUMBER_OF_INSTRUMENTS);
	float maxHeight = RowsToHeight(maxRows);
	
	SetSizeLimits( 0.0, 10000.0, minHeight, maxHeight);
	
	// Buttons aktualisieren
	BView	*main_view = FindView("main");
	BView	*child;
	DisableUpdates();
	for (int i=0; i<60; i++ ) {
		child = main_view->ChildAt(i);
		child->MoveTo( 	5 + int(i/numRows) * kDrumsetButtonBounds.Width(),
						5 + (i % numRows) * kDrumsetButtonBounds.Height() );
	}
	EnableUpdates();

};

void DrumsetWindow::Zoom( BPoint rec_position, float rec_width, float rec_height) {
	UpdateSize(true);
};

void DrumsetWindow::WorkspacesChanged(uint32 old_ws, uint32 new_ws) {
	UpdateSize();
};

void DrumsetWindow::ScreenChanged(BRect screen_size, color_space depth) {
	UpdateSize();
}

bool DrumsetWindow::QuitRequested() {
	if (modifiers() & B_SHIFT_KEY) {
		be_app->PostMessage(B_QUIT_REQUESTED);
	}
	else {
		be_app->Lock();
		if (((App *)be_app)->IsLastWindow()) {
			if (be_app->QuitRequested()) be_app->Quit();
		}
		else {
			Hide();
		}
		be_app->Unlock();
	}
	return false;
}

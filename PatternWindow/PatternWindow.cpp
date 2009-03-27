/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#include "PatternWindow.h"

#include <Alert.h>
#include <Cursor.h>
#include <Entry.h>
#include <File.h>
#include <Path.h>
#include <MenuBar.h>
#include <MenuItem.h>
#include <NodeInfo.h>
#include <Picture.h>
#include <Screen.h>

#include <iostream>

#include "App.h"
#include "Instruments.h"
#include "FileMenu.h"
#include "HelpMenu.h"
#include "PatternDefs.h"
#include "PatternMainView.h"
#include "PatternRowView.h"
#include "PatternToolBar.h"
#include "PlayButton.h"
#include "Prefs.h"
#include "Song.h"
#include "WindowsMenu.h"

PatternWindow::PatternWindow( BRect frame )
:	BWindow( frame, "Pattern", B_TITLED_WINDOW_LOOK, B_NORMAL_WINDOW_FEEL, B_NOT_RESIZABLE/*B_NOT_V_RESIZABLE|B_FULL_UPDATE_ON_RESIZE|B_FRAME_EVENTS|*/|B_NOT_ZOOMABLE|B_ASYNCHRONOUS_CONTROLS)
{

	// Read Prefs
	if (gPrefs.FindRect("patternwindow_frame", &frame)==B_OK) {
		MoveTo( frame.LeftTop() );
		ResizeTo( frame.Width(), Bounds().Height() );
	}
		
	// Menu
	BMenuBar	*menubar;
	BMenu		*menu;
	
	menubar	= new BMenuBar( BRect(0,0,10000,0), "menubar", B_FOLLOW_TOP | B_FOLLOW_LEFT_RIGHT );
	menu	= new FileMenu();
	menubar->AddItem(menu);
	
	menu	= new WindowsMenu(0);
	menubar->AddItem(menu);

	menu	= new HelpMenu();
	menubar->AddItem(menu);

	AddChild( menubar );

	ResizeBy(0, menubar->Bounds().bottom);

	// Tool bar
	fToolBar = new PatternToolBar( BRect(0.0, menubar->Bounds().bottom + 1.0, Bounds().right, menubar->Bounds().bottom + 28.0));
	AddChild( fToolBar );
	
	fMainView = new PatternMainView( BRect(0.0, menubar->Bounds().bottom + 28.0, Bounds().right, Bounds().bottom));
	AddChild( fMainView );
}

PatternWindow::~PatternWindow() {
	gPrefs.RemoveName("patternwindow_frame");
	gPrefs.AddRect("patternwindow_frame", Frame());
}

void PatternWindow::MessageReceived(BMessage *message) {

	switch( message->what ) {
		case B_KEY_UP:
		case B_KEY_DOWN: {
			int32	repeat;
			message->FindInt32("be:key_repeat", &repeat);
			if (repeat>0) break;

			int32	key;
			message->FindInt32("key", &key);
			
			if (key>=2 && key<=13 && message->what==B_KEY_DOWN) {
				((PlayButton *)FindView("patternnrbutton"))->SetValue( key - 1 );
			}
			else

				switch( key ) {
					case 71: // RETURN
					case 91: // ENTER
					{
						((PlayButton *)FindView("play"))->ChangeState( message->what == B_KEY_DOWN );
					} break;

				}
			
		} break;
		
		case 'mbut': {
			int32	mode, mousebutton;
			if( message->FindInt32("be:value", &mode ) == B_OK
				&& message->FindInt32("mousebutton", &mousebutton ) == B_OK) {
				switch( mousebutton ) {
					case B_PRIMARY_MOUSE_BUTTON: fDrawMode[0] = mode; break;
					case B_SECONDARY_MOUSE_BUTTON: fDrawMode[1] = mode; break;
					case B_TERTIARY_MOUSE_BUTTON: fDrawMode[2] = mode; break;
				}
			}
		} break;
		
		case 'pttr': { // Pattern löschen
			
			gSong->Lock();
			gSong->fPattern->ClearActivePart();
			gSong->Unlock();
			
			fMainView->UpdateRowViews();

		} break;
		
		case 'ptnr': { // Pattern aktivieren

			int32	newpart = 0;
			message->FindInt32("be:value", &newpart);
			newpart--;
			
			gSong->Lock();
			gSong->fPattern->SetActivePart( newpart );
			gSong->Unlock();
			
			fMainView->UpdateRowViews();
			
		} break;
		
		case 'drpt': { // Pattern kopiern

			int32	source = 0, destination = 0;
			message->FindInt32("source", &source);
			message->FindInt32("destination", &destination);

			if (source && destination && source != destination ) {
				gSong->Lock();
				gSong->fPattern->CopyPart( source-1, destination-1 );
				gSong->Unlock();
				((BControl *)fToolBar->FindView("patternnrbutton"))->SetValue( destination );
			}
		} break;
		
		case 'plys': {
			((PlayButton *)FindView("play"))->SetAsStopButton( false );
			((PatternMainView *)FindView("main"))->PlaySong();
		} break;
		
		case 'plyp': {
			((PlayButton *)FindView("play"))->SetAsStopButton( true );
			((PatternMainView *)FindView("main"))->PlayPattern();
		} break;
		
		case 'stop': {
			((PlayButton *)FindView("play"))->SetAsStopButton( false );
			((PatternMainView *)FindView("main"))->Stop();
		} break;
		
		case 'FUpd': {
			DisableUpdates();
			((BControl *)FindView("patternnrbutton"))->SetValue(1);
			((PlayButton *)FindView("play"))->SetAsStopButton( true );
			((PatternMainView *)FindView("main"))->FullUpdate();
			EnableUpdates();
		} break;
		
		case 'Work': {
			message->AddInt32("workspaces", Workspaces());
			be_app->PostMessage(message);
		} break;
		
		case 'Rcnt': {
			BMenuBar *menuBar = KeyMenuBar();
			BMenuItem *menuitem = menuBar->FindItem("File");
			FileMenu *menu = (FileMenu *)menuitem->Submenu();
			menu->UpdateRecentFiles();
		} break;
		
		case 'Help': {
			message->AddInt32("helppage", DOCU_PATTERNWINDOW);
			be_app->PostMessage(message);
		} break;

		default:
			BWindow::MessageReceived(message);
	}
}

bool PatternWindow::QuitRequested() {
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

int32 PatternWindow::DrawMode( int32 mousebuttons ) {
	
	if (mousebuttons & B_SECONDARY_MOUSE_BUTTON) 	return fDrawMode[1];
	if (mousebuttons & B_TERTIARY_MOUSE_BUTTON) 	return fDrawMode[2];

	return fDrawMode[0];

}

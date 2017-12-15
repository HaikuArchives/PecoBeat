/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#include "SongWindow.h"

#include <iostream>
#include <MenuBar.h>
#include <MenuItem.h>
#include <Screen.h>

#include "App.h"
#include "Instruments.h"
#include "FileMenu.h"
#include "HelpMenu.h"
#include "PlayButton.h"
#include "PlayListField.h"
#include "Prefs.h"
#include "Song.h"
#include "SongToolBar.h"
#include "WindowsMenu.h"

SongWindow::SongWindow( BRect frame )
:	BWindow( frame, "Song", B_TITLED_WINDOW_LOOK, B_NORMAL_WINDOW_FEEL, B_NOT_RESIZABLE|B_NOT_ZOOMABLE|B_FULL_UPDATE_ON_RESIZE|B_ASYNCHRONOUS_CONTROLS)
{

	// Read Prefs
	if (gPrefs.FindRect("songwindow_frame", &frame)==B_OK) {
		MoveTo( frame.LeftTop() );
	}
		
	// Menu
	BMenuBar	*menubar;
	BMenu		*menu;
	
	menubar	= new BMenuBar( BRect(0,0,10000,0), "menubar", B_FOLLOW_TOP | B_FOLLOW_LEFT_RIGHT );

	menu	= new FileMenu();
	menubar->AddItem(menu);

	menu	= new WindowsMenu(1);
	menubar->AddItem(menu);

	menu	= new HelpMenu();
	menubar->AddItem(menu);

	AddChild( menubar );

	ResizeBy(0, menubar->Bounds().bottom);
	if (BScreen().Frame().bottom - 10 < Frame().bottom) MoveBy( 0, -menubar->Bounds().bottom );

	frame = BRect(0.0, menubar->Bounds().bottom + 1.0, Bounds().right, menubar->Bounds().bottom + 28.0);
	
	// Control bar
	fToolBar = new SongToolBar( frame );
	AddChild( fToolBar );
	
	// Main View
	frame.top = frame.bottom; frame.bottom = Bounds().bottom;
	fMainView = new BView( frame, "main", B_FOLLOW_ALL, B_WILL_DRAW|B_DRAW_ON_CHILDREN|B_FRAME_EVENTS );
	AddChild( fMainView );

	fMainView->SetViewBitmap(((App *)be_app)->fHGPattern);
	fMainView->AddChild( new PlayListField( frame.OffsetToCopy(0.0, 0.0).InsetByCopy( 4.0, 4.0 ), new BMessage('plls') ) );
}

SongWindow::~SongWindow() {
	gPrefs.RemoveName("songwindow_frame");
	gPrefs.AddRect("songwindow_frame", Frame());
}

void SongWindow::MessageReceived(BMessage *message) {

	switch( message->what ) {
		case B_KEY_UP:
		case B_KEY_DOWN: {
			int32	repeat;
			message->FindInt32("be:key_repeat", &repeat);
			if (repeat>0) break;

			int32	key;
			message->FindInt32("key", &key);
			
			switch( key ) {
				case 71: // RETURN
				case 91: // ENTER
				{
					((PlayButton *)FindView("play"))->ChangeState( message->what == B_KEY_DOWN );
					
				} break;

				case 1: // ESC
				{
				}
				
				default: {
				}
			}
			
		} break;
		
		case 'sped': {
			gSong->Lock();
			gSong->SetSpeed(message->FindInt32("be:value"));
			gSong->Unlock();
		} break;

		case 'volu': {
			gSong->Lock();
			gSong->SetVelocity(message->FindInt32("be:value"));
			gSong->Unlock();
		} break;

		case 'plys': {
			((PlayButton *)FindView("play"))->SetAsStopButton( true );
			((PlayListField *)FindView("playlistfield"))->PlaySong();
		} break;
		
		case 'plyp': {
			((PlayButton *)FindView("play"))->SetAsStopButton( false );
		} break;
		
		case 'stop': {
			((PlayButton *)FindView("play"))->SetAsStopButton( false );
			((PlayListField *)FindView("playlistfield"))->Stop();
		} break;

		case 'FUpd': {
			DisableUpdates();
			gSong->Lock();
			BControl *control;
			control = (BControl *)FindView("speed");

			control->SetValue( gSong->Speed() );

			control = (BControl *)FindView("volume");
			control->SetValue( gSong->Velocity() );
			
			((PlayListField *)FindView("playlistfield"))->FullUpdate();
			
			((SongToolBar *)FindView("toolbar"))->SetEnabled(gSong->LastFilledNr()!=-1);
			gSong->Unlock();
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
			message->AddInt32("helppage", DOCU_SONGWINDOW);
			be_app->PostMessage(message);
		} break;

		default:
			BWindow::MessageReceived(message);
	}
}

bool SongWindow::QuitRequested() {
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

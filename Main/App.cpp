/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#include "App.h"

#include <Alert.h>
#include <AppFileInfo.h>
#include <Bitmap.h>
#include <Entry.h>
#include <FilePanel.h>
#include <Font.h>
#include <iostream>
#include <MidiConsumer.h>
#include <Mime.h>
#include <Node.h>
#include <NodeInfo.h>
#include <Resources.h>
#include <Roster.h>
#include <Screen.h>
#include <stdlib.h>
#include <string.h>
#include <View.h>

#include "About.h"
#include "DrumsetWindow.h"
#include "MIDIProducer.h"
#include "PatternWindow.h"
#include "Prefs.h"
#include "Song.h"

#include "SongWindow/SongWindow.h"

#define FILE_MIME "application/x-pecobeat-song"
#define APP_MIME "application/x-vnd.pecora-pecobeat"

#define INTERNALMIDI_SIGNATURE "application/x-vnd.haiku-internal_midi"

App::App()
:	BApplication(APP_MIME),
	fFilePanel( 0 )

{

	// MIME initialisieren
	BMimeType	mime(FILE_MIME);

	char	mime_string[B_MIME_TYPE_LENGTH];
	if (!mime.IsInstalled() || mime.GetPreferredApp(mime_string)!=B_OK) {
		mime.Install();
		mime.SetPreferredApp(APP_MIME);

		mime.SetShortDescription("PecoBeat Song");
		mime.SetLongDescription("PecoBeat Drum Beat Song");

		BResources	*Resourcen = AppResources();

		size_t		groesse;
		const void	*data;

		if ((data = Resourcen->LoadResource('ICON', "BEOS:L:application/x-pecobeat-song", &groesse))) {
			BBitmap icon(BRect(0,0,31,31), B_CMAP8 );
			icon.SetBits(data, groesse, 0, B_CMAP8);
			mime.SetIcon( &icon, B_LARGE_ICON);
		}

		if ((data = Resourcen->LoadResource('MICN', "BEOS:M:application/x-pecobeat-song", &groesse))) {
			BBitmap icon(BRect(0,0,15,15), B_CMAP8 );
			icon.SetBits(data, groesse, 0, B_CMAP8);
			mime.SetIcon( &icon, B_MINI_ICON);
		}
	}

	// Start InternalMIDI
	if ( !be_roster->IsRunning( INTERNALMIDI_SIGNATURE ) ) {
		if ( be_roster->Launch( INTERNALMIDI_SIGNATURE ) != B_OK ) {
			(new BAlert("", "FATAL ERROR: I wasn't able to find and start InternalMIDI. Is it not installed?", "Ooops...", NULL, NULL, B_WIDTH_AS_USUAL, B_STOP_ALERT))->Go();
		}
	}

	// Allgemeine Initialisierungen

	gMidiProducer = new MIDIProducer();

	gSong = new Song();

	fFilePath = "";

	// Fetch window background from Resources
	fHGPattern = FetchResourceBitmap("hgpattern");

	// Create windows
	BRect	frame = BRect(7.0, 26.0, BScreen().Frame().right - 7.0, 30.0);

	// Create drumset window
	BWindow	*drumsetwin	= new DrumsetWindow( frame );

	// Create Pattern window
	BPoint lefttop( drumsetwin->Frame().left, drumsetwin->Frame().bottom + 32.0 );
	frame = BRect( 0.0, 0.0, 99.0 + 17.0 * 32, 65.0 );
	frame.OffsetTo(lefttop);
	fWindows.AddItem(drumsetwin);

	BWindow	*patternwin = new PatternWindow( frame );
	fWindows.AddItem(patternwin);

	BView *mainview = patternwin->FindView("main");
	if (mainview) mainview->Invalidate();

	// Create Song window
	frame = BRect( 0.0, 0.0, 287.0, 121.0).OffsetToCopy( frame.right + 15.0, frame.top );

	if (!BScreen().Frame().Contains( frame.RightBottom() ) )
		frame.OffsetTo( 7.0, BScreen().Frame().bottom - frame.Height() - 7.0 );
	BWindow	*songwin	= new SongWindow( frame );
	fWindows.AddItem(songwin);

	// Read prefs and load song
	BString	lastSong;
	if (gPrefs.FindString("currentsong", &lastSong)==B_OK && lastSong!="") {
		OpenSong(lastSong.String(), true);
	}
	else {
		gSong->Lock();
		gSong->SetSpeed( 200 );
		gSong->SetVelocity( 127 );
		gSong->Unlock();
	}

	for (int i=0; i<fWindows.CountItems(); ++i ) {
		BWindow * win = ((BWindow *)fWindows.ItemAt(i));
		win->Show();
		MakeWindowVisible(win);
	}

	Broadcast('FUpd');

	Run();
}

App::~App() {

	for (int i=0; i<fWindows.CountItems(); ++i ) {
		BWindow * win = ((BWindow *)fWindows.ItemAt(i));
		win->Lock();
		win->Quit();
	}

	gPrefs.RemoveName("currentsong");
	gPrefs.AddString("currentsong", fFilePath.Path() );

	if (gSong) {
		gSong->Lock();
		gSong->Quit();
	}

	gMidiProducer->Disconnect();
	gMidiProducer->Unregister();

}

BBitmap	*App::FetchResourceBitmap(const char *name) {
	BResources	*Resourcen = AppResources();

	size_t		groesse;
	BMessage	msg;
	char		*buf;

	BString		text( "FATAL ERROR: Required resource not found: \n\n" );
	text << name;

	buf = (char *)Resourcen->LoadResource('BBMP', name, &groesse);
	if (!buf) {
		(new BAlert("", text.String(), "Go crash!", NULL, NULL, B_WIDTH_AS_USUAL, B_STOP_ALERT))->Go();
		// TODO: Build Fix
		printf("*** Resource not found: %s\n", name);
		//cerr << "*** Resource not found: " << name << endl;
		return 0;
	}
	msg.Unflatten(buf);
	return new BBitmap( &msg );
};

void App::AboutRequested() {
	const char*	about_text = "The BeOS only drum computer of the next generation\n\nCopyright ©2001 by Werner Freytag";
	About("About PecoBeat", "PecoBeat", "1.0", about_text, "Yeah!", "http://www.pecora.de/pecobeat", "PecoBeat Webpage", "freytag@gmx.de" );
}

void App::MessageReceived(BMessage *message) {

	switch (message->what) {
		case B_SILENT_RELAUNCH: { // Relaunch
			for (int i=0; i<fWindows.CountItems(); ++i ) {
				((BWindow *)fWindows.ItemAt(i))->Show();
			}
		} break;

		case 'plys':
		case 'plyp':
		case 'stop':

			gSong->PostMessage(message);
			Broadcast(message);

		break;

		case 'New': {

			NewSong();

		} break;

		case 'Open': {

			if (ReallyDoIt()) {

				entry_ref	start_directory;

				if (fFilePanel) {
					if (fFilePanel->PanelMode()==B_OPEN_PANEL) {
						fFilePanel->Show();
					}
					else {
						fFilePanel->GetPanelDirectory(&start_directory);
						delete fFilePanel;
						fFilePanel = 0;
					}
				}

				if (!fFilePanel) {
					fFilePanel = new BFilePanel(B_OPEN_PANEL, 0, &start_directory, 0, false, new BMessage('OpeF'), 0, true);
				}
				fFilePanel->Show();
			}

		} break;

		case 'Save': {

			SaveSong();

		} break;

		case 'SavA': {

			entry_ref	start_directory;

			if (fFilePanel) {
				if (fFilePanel->PanelMode()==B_SAVE_PANEL) {
					fFilePanel->Show();
				}
				else {
					fFilePanel->GetPanelDirectory(&start_directory);
					delete fFilePanel;
					fFilePanel = 0;
				}
			}

			if (!fFilePanel) {
				fFilePanel = new BFilePanel(B_SAVE_PANEL, 0, &start_directory, 0, false, new BMessage('SavF'), 0, true);
			}
			fFilePanel->Show();

		} break;

		case 'SavF': {
			entry_ref	ref;
			const char *name = 0;
			message->FindRef("directory", &ref);
			message->FindString("name", &name);

			BPath	path(&ref);
			path.Append(name);

			SaveSong( path );

		} break;

		case 'OpeF': {

			entry_ref	ref;
			message->FindRef("refs", &ref);

			BPath	path(&ref);

			OpenSong( path );

		} break;

		case 'Win0': {
			ShowWindow(0);
		} break;

		case 'Win1': {
			ShowWindow(1);
		} break;

		case 'Win2': {
			ShowWindow(2);
		} break;

		case 'Work': {
			int32 workspaces;
			if (message->FindInt32("workspaces", &workspaces)==B_OK) {
				for (int i=0; i<fWindows.CountItems(); ++i ) {
					BWindow *win = ((BWindow *)fWindows.ItemAt(i));
					win->SetWorkspaces(workspaces);
					MakeWindowVisible(win);
				}
			}
		} break;

		case 'Ope0':
		case 'Ope1':
		case 'Ope2':
		case 'Ope3':
		case 'Ope4': {

			int nr = message->what - 'Ope0';

			BString pathstr;
			if (gPrefs.FindString("recentfiles", nr, &pathstr)==B_OK) {
				OpenSong( pathstr.String() );
			}

		} break;

		case 'Docu': {
			OpenDocumentation();
		} break;


		case 'Help': {

			documentation_page	page;
			if (message->FindInt32("helppage", (int32 *)&page)==B_OK) {
				OpenDocumentation(page);
			}

		} break;

		default:
			BApplication::MessageReceived(message);

	}
}

void App::RefsReceived(BMessage *a_message) {

	entry_ref	ref;
	if (a_message->FindRef("refs", &ref)!=B_OK) return;

	BPath	path(&ref);

	OpenSong(path);

}

void App::MakeWindowVisible( BWindow *win ) {
	BScreen screen(win);
	if (!screen.Frame().Contains(win->Frame()))
		win->MoveTo( min_c(screen.Frame().right, win->Frame().left),
					 min_c(screen.Frame().bottom, win->Frame().top));
}

void App::ShowWindow(int32 nr) {

	char *title = 0;
	switch (nr) {
		case 0: title = "Pattern"; break;
		case 1: title = "Song"; break;
		case 2: title = "Drumset"; break;
		default: return;
	}

	BWindow *win;
	for (int i=0; i<fWindows.CountItems(); ++i ) {
		win = (BWindow *)fWindows.ItemAt(i);
		if (strncmp(title, win->Name(), strlen(title))==0) {
			win->Show();
			win->Minimize(false);
			win->Activate();
			MakeWindowVisible(win);
			break;
		}
	}
}

void App::UpdateSongPath( BPath path ) {

	if (path==fFilePath) return;

	if (!path.Path()) path = "";

	fFilePath = path;

	BWindow *win;
	for (int i=0; (win = WindowAt(i))!=0; ++i) {
		if (strncmp("Song", win->Name(), 4)==0) {

			BString	title = "Song";

			if (strlen(path.Path())!=0) {

				title += ":  ";
				title += path.Path();

				be_bold_font->TruncateString(&title, B_TRUNCATE_MIDDLE, 247.0);

			}
			win->Lock();
			win->SetTitle(title.String());
			win->Unlock();
			break;
		}
	}

	// Update the "Recent files" list
	if (strlen(path.Path())!=0) {

		entry_ref ref;
		if ( get_ref_for_path(path.Path(), &ref)==B_OK )
			be_roster->AddToRecentDocuments(&ref);

		BString	pathstr;
		BList	list;
		for (int i=0; (i<5) && (gPrefs.FindString("recentfiles", i, &pathstr)==B_OK); ++i) {
			if (path.Path()!=pathstr)
				list.AddItem(new BString(pathstr));
		}

		gPrefs.RemoveName("recentfiles");
		gPrefs.AddString("recentfiles", fFilePath.Path());
		for (int i=0; i<list.CountItems(); ++i) {
			BString *str = (BString *)list.ItemAt(i);
			gPrefs.AddString("recentfiles", *str);
			delete str;
		}

		Broadcast('Rcnt');
	}

}

bool App::ReallyDoIt() {

	gSong->Lock();
	bool modified = gSong->Modified();
	gSong->Unlock();

	if (modified) {
		BAlert *alert = new BAlert("", "Save changes to the current song?", "Cancel", "Don't save", "Save");
		alert->SetShortcut(0, 27);
		alert->SetShortcut(2, 's');
		switch (alert->Go()) {
			case 0: return false;
			case 1: return true;
			case 2: {

				SaveSong();

				return !gSong->Modified();

			}
		}
	}
	return true;
}

void App::NewSong() {

	if (ReallyDoIt() && gSong!=0) {

		gSong->Lock();
		gSong->Stop();
		gSong->Init();
		gSong->Unlock();

		UpdateSongPath( BPath() );

		Broadcast('FUpd');

	}
}

void App::OpenSong( BPath path, bool quiet ) {

	if (strlen(path.Path())==0 || !ReallyDoIt()) return;

	gSong->Stop();

	Broadcast('stop');

	BFile	file;
	file.SetTo(path.Path(), B_READ_ONLY );
	if (file.InitCheck()!=B_OK) {
		BString error_string = "I'm sorry, I can't open the file '";
		error_string += path.Path();
		error_string += "':\n";
		error_string += strerror(file.InitCheck());

		if (!quiet) (new BAlert("", error_string.String(), "Oh...", NULL, NULL, B_WIDTH_AS_USUAL, B_STOP_ALERT))->Go();
		return;
	}

	BMessage message;

	if (message.Unflatten( &file )!=B_OK) {
		if (!quiet) (new BAlert("", "An error occured while reading the data.", "Oh...", NULL, NULL, B_WIDTH_AS_USUAL, B_STOP_ALERT))->Go();
		return;
	}

	gSong->Lock();
	bool ok = gSong->LoadFromMessage(message)==B_OK;
	gSong->Unlock();
	if (!ok) {
		if (!quiet) (new BAlert("", "There is an error in the file structure.", "Oh...", NULL, NULL, B_WIDTH_AS_USUAL, B_STOP_ALERT))->Go();
		return;
	}

	UpdateSongPath(path);

	gSong->Lock();
	gSong->SetModified(false);
	gSong->Unlock();

}

void App::SaveSong( BPath path ) {

	if (!path.Path()) {
		if (strlen(fFilePath.Path())==0) {
			PostMessage('SavA');
			return;
		}
		else {
			path = fFilePath;
		}
	}

	BFile	file;
	file.SetTo(path.Path(), B_WRITE_ONLY|B_CREATE_FILE);
	if (file.InitCheck()!=B_OK) {
		BString error_string = "An error occured while opening the file:\n";
		error_string += strerror(file.InitCheck());

		(new BAlert("", error_string.String(), "Oh...", NULL, NULL, B_WIDTH_AS_USUAL, B_STOP_ALERT))->Go();
		return;
	}

	BMessage message;

	gSong->Lock();
	gSong->SaveToMessage(message);
	gSong->Unlock();

	if (message.Flatten( &file )!=B_OK) {
		(new BAlert("", "An error occured while writing the data", "Oh...", NULL, NULL, B_WIDTH_AS_USUAL, B_STOP_ALERT))->Go();
		return;
	}

	BNode		node(path.Path());
	BNodeInfo	nodeInfo(&node);
	nodeInfo.SetType(FILE_MIME);

	UpdateSongPath(path);

	gSong->Lock();
	gSong->SetModified(false);
	gSong->Unlock();

}

void App::Broadcast(uint32 what) {
	BMessage *msg = new BMessage(what);
	Broadcast(msg);
	delete msg;
}

void App::Broadcast(BMessage *msg) {
	BWindow *window;
	for (int i=0; i<fWindows.CountItems(); ++i ) {
		window = ((BWindow *)fWindows.ItemAt(i));
		window->PostMessage(msg);
	}
}

bool App::QuitRequested() {
	return (ReallyDoIt());
}

bool App::IsLastWindow() {

	int32 openWindowCount = 0;
	for (int i=0; i<fWindows.CountItems(); ++i ) {
		BWindow *window = ((BWindow *)fWindows.ItemAt(i));
		window->Lock();
		openWindowCount += !window->IsHidden();
		window->Unlock();
	}

	return (openWindowCount==1);

}

BPath App::FilePath() {
	return fFilePath;
}

void App::OpenDocumentation( documentation_page page ) {
	app_info	myAppInfo;
	be_app->GetAppInfo(&myAppInfo);

	BPath	HelpFilePath;
	BPath(&myAppInfo.ref).GetParent(&HelpFilePath);
	HelpFilePath.Append("documentation/");

	switch (page) {
		case DOCU_DRUMSETWINDOW:
			HelpFilePath.Append("page2.html"); break;
		case DOCU_PATTERNWINDOW:
			HelpFilePath.Append("page3.html"); break;
		case DOCU_SONGWINDOW:
			HelpFilePath.Append("page4.html"); break;
		default:
			HelpFilePath.Append("index.html");
	}

	entry_ref	ref;
	char		Signatur[B_MIME_TYPE_LENGTH];

	BMimeType("text/html").GetPreferredApp(Signatur);
	BMimeType(Signatur).GetAppHint(&ref);

	if ( (BPath(&ref).Path()==NULL) || (!BEntry(HelpFilePath.Path()).Exists()) ) {
		(new BAlert(NULL, "I cannot open the help file.", "Oh..."))->Go();
		return;
	}

	BString		Befehl(BPath(&ref).Path());
	Befehl.Append(" file://").Append(HelpFilePath.Path()).Append(" &");

	system(Befehl.String());
};

int main() {
	delete new App();
	return 0;
}

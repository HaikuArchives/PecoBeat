/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#ifndef _APP_H
#define _APP_H

#include <Application.h>
#include <List.h>
#include <Path.h>

enum documentation_page {
	DOCU_START,
	DOCU_DRUMSETWINDOW,
	DOCU_PATTERNWINDOW,
	DOCU_SONGWINDOW
};

class BBitmap;
class BFilePanel;

class App : public BApplication {

public:
					App();
					~App();

void				MessageReceived(BMessage *message);	
void				AboutRequested();
void				RefsReceived(BMessage *a_message);

BBitmap				*FetchResourceBitmap(const char *name);
bool				QuitRequested();
bool				IsLastWindow();

void				NewSong();
void				OpenSong( BPath path, bool quiet = false );
void				SaveSong( BPath path = BPath() );

BPath				FilePath();

void				OpenDocumentation( documentation_page page = DOCU_START );

BBitmap				*fHGPattern;

private:

void				ShowWindow(int32 nr);
void				MakeWindowVisible( BWindow *win );

void				UpdateSongPath(BPath path);

bool				ReallyDoIt();

void				Broadcast(uint32 what);
void				Broadcast(BMessage *msg);

BList				fWindows;
BFilePanel			*fFilePanel;
BPath				fFilePath;
};

#endif

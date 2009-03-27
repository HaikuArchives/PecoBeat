/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#include "FileMenu.h"

#include <MenuItem.h>
#include <Path.h>
#include <String.h>
#include <Window.h>

#include <iostream>
#include <string.h>

#include "App.h"
#include "Prefs.h"

FileMenu::FileMenu()
:	BMenu("File")
{

	BMenuItem	*menuitem;

	AddItem(new BMenuItem("New",		new BMessage('New'), 'N'));

	menuitem = new BMenuItem(new BMenu("Open..."),	new BMessage('Open'));
	menuitem->SetShortcut('O', 0);
	AddItem(menuitem);
	
	AddSeparatorItem();
	AddItem(new BMenuItem("Save",		new BMessage('Save'), 'S'));
	AddItem(new BMenuItem("Save as...",	new BMessage('SavA'), 'S', B_SHIFT_KEY));
	AddSeparatorItem();
	AddItem(new BMenuItem("About...",	new BMessage(B_ABOUT_REQUESTED)));
	AddSeparatorItem();
	AddItem(new BMenuItem("Close",		new BMessage(B_QUIT_REQUESTED), 'W'));
	AddSeparatorItem();
	AddItem(new BMenuItem("Quit",		new BMessage(B_QUIT_REQUESTED), 'Q'));

	SetTargetForItems(be_app);

	menuitem = FindItem("Close");
	menuitem->SetTarget(NULL);

	UpdateRecentFiles();
	
}

void FileMenu::UpdateRecentFiles() {

	BMenuItem	*menuitem = FindItem("Open...");
	BMenu		*submenu = menuitem->Submenu();
	
	while (	BMenuItem *subitem = submenu->ItemAt(0) ) {
		submenu->RemoveItem(subitem);
		delete subitem;
	}
	
	BString	pathStr;

	BPath currentPath = (((App *)be_app)->FilePath());

	BString currentPathStr = currentPath.Path();
	
	if (currentPath.Path()) {
		currentPathStr.Truncate( strlen(currentPath.Path()) - strlen(currentPath.Leaf()) );
	}
	
	for (int i=0; (i<5) && (gPrefs.FindString("recentfiles", i, &pathStr)==B_OK); ++i) {
		
		if (currentPath.Path()) {
			BPath	path(pathStr.String());
			BString newPathStr( pathStr.String(), strlen(path.Path()) - strlen(path.Leaf()) );
			
			if (currentPathStr==newPathStr) {
				pathStr = path.Leaf();
			}
		}
		
		submenu->AddItem( new BMenuItem(pathStr.String(),
			new BMessage('Ope0' + i), '1' + i, B_SHIFT_KEY ) );
	}

	submenu->SetTargetForItems(be_app);

}

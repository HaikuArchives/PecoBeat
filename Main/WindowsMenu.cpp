/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#include "WindowsMenu.h"

#include <Application.h>
#include <MenuItem.h>

WindowsMenu::WindowsMenu( int32 disabledWindow )
:	BMenu("Windows")
{

	BMenuItem	*menuitem;

	menuitem = new BMenuItem("Pattern", new BMessage('Win0'), '0');
	if (disabledWindow==0) menuitem->SetEnabled(false);
	AddItem(menuitem);
	
	menuitem = new BMenuItem("Song", new BMessage('Win1'), '1');
	if (disabledWindow==1) menuitem->SetEnabled(false);
	AddItem(menuitem);

	menuitem = new BMenuItem("Drumset", new BMessage('Win2'), '2');
	if (disabledWindow==2) menuitem->SetEnabled(false);
	AddItem(menuitem);

	SetTargetForItems(be_app);

	AddSeparatorItem();
	AddItem(new BMenuItem( "Move all to this workspace", new BMessage('Work')));
	
}

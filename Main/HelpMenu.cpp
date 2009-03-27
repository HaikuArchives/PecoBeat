/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#include "HelpMenu.h"

#include <Application.h>
#include <MenuItem.h>

HelpMenu::HelpMenu()
:	BMenu("?")
{

	BMenuItem *helpitem = new BMenuItem("Help", new BMessage('Help'), 'H');
	AddItem( helpitem );
	
	AddSeparatorItem();
	AddItem( new BMenuItem("Documentation", new BMessage('Docu'), 'D') );
	AddSeparatorItem();
	AddItem(new BMenuItem("About...",	new BMessage(B_ABOUT_REQUESTED)));

	SetTargetForItems(be_app);

	helpitem->SetTarget(NULL);
}

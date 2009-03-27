/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#ifndef _WINDOWS_MENU_H
#define _WINDOWS_MENU_H

#include <Menu.h>

class WindowsMenu : public BMenu {

public:
			WindowsMenu(  int32 disabledWindow  );
};

#endif

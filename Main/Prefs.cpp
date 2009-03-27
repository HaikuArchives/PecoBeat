/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#include "Prefs.h"

#include <FindDirectory.h>
#include <Path.h>

#define PREFS_FILENAME "PecoBeat_prefs"

Prefs gPrefs;

Prefs::Prefs() {

	BFile file = PrefsFile( B_READ_ONLY );
	Unflatten( &file );
	
}

Prefs::~Prefs() {

	BFile file = PrefsFile( B_WRITE_ONLY|B_CREATE_FILE );
	Flatten( &file );

}

BFile Prefs::PrefsFile( int32 mode ) {

	BPath	path;
	find_directory(B_USER_SETTINGS_DIRECTORY, &path);
	path.SetTo( path.Path(), PREFS_FILENAME );
	
	return BFile( path.Path(), mode );
}

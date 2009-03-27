/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#ifndef _PREFS_H
#define _PREFS_H

#include <File.h>
#include <Message.h>

class Prefs : public BMessage {

public:
		Prefs();
		~Prefs();

private:

BFile		PrefsFile( int32 mode );

};

extern Prefs gPrefs;

#endif

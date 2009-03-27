/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

/* Definitionen der Noten: Keine Note, An, Aus */

#ifndef _PATTERN_DEFS_H
#define _PATTERN_DEFS_H

enum {
	NOTE_NONE = 0x00,
	NOTE_RUBBER = 0x00,
	NOTE_ON = 0x01,
	NOTE_OFF = 0x02
};

#endif

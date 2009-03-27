/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#ifndef _DRUMSET_DROPFIELD
#define _DRUMSET_DROPFIELD

#include <Control.h>

class	DrumsetDropField : public BControl {

public:
				DrumsetDropField(BRect frame, const char *name, BMessage *message, uint32 resizeMask, uint32 flags);
virtual	void	AttachedToWindow();
virtual	void	Draw(BRect updateRect);
virtual	void	MouseMoved(	BPoint where, uint32 code, const BMessage *a_message);
virtual void	MessageReceived(BMessage *message);
virtual			~DrumsetDropField();

private:

BPicture		*fNormalBitmap, *fActiveBitmap;
};

#endif

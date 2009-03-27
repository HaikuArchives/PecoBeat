/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#ifndef _DRUMSET_BUTTON_H
#define _DRUMSET_BUTTON_H

#include <Bitmap.h>
#include <PictureButton.h>
#include <Point.h>
#include <Rect.h>

enum { kNormalButton, kPressedButton, kFloatingButton };

const	BRect kDrumsetButtonBounds(0.0, 0.0, 97.0, 17.0);

class BMessageRunner;

class	DrumsetButton : public BPictureButton {

public:
					DrumsetButton(BRect rect, int key, 
						BMessage *message=NULL, bool allow_drag=true,
						bool allow_drop=false);
virtual				~DrumsetButton();

virtual	void		AttachedToWindow();

void				MouseDown(BPoint where);
void				MouseUp(BPoint where);
void				MouseMoved(	BPoint where, uint32 code, const BMessage *a_message);

void				MessageReceived( BMessage *message );
status_t			Invoke(BMessage *msg = NULL);

void				SetKey(int8 key=-1); // -1 heisst: initialisieren!
inline int8			Key() { return fKey; }

void				StartDrag( BPoint where );

private:

void				CreateIt(BView *view, int32 mode, int8 key=-1);
BBitmap				*fDragBitmap;
int8				fKey;
bool				fAllowDrag;
bool				fAllowDrop;
BMessageRunner		*fMessageRunner;
};

#endif

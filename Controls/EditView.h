/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#ifndef PATTERN_EDIT_VIEW_H
#define PATTERN_EDIT_VIEW_H

#include <View.h>
#include <Point.h>

#include <Bitmap.h>
#include <Control.h>
#include <Rect.h>
#include <View.h>

#include "PatternRow.h"

class EditViewBgBuffer;
class PatternWindow;

/* -------------------- EditView ------------------------------------ */

class EditView : public BControl {

public:
					EditView(BRect rect);

void				Draw(BRect updateRect);
void				AttachedToWindow();
void				FrameResized(float new_width, float new_height);

void				MouseDown(BPoint where);
void				MouseUp(BPoint where);
void				MouseMoved(	BPoint where, uint32 code, const BMessage *a_message);
					~EditView();

void				Draw( bool deep_draw = false );

inline void			SetNote(uint8 note, uint8 mode, bool quiet = false);
inline uint8		Note(uint8 note);

void				Init();
void				SaveToMessage( BMessage	&msg) const;
void				LoadFromMessage(BMessage &msg);

private:

void				CreateBuffer();
void				DeleteBuffer();
void				CutCoordinate( BPoint &where );
inline uint8		GetXCoordinate( float x );
void				PlaySound( bool note_on );
void				UpdatePatternRow(int tonNr, int drawMode );

BBitmap				*fBgBufferBitmap; 	// Background
EditViewBgBuffer	*fBgBuffer;			// 

BBitmap				*fDrawBufferBitmap;	// hier wird gezeichnet und 
BView				*fDrawBuffer;		// in die EditView rüberkopiert

int32				fRemovedFromCol; 	// Wenn Note verschoben wird, hier Position merken;
PatternWindow		*fWindow;
uint32				fMouseButtons;

PatternRow			*fPatternRow;
};


/* -------------------- EditViewBgBuffer ------------------------------------ */

class EditViewBgBuffer : public BView {

public:
					EditViewBgBuffer( BRect );
void				AttachedToWindow();
void				Draw();

inline void			SetNote(uint8 note, uint8 mode, bool quiet = false);
inline uint8		Note(uint8 note);

private:
uint8				fNotes[256];
};

#endif

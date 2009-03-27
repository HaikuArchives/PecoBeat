/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#ifndef _PATTERN_MAIN_VIEW_H
#define _PATTERN_MAIN_VIEW_H

#include <Message.h>
#include <View.h>

#include "PatternRow.h"
#include "PatternWindow.h"
#include "Skala.h"

class PatternRowView;
class Window;

class PatternMainView : public BView {

friend class PatternRowView;

public:
				PatternMainView( BRect frame );
				~PatternMainView();

void			UpdateRowViews();

void			AttachedToWindow();
void			Draw(BRect updateRect);
void			UpdateChildren();

void			MouseMoved(BPoint pt, uint32 code, const BMessage *msg);
void			MouseUp(BPoint where);
void			MessageReceived( BMessage *msg );

void			AddSkala();
void			RemoveSkala();
void			DragRow( PatternRowView *row );

void			FullUpdate();

void			PlaySong();
void			PlayPattern();

void			Stop();

private:

void			StartMessageRunner();

// Diese Funktionen fügen die neue Row nur im Song hinzu!				
int32			AddPatternRowToSong( BMessage &msg, int32 index = -1 );
void			RemovePatternRowFromSong( int32 index );

// Diese Funktionen fügen lediglich die View hinzu!
void			AddPatternRowView( int32 index = -1 );
void			RemovePatternRowView( int32 index );

void			DeleteDragWindow();

// variables
BList			fInstrumentList;

Skala			*fSkala;
BWindow			*fDragWindow;

PatternRowView	*fFollowingRow;
BMessage		*fRemovedRow;

int32			fRowOverIndex;
BMessageRunner	*fMessageRunner;
};

#endif

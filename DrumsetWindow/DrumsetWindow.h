/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */


#ifndef _DRUMSETWINDOW_H
#define _DRUMSETWINDOW_H

#include <Window.h>

class BMessageRunner;

class DrumsetWindow : public BWindow {

public:
				DrumsetWindow( BRect frame );
				~DrumsetWindow();

void			FindInternalMIDI();
		
void			MessageReceived(BMessage *message);

void			Zoom( BPoint rec_position, float rec_width, float rec_height);
void			FrameResized(float new_width, float new_height);

void			WorkspacesChanged(uint32 old_ws, uint32 new_ws);
void			ScreenChanged(BRect screen_size, color_space depth);

void			HideWithFlag();

bool			QuitRequested();

private:

inline int32	HeightToRows( float height );
inline int32	WidthToCols( float width );

inline float	RowsToHeight( int32 rows );
inline float	WidthForRows( int32 rows );

void			UpdateSize( bool zoom = false );

BMessageRunner	*fMessageRunner;
int32			fWidthOffset, fHeightOffset;
};

#endif

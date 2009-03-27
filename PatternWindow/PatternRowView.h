/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#ifndef _PATTERN_ROW_VIEW_H
#define _PATTERN_ROW_VIEW_H

#include <View.h>

class DrumsetButton;
class EditView;
class SideGrip;
class Knob;

class PatternRowView : public BView {

public:
					PatternRowView( int key = 0, int velocity = 100 );
					PatternRowView( const PatternRowView& copy, bool controlsDisabled = false );

void				Init( bool controlsDisabled = false );
void				LoadFromMessage( BMessage &msg );
void				SaveToMessage( BMessage &msg ) const;

void				AttachedToWindow();
void				MessageReceived(BMessage *msg);

void				Draw(BRect updateRect);
void				Draw() { Draw( Bounds() ); }

inline int32		RowIndex();

private:

SideGrip			*fSideGrip;
DrumsetButton		*fDrumsetButton;
Knob				*fKnob;
EditView			*fEditView;
};

#endif

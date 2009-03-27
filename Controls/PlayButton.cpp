/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#include "PlayButton.h"

#include <Window.h>

#include "App.h"

#include <iostream>

PlayButton::PlayButton(BRect frame, const char* name, uint32 playmode)
:	BPictureButton(frame, name, new BPicture(), new BPicture(), new BMessage(), B_ONE_STATE_BUTTON ),
	fPlay0( 0 ),
	fPlay1( 0 ),
	fStop0( 0 ),
	fStop1( 0 ),
	fMousePressedOverButton( false ),
	fIsPlayButton( true ),
	fPlayMode( playmode )
{
	SetTarget(be_app);
}

PlayButton::~PlayButton() {
	delete fPlay0;
	delete fPlay1;
	delete fStop0;
	delete fStop1;
}

void PlayButton::AttachedToWindow() {

	be_app->Lock();

	BeginPicture( new BPicture() );
	DrawBitmap (((App *)be_app)->FetchResourceBitmap("play0"));
	fPlay0 = EndPicture();
	
	BeginPicture( new BPicture() );
	DrawBitmap (((App *)be_app)->FetchResourceBitmap("play1"));
	fPlay1 = EndPicture();

	BeginPicture( new BPicture() );
	DrawBitmap (((App *)be_app)->FetchResourceBitmap(fPlayMode==PLAY_SONG ? "pause1" : "stop1"));
	fStop0 = EndPicture();
	
	BeginPicture( new BPicture() );
	DrawBitmap (((App *)be_app)->FetchResourceBitmap(fPlayMode==PLAY_SONG ? "pause0" : "stop0"));
	fStop1 = EndPicture();

	be_app->Unlock();

	SetEnabledOn( fPlay1 );
	SetEnabledOff( fPlay0 );
	SetDisabledOn( fPlay1 );
	SetDisabledOff( fPlay0 );
	SetEnabled(false);
	
	BPictureButton::AttachedToWindow();
}

void PlayButton::SetEnabled(bool on) {
	SetAsStopButton( false );
	SetValue( 0 );
	BPictureButton::SetEnabled( on );
}


status_t PlayButton::Invoke(BMessage *msg) {

	if (!msg) msg = Message();

	msg->what =	fIsPlayButton
		?
			( fPlayMode==PLAY_SONG ? 'plys' : 'plyp' )
		:
			'stop';

	return BInvoker::Invoke(msg);
	
}

void PlayButton::ChangeState( bool buttondown ) {

	if (!IsEnabled()) return;
	
	SetValue(!Value());
	if (buttondown) Invoke();
}

void PlayButton::MouseDown(BPoint where)
{
	if (!IsEnabled()) return;
	
	ChangeState( true );

	SetMouseEventMask( B_POINTER_EVENTS );
	fMousePressedOverButton = true;
}

void PlayButton::MouseUp(BPoint where)
{
	if (fMousePressedOverButton) ChangeState( false );
	fMousePressedOverButton = false;
}

void PlayButton::MouseMoved(BPoint pt, uint32 code, const BMessage *msg)
{

	if (!fMousePressedOverButton || !IsEnabled()) return;
	
	BPoint	where;
	uint32	buttons;

	GetMouse( &where, &buttons );
	if (!buttons || code==B_EXITED_VIEW) {
		MouseUp( where );
		return;
	}

}

void PlayButton::SetAsStopButton( bool doit ) {
	fIsPlayButton = !doit;
	SetEnabledOn( doit ? fPlay1 : fStop0 );
	SetEnabledOff( doit ? fStop1 : fPlay0 );
	Draw( Bounds() );
}


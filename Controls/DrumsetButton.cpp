/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#include <iostream>

#include <stdio.h>
#include <math.h>
#include <MessageRunner.h>
#include <Window.h>

#include "App.h"
#include "DrumsetButton.h"
#include "Instruments.h"

DrumsetButton::DrumsetButton(BRect rect, int key, BMessage *message, bool allow_drag, bool allow_drop)
	: BPictureButton(rect, NULL, new BPicture(), new BPicture(), message),
	  fKey(key),
	  fAllowDrag(allow_drag),
	  fAllowDrop(allow_drop),
	  fMessageRunner( 0 )
{
	
	rect = BRect(0.0, 0.0, Bounds().right + 2, Bounds().bottom + 2);

	fDragBitmap = new BBitmap(rect, B_RGB32, true);
	BView	*view = new BView( rect, NULL, B_FOLLOW_NONE, 0);

	fDragBitmap->AddChild(view);
	SetViewColor(57, 101, 140);
}

void DrumsetButton::AttachedToWindow() {
	SetKey();
	SetTarget( Parent() );
}

void DrumsetButton::SetKey(int8 key) {
	if (key==fKey) return;
	
	if (key!=-1) {
		if (key!=0 && key<kFirstMidiID || key>kLastMidiID) {
			// TODO: Build Fix
			printf("DrumsetButton: Key out of range");
			//cerr << "DrumsetButton: Key out of range" << endl;
			return;
		}
		fKey = key;
	}

	SetLabel(kInstruments[fKey - kFirstMidiID]);
	
	fDragBitmap->Lock();
	BView	*view	=	fDragBitmap->ChildAt(0);
		
	BFont font;
	font.SetFamilyAndStyle("Swis721 BT","Roman"); 
	
	font.SetSize(9.5);
	view->SetFont(&font);

	BPicture *picture;	

	view->BeginPicture( new BPicture() );
	CreateIt(view, kNormalButton, fKey);
	picture = view->EndPicture();
	SetEnabledOff(picture);
	SetDisabledOff(picture);
	delete picture;
		
	view->BeginPicture( new BPicture() );
	CreateIt(view, kPressedButton, fKey);
	picture = view->EndPicture();
	SetEnabledOn(picture);
	SetDisabledOn(picture);
	delete picture;
	
	CreateIt(view, kFloatingButton, fKey);
	view->Flush();
	
	fDragBitmap->Unlock();

	Invalidate();
}

status_t DrumsetButton::Invoke(BMessage *msg) {

	if (msg==NULL) msg = Message();

	if (!msg) return B_ERROR;

	msg->RemoveName("key");
	msg->AddInt8("key", fKey);
	msg->RemoveName("be:value");
	msg->AddBool("be:value", (bool)Value());

	return BInvoker::Invoke(msg);
	
}

void DrumsetButton::CreateIt(BView *view, int32 mode, int8 key) {
	
	// Fetch Bitmap from Resource
	BBitmap		*instr_button_bmp = ((App *)be_app)->FetchResourceBitmap("button_instr");
	
	BRect	frame = BRect(0.0, 0.0, 97.0, 17.0);

	if (mode==kFloatingButton) {
		view->SetHighColor(0,0,0,0);
		view->FillRect(view->Bounds());
		view->SetHighColor(0, 0, 0, 30);
		view->FillRoundRect(view->Bounds(), 3, 3);
		view->SetHighColor(0, 0, 0, 50);
		view->FillRoundRect(view->Bounds().InsetByCopy(1.0,1.0), 2, 2);
	}
	else {
		view->SetHighColor(60, 90, 140, 0); 
		view->FillRect(view->Bounds());
	}

	view->SetDrawingMode(B_OP_ALPHA);
	if (mode==kFloatingButton) {
		view->SetHighColor(255, 255, 255, 200); 
		view->SetBlendingMode(B_CONSTANT_ALPHA, B_ALPHA_OVERLAY);
	}
	else {
		view->SetHighColor(255, 255, 255, 255); 
	}
	view->DrawBitmap(instr_button_bmp, BPoint(1.0,1.0));

	view->SetHighColor(0, 0, 0, 255); 
	view->StrokeRoundRect(frame, 2, 2);

	// Farbe
	float farb_nr = key - kFirstMidiID;
 	float multip = M_PI/30; // um einen Bereich von 0..2pi zu bekommen
	int r = (int)(sin(farb_nr*multip)*100+155);
	int g = (int)(sin((farb_nr+20)*multip)*100+155);
	int b = (int)(sin((farb_nr+40)*multip)*100+155);
	
	view->SetHighColor(r, g, b, 200); 
	BRect	rect = view->Bounds().InsetByCopy(2.0, 2.0).OffsetToCopy(1.0, 1.0); 
	view->StrokeRect(rect);

	if (mode==kPressedButton) {
		view->SetDrawingMode(B_OP_ALPHA);
		view->SetHighColor(0, 0, 0, 60);
		view->FillRect(rect, B_SOLID_LOW);
	}

	view->SetDrawingMode(B_OP_COPY);
	view->SetLowColor(20, 20, 30);
	view->SetHighColor(255, 255, 255);
	BPoint pos(5 + (mode==kPressedButton), 11 + (mode==kPressedButton));
	
	if (key>=27 && key<=86)
		view->DrawString(Label(), pos);
};

void DrumsetButton::MouseMoved(	BPoint where, uint32 code, const BMessage *a_message) {

	// Instrument wird über ein anderes gezogen
	if (a_message) {

		if (a_message->what=='PecM' && fAllowDrop) 
			SetValue(code==B_ENTERED_VIEW||code==B_INSIDE_VIEW);

	}
	
	// Keine Drop-Message
	else {
		int32 buttons;
		Window()->CurrentMessage()->FindInt32("buttons", &buttons);

		if	(	(Value() && buttons && code == B_INSIDE_VIEW)
			||	(!Value() && !buttons && code == B_OUTSIDE_VIEW)
			||	(!Value() && (code==B_ENTERED_VIEW || code==B_EXITED_VIEW) ) )
			return;
		
		if (buttons) {
			switch (code) {
				case B_ENTERED_VIEW: {
					SetValue(1);
					Invoke();
				} break;
				
				case B_EXITED_VIEW: {
					SetValue(0);
					Invoke();
					if ( fAllowDrag || modifiers() & B_SHIFT_KEY ) StartDrag( where );
				} break;
			}
		}
		// keine Buttons
		else {
			SetValue( 0 );
		}
	}
}

void DrumsetButton::MouseDown(BPoint where) {

	SetMouseEventMask(0, B_SUSPEND_VIEW_FOCUS);

	SetValue(1);
	Invoke();
	
	if (!fAllowDrag) {
		delete fMessageRunner;
		fMessageRunner = new BMessageRunner( this, new BMessage('ddrg'), 400000, 1 );
	}

};

void DrumsetButton::MouseUp(BPoint where) {

	if ( !Value() ) return;

	delete fMessageRunner;
	fMessageRunner = 0;

	SetValue(0);
	Invoke();
	
};

void DrumsetButton::MessageReceived( BMessage *message ) {

	switch (message->what) {
		case 'PecM': {
			if (!fAllowDrop) return;

			int8		key;
			if (message->FindInt8("key", &key) != B_OK) return;
			
			if (key!=Key()) {

				SetKey(key);
			
				message->what = 'setk';
				Invoke( message );
			}
		} break;
		
		case 'ddrg': {

			BPoint	where;
			uint32	buttons;
			GetMouse( &where, &buttons );
			
			if (!buttons || !Bounds().Contains(where)) return;
			
//			SetValue(0);
//			Invoke();

			StartDrag( where );

		} break;
	}
}

void DrumsetButton::StartDrag( BPoint where ) {
	if (where.x<0) where.x = 0;
	else if (where.x>Bounds().right) where.x = Bounds().right;

	if (where.y<3) where.y = 3;
	else if (where.y>Bounds().bottom) where.y = Bounds().bottom-3;
	where.x += 1;
	where.y += 1;

	BMessage	drag_message('PecM');
	drag_message.AddInt8("key", fKey);
	DragMessage(&drag_message, new BBitmap(fDragBitmap), B_OP_ALPHA, where);
}

DrumsetButton::~DrumsetButton() {
	delete fMessageRunner;
	delete fDragBitmap;
}

/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#include "Ticker.h"

#include <Message.h>
#include <MessageQueue.h>

#include <iostream>

int32 ticker(void *data) {
	
	BMessage	*msg = (BMessage *)data;
	
	int64	snooze_time = 0;
	msg->FindInt64("snooze", &snooze_time);

	BLooper		*looper = 0;
	msg->FindPointer("looper", (void **)&looper);

	delete msg;
	
	if (looper==0 || snooze_time==0) return 10;
	
	long long nexttick = system_time();
	
	for (;;) {
		nexttick += snooze_time;
		snooze_until(nexttick,  B_SYSTEM_TIMEBASE);
		looper->PostMessage('tick');
	}
	return 0;
}

Ticker::Ticker( BLooper *receiver )
:	BLooper("Ticker", 50 ),
	fDelay( 0 ),
	fReceiver( receiver ),
	fTicker( 0 ),
	fNewDelay( 0 )
{
	Run();
}

Ticker::~Ticker() {
	Stop();
}

void Ticker::MessageReceived(BMessage *msg) {

	if (msg->what=='tick') {
		if (fNewDelay) {
			fDelay = fNewDelay;
			fNewDelay = 0;
			Start();
		}
		fReceiver->PostMessage(msg);
	}
}

void Ticker::Start() {

	Stop();
	BMessage	*msg = new BMessage();
	msg->AddInt64("snooze", fDelay);
	msg->AddPointer("looper", this);
	
	fTicker = spawn_thread(ticker, "Beat Ticker", 50, msg);
	resume_thread(fTicker);

}

void Ticker::Stop() {

	if (fTicker) {
		kill_thread(fTicker);
		fTicker = 0;
	}

}

void Ticker::SetDelay( int32 delay ) {
	
	if (!fDelay) fDelay = delay;
	else fNewDelay = delay;
}

int32 Ticker::Delay() {
	return fDelay;
}


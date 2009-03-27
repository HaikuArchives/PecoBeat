/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

/****************************************************************************
* Der Ticker ist ein BLooper, der in regelmäßigen Abständen Messages		*
* an einen anderen BLooper sendet.											*
*****************************************************************************/

#ifndef _TICKER_H
#define _TICKER_H

#include <Looper.h>

#include <iostream>

class Ticker : public BLooper {

public:
				Ticker( BLooper *receiver );
				~Ticker();
				
void 			MessageReceived(BMessage *msg);

void			Start();
void			Stop();

void			SetDelay( int32 delay );
int32			Delay();

private:
int32			fDelay;
BLooper			*fReceiver;
thread_id		fTicker;
int32			fNewDelay;
};

#endif

/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#include <AppFileInfo.h>
#include <Bitmap.h>
#include <MidiRoster.h>
#include <Roster.h>

#include <iostream>

#include "MIDIProducer.h"

#include <MidiConsumer.h>

// Initialize Midi
MIDIProducer	*gMidiProducer = 0;

MIDIProducer::MIDIProducer()
:	BMidiLocalProducer( "PecoBeat" ),
	fConsumer( 0 )
{

/*
	app_info appInfo; 
	BFile file; 
	BAppFileInfo appFileInfo; 

	be_app->GetAppInfo(&appInfo); 
	file.SetTo(&appInfo.ref, B_READ_WRITE); 
	appFileInfo.SetTo(&file);
	
	BMessage	props;
	BBitmap		*bitmap;
	
	bitmap = new BBitmap(BRect(0,0,31,31), B_CMAP8);
	appFileInfo.GetIcon(bitmap, B_LARGE_ICON);
	props.AddData("be:large_icon", 'ICON', bitmap->Bits(), 1024);
	delete bitmap;
	
	bitmap = new BBitmap(BRect(0,0,15,15), B_CMAP8);
	appFileInfo.GetIcon(bitmap, B_MINI_ICON);
	props.AddData("be:small_icon", 'MICN', bitmap->Bits(), 256);
	delete bitmap;

	SetProperties( &props );
	
	Register();
*/
}

void MIDIProducer::ChannelPressure(uchar channel, uchar pressure, bigtime_t time) const {
	BMidiLocalProducer::SprayChannelPressure(channel - 1, pressure, time);
}

void MIDIProducer::PitchBend(uchar channel, uchar lsb, uchar msb, bigtime_t time) const {
	BMidiLocalProducer::SprayPitchBend(channel - 1, lsb, msb, time);
}

void MIDIProducer::SprayNoteOn(uchar channel, uchar note, uchar velocity, bigtime_t time) const {
	BMidiLocalProducer::SprayNoteOn(channel - 1, note, velocity, time);
}

void MIDIProducer::SprayNoteOff(uchar channel, uchar note, uchar velocity, bigtime_t time) const {
	BMidiLocalProducer::SprayNoteOff(channel - 1, note, velocity, time);
}

void MIDIProducer::ProgramChange(uchar channel, uchar progNum, bigtime_t time) const {
	BMidiLocalProducer::SprayProgramChange(channel - 1, progNum, time);
}

void MIDIProducer::Connect( BMidiConsumer *consumer) {

	Disconnect();
	if (BMidiProducer::Connect(consumer)==B_OK) {
		fConsumer = consumer;
	}
	
}

void MIDIProducer::Disconnect() {

	if (fConsumer) {
		BMidiProducer::Disconnect( fConsumer );
		fConsumer = 0;
	}

}

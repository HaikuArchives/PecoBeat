/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#ifndef _MIDI_PRODUCER_H
#define _MIDI_PRODUCER_H

#include <MidiProducer.h>
#include <MidiConsumer.h>

class	MIDIProducer : public BMidiLocalProducer {

public:
		MIDIProducer();

void	Connect( BMidiConsumer *consumer );
void	Disconnect();

void	ChannelPressure(uchar channel, uchar pressure, bigtime_t time = system_time()) const;
void	PitchBend(uchar channel, uchar lsb, uchar msb, bigtime_t time = system_time()) const;
void	SprayNoteOn(uchar channel, uchar note, uchar velocity, bigtime_t time = system_time()) const;
void	SprayNoteOff(uchar channel, uchar note, uchar velocity, bigtime_t time = system_time()) const;
void	NoteOn(uchar channel, uchar note, uchar velocity, bigtime_t time = system_time()) const;
void	NoteOff(uchar channel, uchar note, uchar velocity, bigtime_t time = system_time()) const;
void	ProgramChange(uchar channel, uchar progNum, bigtime_t time = system_time()) const;

private:
BMidiConsumer	*fConsumer;

};

extern MIDIProducer	*gMidiProducer;

#endif

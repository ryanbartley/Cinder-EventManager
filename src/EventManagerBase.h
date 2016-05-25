//========================================================================
// EventManager.h : implements a multi-listener multi-sender event system
//
// Part of the GameCode4 Application
//
// GameCode4 is the sample application that encapsulates much of the source code
// discussed in "Game Coding Complete - 4th Edition" by Mike McShaffry and David
// "Rez" Graham, published by Charles River Media.
// ISBN-10: 1133776574 | ISBN-13: 978-1133776574
//
// If this source code has found it's way to you, and you think it has helped you
// in any way, do the authors a favor and buy a new copy of the book - there are
// detailed explanations in it that compliment this code well. Buy a copy at Amazon.com
// by clicking here:
//    http://www.amazon.com/gp/product/1133776574/ref=olp_product_details?ie=UTF8&me=&seller=
//
// There's a companion web site at http://www.mcshaffry.com/GameCode/
//
// The source code is managed and maintained through Google Code:
//    http://code.google.com/p/gamecode4/
//
// (c) Copyright 2012 Michael L. McShaffry and David Graham
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser GPL v3
// as published by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
// http://www.gnu.org/licenses/lgpl-3.0.txt for more details.
//
// You should have received a copy of the GNU Lesser GPL v3
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//
//========================================================================

#pragma once

#include <string>
#include "BaseEventData.h"
#include "FastDelegate.h"
	
using EventType				= uint64_t;
using EventListenerDelegate = fastdelegate::FastDelegate1<EventDataRef, void>;

class EventManagerBase {
public:
	
	enum eConstants { kINFINITE = 0xffffffff };
	explicit EventManagerBase( const std::string &name, bool setAsGlobal );
	virtual ~EventManagerBase();
	
	//! Registers a delegate function that will get called when the event type is
	//! triggered. Returns true if successful, false if not.
	virtual bool addListener( const EventListenerDelegate &eventDelegate, const EventType &type ) = 0;
	
	//! Removes a delegate / event type pairing from the internal tables.
	//! Returns false if the pairing was not found.
	virtual bool removeListener( const EventListenerDelegate &eventDelegate, const EventType &type ) = 0;
	
	//! Fires off event NOW. This bypasses the queue entirely and immediately
	//! calls all delegate functions registered for the event.
	virtual bool triggerEvent( const EventDataRef &event ) = 0;
	
	//! Fires off event. This uses the queue and will call the delegate
	//! function on the next call to tickUpdate. assuming there's enough time.
	virtual bool queueEvent( const EventDataRef &event ) = 0;
	
	// Finds the next-available instance of the named event type and remove it
	// from the processing queue. This may be done up to the point that it is
	// actively being processed ... e.g.: is safe to happen during event
	// processing itself.
	//
	// If allOfType is true, then all events of that type are cleared from the
	// input queue. //
	// returns true if the event was found and removed, false otherwise
	virtual bool abortEvent( const EventType& type, bool allOfType = false ) = 0;
	
	// Allows for processing of any queued messages, optionally specify a
	// processing time limit so that the event processing does not take too
	// long. Note the danger of using this artificial limiter is that all
	// messages may not in fact get processed.
	//
	// returns true if all messages ready for processing were completed, false
	// otherwise (e.g. timeout).
	virtual bool update( uint64_t maxMillis = kINFINITE ) = 0;
	
	// Getter for the main global event manager. This is the event manager that
	// is used by the majority of the engine, though you are free to define your
	// own as long as you instantiate it with setAsGlobal set to false.
	// It is not valid to have more than one global event manager.
	static EventManagerBase* get();
	
	//! Registers a delegate function that will get called when the event type is
	//! triggered. NOTE: This listener can be called from any thread. Appropriate
	//! locks in the listener should be considered. Returns true if successful,
	//! false if not. This function is Thread Safe
	virtual bool addThreadedListener( const EventListenerDelegate &eventDelegate, const EventType &type ) = 0;
	//! Removes a delegate / event type pairing from the internal tables. This
	//! function removes in a Thread Safe manner. Returns false if the pairing
	//! was not found.
	virtual bool removeThreadedListener( const EventListenerDelegate &eventDelegate, const EventType &type ) = 0;
	//! Fires off event NOW. NOTE: This function could be called from any thread.
	//! This bypasses the queue entirely and immediately calls all delegate functions
	//! registered to listen for this event.
	virtual bool triggerThreadedEvent( const EventDataRef &event ) = 0;
	virtual void removeAllThreadedListeners() = 0;
};
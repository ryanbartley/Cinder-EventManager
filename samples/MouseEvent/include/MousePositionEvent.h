//
//  MouseEvent.h
//  MouseEvent
//
//  Created by Ryan Bartley on 1/20/15.
//
//

#pragma once

#include "cinder/Vector.h"

#include "BaseEventData.h"

using MousePositionEventRef = std::shared_ptr<class MousePositionEvent>;

class MousePositionEvent : public EventData {
public:
	
	//! Simplifies the creation of this shared pointer. This system uses a
	//! lot of casting back and forth to transmit things cleanly. We'll use
	//! a shared_ptr so as to not leak but this can easily be changed in the
	//! implementation of the EventManager to be a unique ptr or a raw pointer
	static MousePositionEventRef create( ci::ivec2 position );
	//! Simplifies the creation of this shared pointer. This system uses a
	//! lot of casting back and forth to transmit things cleanly. We'll use
	//! a shared_ptr so as to not leak but this can easily be changed in the
	//! implementation of the EventManager to be a unique ptr or a raw pointer
	static MousePositionEventRef create();
	
	//! virtual destructor in case you want to further specialize this type of
	//! event.
	virtual ~MousePositionEvent() {}
	
	//! This is here to be able to make a deep copy of this event. For instance,
	//! if you have logic that makes things die after a specific time or after
	//! the event was handled or if you wanted to transport this event to another
	//! EventManager somewhere else in your system, the event should know how
	//! to copy itself.
	virtual EventDataRef copy() { return create( mPosition ); }
	
	//! The RTTI of the EventManager to keep everything organized. Initialized
	//! in the .cpp
	static EventType TYPE;
	//! Because this is virtual and we're overriding it, we can keep control
	//! over the RTTI.
	virtual EventType getEventType() const { return TYPE; }
	//! This is for debug purposes. We could've made it static but it needs to
	//! be enforced that people write it.
	virtual const char* getName() const { return "MouseEvent"; }
	
	//! This is a cool, yet rarely useful part of the interface. It allows you
	//! centralized control over how this event serializes it's internal data.
	//! For our purposes we'll just leave it blank but this is super useful for
	//! internet applications as it uncouples the need for your application to
	//! know the details of your connection to the internet.
	virtual void serialize( ci::Buffer &streamOut ) {}
	//! This is a cool, yet rarely useful part of the interface. It allows you
	//! centralized control over how this event deserializes it's internal data.
	//! For our purposes we'll just leave it blank but this is super useful for
	//! internet applications as it uncouples the need for your application to
	//! know the details of your connection to the internet.
	virtual void deSerialize( const ci::Buffer &streamIn ) {}
	
	//! Getter for position.
	ci::vec2 getPosition() { return mPosition; }
	//! Setter for position.
	void setPosition( ci::ivec2 position ) { mPosition = position; }
	
private:
	//! Position is the only data member we need for our purposes.
	explicit MousePositionEvent( ci::ivec2 position );
	//! It's sometimes useful to have a default constructor, like in cases
	//! where you'll want to "deSerialize" the data into this object.
	MousePositionEvent();
	
	// our useful info
	ci::ivec2 mPosition;
};

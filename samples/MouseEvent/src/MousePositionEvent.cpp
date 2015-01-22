//
//  MouseEvent.cpp
//  MouseEvent
//
//  Created by Ryan Bartley on 1/20/15.
//
//

#include "MousePositionEvent.h"
#include "cinder/app/App.h"

#include <boost/functional/hash.hpp>

// This is how I get a unique id. Normally, I'd statically create this
// in a common file and use it for each Event that I had, since we only
// have one event though, that would be overkill.
boost::hash<std::string> hasher;

// Then I use the Event Class Name as the input to the hash, since it is
// required for that name to be globally unique by C++.
EventType MousePositionEvent::TYPE = hasher( "MousePositionEvent" );

// This is our specialized creator that takes a position and
// initializes our position data member. It also uses Cinder
// to get the current timestamp
MousePositionEvent::MousePositionEvent( ci::ivec2 position )
: EventData( ci::app::App::get()->getElapsedSeconds() ), mPosition( position )
{
}

// This is our default that still uses Cinder to get the current
// timestamp. This may or may not be useful.
MousePositionEvent::MousePositionEvent()
: EventData( ci::app::App::get()->getElapsedSeconds() )
{
}

MousePositionEventRef MousePositionEvent::create( ci::ivec2 position )
{
	return MousePositionEventRef( new MousePositionEvent( position ) );
}

MousePositionEventRef MousePositionEvent::create()
{
	return MousePositionEventRef( new MousePositionEvent() );
}
//
//  Circle.cpp
//  MouseEvent
//
//  Created by Ryan Bartley on 1/20/15.
//
//

#include "Circle.h"

#include "cinder/Rand.h"
#include "cinder/app/App.h"
#include "cinder/Timeline.h"

#include "FastDelegate.h"
#include "EventManager.h"
#include "MousePositionEvent.h"

using namespace ci;
using namespace std;

static int constructAccum = 0;
static int moveAccum = 0;
static int copyAccum = 0;

Circle::Circle( const ci::vec2 &position, float radius )
: mColor( ci::ColorA( 0.0f, randFloat(0.15f, 1.0f), randFloat(0.15f, 1.0f), 1.0f ) ),
	mPosition( position ), mRadius( radius ), mIsActivated( false )
{
	// We need to add our instance's delegate to the event manager here.
	cout << "I'm adding " << constructAccum++ << " to the eventManager from constructor" << endl;
	initializeListener();
}

// implementation of copy.
Circle::Circle( const Circle &other )
: mColor( other.mColor ), mPosition( other.mPosition ),
	mRadius( other.mRadius ), mIsActivated( other.mIsActivated )
{
	// Just as in the constructor we need to add this instance's delegate to the event
	// manager's listener list.
	cout << "I'm adding " << copyAccum++ << " to the eventManager from copy" << endl;
	initializeListener();
}

Circle::Circle( Circle &&other )
: mColor( std::move( other.mColor ) ), mPosition( std::move( other.mPosition ) ),
	mRadius( other.mRadius ), mIsActivated( other.mIsActivated )
{
	cout << "I'm adding " << moveAccum++ << " to the eventManager from move" << endl;
	other.uninitializeListener();
	// Just as in the constructor we need to add this instance's delegate to the event
	// manager's listener list.
	initializeListener();
}

Circle::~Circle()
{
	// Because we've added this instance as a listener to the event manager. We now need
	// to remove this as a listener.
	uninitializeListener();
}

void Circle::initializeListener()
{
	// Here's the magic. When a circle is constructed. It immediately adds it's delegate
	// to the list of listeners. Get the global eventManager for this.
	auto eventManager = EventManager::get();
	// check to make sure it's constructed.
	
	if( eventManager ) {
		
		// create a fast delegate. Fast delegate is similar to a std::function in that it
		// is a callable entity. It's an "impossibly fast delegate" functor. If you're
		// interested beyond that check out...
		// http://www.codeproject.com/Articles/11015/The-Impossibly-Fast-C-Delegates
		auto thisListenerDelegate = fastdelegate::MakeDelegate( this, &Circle::mouseEventDelegate );
		
		// then add the delegate and what Type it's listening to to the eventManager
		eventManager->addListener( thisListenerDelegate, MousePositionEvent::TYPE );
		
		// that's basically it. Internally, any time an event of MouseEvent::TYPE is either
		// queued or triggered, this instance's Circle::mouseEventDelegate function will be
		// called
	}
}

void Circle::uninitializeListener()
{
	auto eventManager = EventManager::get();
	if( eventManager ) {
		auto thisListenerDelegate = fastdelegate::MakeDelegate( this, &Circle::mouseEventDelegate );
		// just as we call add we call remove.
		eventManager->removeListener( thisListenerDelegate, MousePositionEvent::TYPE );
	}
}

void Circle::update()
{
	if( ! mIsActivated ) {
		mPosition.x += randFloat(-1.0f, 1.0f);
		mPosition.y += randFloat(-1.0f, 1.0f);
	}
}

void Circle::draw()
{
	gl::color( mColor );
	gl::drawSolidCircle( mPosition, mRadius );
}

void Circle::mouseEventDelegate( EventDataRef eventData )
{
	// First, we should exit if we're already activated.
	if( mIsActivated ) return;
	
	// if we've made it to this function, then a MouseEvent must have been queued or
	// triggered as above. So we can pretty safely dynamic_cast the shared_ptr.
	auto mouseEvent = std::dynamic_pointer_cast<MousePositionEvent>( eventData );
	
	// "Pretty safely", because if you're working with others, they might have screwed
	// something up because you would never screw things up. So, you might just want to
	// check whether the pointer is correct...
	// if( ! mouseEvent ) return;
	
	// now you can get the event's data and work with it.
	auto pos = mouseEvent->getPosition();
	
	if( pos.x < mPosition.x + mRadius && pos.x > mPosition.x - mRadius &&
	   pos.y < mPosition.y + mRadius && pos.y > mPosition.y - mRadius ) {
		cout << "I picked a circle" << endl;
		cout << "MousePosition: " << pos << " Position: " << mPosition << " Radius: " << mRadius << endl;
		activate();
		// We don't want this event to continue so we're going to mark it as handled.
		// This'll deactivate the event in the eventManager.
		mouseEvent->setIsHandled( true );
	}
}

void Circle::activate()
{
	auto app = app::App::get();
	auto & timeline = app->timeline();
	auto color = mColor;
	timeline.applyPtr( &mColor, ci::ColorAf( 1, 0, 0, 1 ), 1.0f ).finishFn( [&, color](){
		auto & timeline = app::App::get()->timeline();
		timeline.applyPtr( &mColor, color, 1.0f ).finishFn( [&]() { mIsActivated = false; } );
	});
	mIsActivated = true;
}


















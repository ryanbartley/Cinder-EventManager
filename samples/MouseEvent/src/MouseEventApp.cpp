#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/Rand.h"

#include "EventManager.h"
#include "MousePositionEvent.h"
#include "Circle.h"

using namespace ci;
using namespace ci::app;
using namespace std;

const int NUM_CIRCLES = 100;

class MouseEventApp : public AppNative {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
	//! when we want to shutdown, we should cleanup
	//! in an order that makes sense.
	void shutdown() override;
	
	//! Create a vector of circles
	std::vector<Circle> mCircles;
	EventManagerRef		mEventManager;
};

void MouseEventApp::setup()
{
	// we first initialize the eventManager that we'll be using, Give it a name
	// and we'll be making this global so I'm passing it true.
	mEventManager = EventManager::create( "Global", true );
	// I know the number of Circles that i have and I want to use move semantics
	// so I first reserve space for that number. If you were to remove this line
	// you'd see that the Circles Copy Constructor is called, because of the
	// memory reallocation.
	mCircles.reserve( NUM_CIRCLES );
	// Then I'll loop through and create each circle. Inside the circles
	// constructor it'll attach a listener, then it'll use move semantics to
	// push itself on to the vector. Check out Circle.h for more explanation
	// on this.
	for ( int i = 0; i < NUM_CIRCLES; i++ ) {
		mCircles.push_back( Circle( vec2( randFloat( 0.0f, getWindowWidth() ),
										  randFloat( 0.0f, getWindowHeight() ) ),
									randFloat( 1.0f, 10.0f ) ) );
	}
}

void MouseEventApp::mouseDown( MouseEvent event )
{
	// Here's where the event will be queued. It'll create a new MousePositionEvent
	// then it'll queue that event with the Event Manager.
	mEventManager->queueEvent( MousePositionEvent::create( event.getPos() ) );
}

void MouseEventApp::update()
{
	// in my update I'll first update the event manager which if an event is queued
	// it will trigger for all listeners
	mEventManager->update();
	// next i update my circles
	for( auto & circle : mCircles ) {
		circle.update();
	}
}

void MouseEventApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
	// draw them normally.
	for( auto & circle : mCircles ) {
		circle.draw();
	}
}

void MouseEventApp::shutdown()
{
	// first we'll destroy all of the circles. This is so we
	// remove our listeners from the event manager before we
	// remove our event manager.
	mCircles.clear();
	// now that we can rest assured no one will call remove
	// on the event manager, we can destroy it.
	mEventManager.reset();
}

CINDER_APP_NATIVE( MouseEventApp, RendererGl )

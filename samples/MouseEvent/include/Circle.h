//
//  Circle.h
//  MouseEvent
//
//  Created by Ryan Bartley on 1/20/15.
//
//

#pragma once

// forward declaration
using EventDataRef = std::shared_ptr<class EventData>;
using CircleRef = std::shared_ptr<class Circle>;

// This is just a simple class to show the event firings
class Circle {
public:
	//! The constructor is where we'll add the Event listener.
	Circle( const ci::vec2 &position, float radius );
	
	// Much of the other constructors below are if you don't create
	// shared_ptr's to these circles. If you do use shared_ptr's, then
	// instance will travel around automatically.
	
	//! Because we may need to copy and as you'll see in the
	//! implementation we'll be using resources that rely on
	//! the specific "this" pointer, we need to implement a
	//! reliable copy constructor.
	Circle( const Circle &other );
	//! This is the move constructor which is most likely in our
	//! example what the compiler will choose as the best way to
	//! construct our vector. For understanding on rvalue references
	//! and move semantics take a look here...
	//! http://thbecker.net/articles/rvalue_references/section_01.html
	Circle( Circle &&other );
	//! Because we may need to destruct a "Circle" and as you'll
	//! see in the implementation we'll be using resources that
	//! rely on the specific "this" pointer, we need to implement
	//! a reliable destructor.
	~Circle();
	
	//! simple update function
	void update();
	//! simple draw function
	void draw();
	
	//! This is the function we're most interested in. Check the
	//! definition to see how it works.
	void mouseEventDelegate( EventDataRef eventData );
	
private:
	//! activation function for the event.
	void activate();
	
	//! This function implements the "hook-in" to the event manager.
	void initializeListener();
	//! This function implements the "un-hook" of the event manager.
	void uninitializeListener();
	
	ci::ColorAf	mColor;
	ci::vec2	mPosition;
	float		mRadius;
	bool		mIsActivated;
};

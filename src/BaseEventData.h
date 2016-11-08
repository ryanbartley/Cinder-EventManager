//
//  EventDataBase.h
//  RendererTest
//
//  Created by Ryan Bartley on 8/16/14.
//
//

#pragma once
#pragma warning( push )
#pragma warning( disable : 4068 )
/* The classes below are exported */
#pragma GCC visibility push(default)
#pragma warning( pop )

#include <memory>

namespace cinder {
	class Buffer;
}

using EventDataRef = std::shared_ptr<class EventData>;
using EventType = uint64_t;
	
class EventData {
public:
	explicit EventData( float timestamp = 0.0f ) : mTimeStamp( timestamp ), mIsHandled( false ) {}
	virtual ~EventData() {}

	virtual EventDataRef copy() = 0;
	virtual const char* getName() const = 0;
	virtual EventType getTypeId() const = 0;
	float getTimeStamp() { return mTimeStamp; }
	
	bool isHandled() { return mIsHandled; }
	void setIsHandled( bool handled = true ) { mIsHandled = handled; }
	
	virtual void serialize( cinder::Buffer &streamOut ) = 0;
	virtual void deSerialize( const cinder::Buffer &streamIn ) = 0;
	
private:
	const float mTimeStamp;
	bool		mIsHandled;
};

#pragma warning( push )
#pragma warning( disable : 4068 )
#pragma GCC visibility pop
#pragma warning( pop )
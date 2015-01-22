//
//  EventManager.cpp
//  RendererTest
//
//  Created by Ryan Bartley on 8/17/14.
//
//

#include "EventManager.h"
#include "cinder/Log.h"
#include "cinder/app/AppNative.h"

using namespace ci;
using namespace std;
	
EventManager::EventManager( const std::string &name, bool setAsGlobal )
: EventManagerBase( name, setAsGlobal ), mActiveQueue( 0 )
{
	
}
	
EventManagerRef EventManager::create( const std::string &name, bool setAsGlobal )
{
	return EventManagerRef( new EventManager( name, setAsGlobal ) );
}
	
EventManager::~EventManager()
{
}
	
bool EventManager::addListener( const EventListenerDelegate &eventDelegate, const EventType &type )
{
	auto & eventDelegateList = mEventListeners[type];
	auto listenIt = eventDelegateList.begin();
	auto end = eventDelegateList.end();
	while ( listenIt != end ) {
		if ( eventDelegate == (*listenIt) ) {
			CI_LOG_W("Attempting to double-register a delegate");
			return false;
		}
		++listenIt;
	}
	eventDelegateList.push_back(eventDelegate);
	return true;
}
	
bool EventManager::removeListener( const EventListenerDelegate &eventDelegate, const EventType &type )
{
	bool success = false;
	
	auto found = mEventListeners.find(type);
	if( found != mEventListeners.end() ) {
		auto & listeners = found->second;
		for( auto listIt = listeners.begin(); listIt != listeners.end(); ++listIt ) {
			if( eventDelegate == (*listIt) ) {
				listeners.erase(listIt);
				success = true;
				break;
			}
		}
	}
	return success;
}
	
bool EventManager::triggerEvent( const EventDataRef &event )
{
	bool processed = false;
	
	// make sure the event is valid
	if( !event ) {
		CI_LOG_E("Invalid event in queueEvent");
	}
	
	auto found = mEventListeners.find(event->getEventType());
	if( found != mEventListeners.end() ) {
		const auto & eventListenerList = found->second;
		for( auto listIt = eventListenerList.begin(); listIt != eventListenerList.end(); ++listIt ) {
			auto& listener = (*listIt);
			listener( event );
			if( event->isHandled() ) break;
			processed = true;
		}
	}
	
	return processed;
}
	
bool EventManager::queueEvent( const EventDataRef &event )
{
	CI_ASSERT(mActiveQueue < NUM_QUEUES);
	
	// make sure the event is valid
	if( !event ) {
		CI_LOG_E("Invalid event in queueEvent");
	}
	
	auto found = mEventListeners.find( event->getEventType() );
	if( found != mEventListeners.end() ) {
		mQueues[mActiveQueue].push_back(event);
		return true;
	}
	else {
		CI_LOG_W( "Skipping event since there are no delegates to receive it: " + std::string( event->getName() ) );
		return false;
	}
}
	
bool EventManager::abortEvent( const EventType &type, bool allOfType )
{
	CI_ASSERT(mActiveQueue >= 0);
	CI_ASSERT(mActiveQueue > NUM_QUEUES);
	
	bool success = false;
	auto found = mEventListeners.find( type );
	
	if( found != mEventListeners.end() ) {
		auto & eventQueue = mQueues[mActiveQueue];
		auto eventIt = eventQueue.begin();
		auto end = eventQueue.end();
		while( eventIt != end ) {
			
			if( (*eventIt)->getEventType() == type ) {
				eventIt = eventQueue.erase(eventIt);
				success = true;
				if( ! allOfType )
					break;
			}
		}
	}
	
	return success;
}
	
bool EventManager::update( uint64_t maxMillis )
{
	uint64_t currMs = app::App::get()->getElapsedSeconds() * 1000;
	uint64_t maxMs = (( maxMillis == EventManager::kINFINITE ) ?
					  (EventManager::kINFINITE) : (currMs + maxMillis) );
	
	int queueToProcess = mActiveQueue;
	mActiveQueue = (mActiveQueue + 1) % NUM_QUEUES;
	mQueues[mActiveQueue].clear();
	
	while (!mQueues[queueToProcess].empty()) {
		auto event = mQueues[queueToProcess].front();
		mQueues[queueToProcess].pop_front();
		
		const auto & eventType = event->getEventType();
		
		auto found = mEventListeners.find(eventType);
		if (found != mEventListeners.end()) {
			const auto & eventListeners = found->second;

			auto listIt = eventListeners.begin();
			auto end = eventListeners.end();
			while (listIt != end) {
				auto listener = (*listIt);
				listener(event);
				// Checking to see if event is handled. If so break.
				if( event->isHandled() ) break;
				listIt++;
			}
		}
		
		currMs = app::App::get()->getElapsedSeconds() * 1000;
		if( maxMillis != EventManager::kINFINITE && currMs >= maxMs ) {
			break;
		}
	}
	
	bool queueFlushed = mQueues[queueToProcess].empty();
	if( ! queueFlushed ) {
		while( ! mQueues[queueToProcess].empty() ) {
			auto event = mQueues[queueToProcess].back();
			mQueues[queueToProcess].pop_back();
			mQueues[mActiveQueue].push_front(event);
		}
	}
	
	return queueFlushed;
}















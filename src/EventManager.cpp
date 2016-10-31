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

#include "EventManager.h"
#include <iostream>
#include <assert.h>

//#define LOG_EVENT( stream )	CI_LOG_I( stream )
#define LOG_EVENT( stream )	((void)0)

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
	std::cout << "Cleaning up event manager" << std::endl;
	mEventListeners.clear();
	mQueues[0].clear();
	mQueues[1].clear();
	std::cout << "Removing all threaded events" << std::endl;
	std::lock_guard<std::mutex> lock( mThreadedEventListenerMutex );
	mThreadedEventListeners.clear();
	std::cout << "Removed ALL EVENT LISTENERS" << std::endl;
}
	
bool EventManager::addListener( const EventListenerDelegate &eventDelegate, const EventType &type )
{
	std::cout << "Attempting to add delegate function for event type: " + to_string( type ) << std::endl;
	
	auto & eventDelegateList = mEventListeners[type];
	auto listenIt = eventDelegateList.begin();
	auto end = eventDelegateList.end();
	while ( listenIt != end ) {
		if ( eventDelegate == (*listenIt) ) {
			std::cout << "Attempting to double-register a delegate" << std::endl;
			return false;
		}
		++listenIt;
	}
	eventDelegateList.push_back(eventDelegate);
	//CI_LOG_V("Successfully added delegate for event type: " + to_string( type ) );
	return true;
}
	
bool EventManager::removeListener( const EventListenerDelegate &eventDelegate, const EventType &type )
{
	LOG_EVENT("Attempting to remove delegate function from event type: " + to_string( type ) );
	bool success = false;
	
	auto found = mEventListeners.find(type);
	if( found != mEventListeners.end() ) {
		auto & listeners = found->second;
		for( auto listIt = listeners.begin(); listIt != listeners.end(); ++listIt ) {
			if( eventDelegate == (*listIt) ) {
				listeners.erase(listIt);
				LOG_EVENT("Successfully removed delegate function from event type: ");
				success = true;
				break;
			}
		}
	}
	return success;
}
	
bool EventManager::triggerEvent( const EventDataRef &event )
{
	//LOG_EVENT("Attempting to trigger event: " + std::string( event->getName() ) );
	bool processed = false;
	
	auto found = mEventListeners.find(event->getTypeId());
	if( found != mEventListeners.end() ) {
		const auto & eventListenerList = found->second;
		for( auto listIt = eventListenerList.begin(); listIt != eventListenerList.end(); ++listIt ) {
			auto& listener = (*listIt);
			//LOG_EVENT("Sending event " + std::string( event->getName() ) + " to delegate.");
			listener( event );
			processed = true;
		}
	}
	
	return processed;
}
	
bool EventManager::queueEvent( const EventDataRef &event )
{
	assert(mActiveQueue < NUM_QUEUES);
	
	// make sure the event is valid
	if( !event ) {
		LOG_EVENT("Invalid event in queueEvent");
	}
	
//	CI_LOG_V("Attempting to queue event: " + std::string( event->getName() ) );
	
	auto found = mEventListeners.find( event->getTypeId() );
	if( found != mEventListeners.end() ) {
		mQueues[mActiveQueue].push_back(event);
		LOG_EVENT("Successfully queued event: " + std::string( event->getName() ) );
		return true;
	}
	else {
		static bool processNotify = false;
		if( !processNotify ) {
			LOG_EVENT( "Skipping event since there are no delegates to receive it: " + std::string( event->getName() ) );
			processNotify = true;
		}
		return false;
	}
}
	
bool EventManager::abortEvent( const EventType &type, bool allOfType )
{
	assert(mActiveQueue >= 0);
	assert(mActiveQueue > NUM_QUEUES);
	
	bool success = false;
	auto found = mEventListeners.find( type );
	
	if( found != mEventListeners.end() ) {
		auto & eventQueue = mQueues[mActiveQueue];
		auto eventIt = eventQueue.begin();
		auto end = eventQueue.end();
		while( eventIt != end ) {
			
			if( (*eventIt)->getTypeId() == type ) {
				eventIt = eventQueue.erase(eventIt);
				success = true;
				if( ! allOfType )
					break;
			}
		}
	}
	
	return success;
}
	
bool EventManager::addThreadedListener( const EventListenerDelegate &eventDelegate, const EventType &type )
{
	std::lock_guard<std::mutex> lock( mThreadedEventListenerMutex );
	
	auto & eventDelegateList = mThreadedEventListeners[type];
	for ( auto & delegate : eventDelegateList ) {
		if ( eventDelegate == delegate ) {
			LOG_EVENT("Attempting to double-register a delegate");
			return false;
		}
	}
	eventDelegateList.push_back(eventDelegate);
	//CI_LOG_V("Successfully added delegate for event type: " + to_string( type ) );
	return true;
}

bool EventManager::removeThreadedListener( const EventListenerDelegate &eventDelegate, const EventType &type )
{
	std::lock_guard<std::mutex> lock( mThreadedEventListenerMutex );
	
	auto found = mThreadedEventListeners.find(type);
	if( found != mThreadedEventListeners.end() ) {
		auto & listeners = found->second;
		for( auto listIt = listeners.begin(); listIt != listeners.end(); ++listIt ) {
			if( eventDelegate == (*listIt) ) {
				listeners.erase(listIt);
				LOG_EVENT("Successfully removed delegate function from event type: " << to_string( type ) );
				return true;
			}
		}
	}
	return false;
}

void EventManager::removeAllThreadedListeners()
{
	std::lock_guard<std::mutex> lock( mThreadedEventListenerMutex );
	mThreadedEventListeners.clear();
}

bool EventManager::triggerThreadedEvent( const EventDataRef &event )
{
	std::lock_guard<std::mutex> lock( mThreadedEventListenerMutex );
	
	bool processed = false;
	auto found = mThreadedEventListeners.find(event->getTypeId());
	if( found != mThreadedEventListeners.end() ) {
		const auto & eventListenerList = found->second;
		for( auto & listener : eventListenerList ) {
			listener( event );
			processed = true;
		}
	}
	if( ! processed )
		LOG_EVENT( "Tried triggering MultiThreaded Event without a listener" );
	return processed;
}
	
bool EventManager::update( uint64_t maxMillis )
{
	static uint64_t currMs = 0;
	currMs = (1.0 / 60.0) * 1000;
	uint64_t maxMs = (( maxMillis == EventManager::kINFINITE ) ? (EventManager::kINFINITE) : (currMs + maxMillis) );
	
	int queueToProcess = mActiveQueue;
	mActiveQueue = (mActiveQueue + 1) % NUM_QUEUES;
	mQueues[mActiveQueue].clear();
	
	static bool processNotify = false;
	if( ! processNotify ) {
		LOG_EVENT("Processing Event Queue " + to_string(queueToProcess) + "; " + to_string(mQueues[queueToProcess].size()) + " events to process");
		processNotify = true;
	}
	
	while (!mQueues[queueToProcess].empty()) {
		auto event = mQueues[queueToProcess].front();
		mQueues[queueToProcess].pop_front();
		LOG_EVENT("\t\tProcessing Event " + std::string(event->getName()));
		
		const auto & eventType = event->getTypeId();
		
		auto found = mEventListeners.find(eventType);
		if (found != mEventListeners.end()) {
			const auto & eventListeners = found->second;
			LOG_EVENT("\t\tFound " + to_string(eventListeners.size()) + " delegates");

			auto listIt = eventListeners.begin();
			auto end = eventListeners.end();
			while (listIt != end) {
				auto listener = (*listIt);
				LOG_EVENT("\t\tSending Event " + std::string(event->getName()) + " to delegate");
				listener(event);
				listIt++;
			}
		}
		
//		currMs = app::App::get()->getElapsedSeconds() * 1000;//Engine::getTickCount();
		if( maxMillis != EventManager::kINFINITE && currMs >= maxMs ) {
			LOG_EVENT("Aborting event processing; time ran out");
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
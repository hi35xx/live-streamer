/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * EvUsageEnvironment.cpp
 * Copyright (C) 2015 Watson Xu <watson@localhost.localdomain>
 *
 * live-streamer is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * live-streamer is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "EvUsageEnvironment.h"


EvTaskScheduler::DelayedTask::DelayedTask
(ev::loop_ref& loop, uint64_t usec, TaskFunc* proc, void* data)
 : timer(loop), fCallback(proc), fData(data)
{
	set(usec / 1000000.0, 0.0);
	do { fToken = ++tokenCounter; } while (fToken == 0);
}


EvTaskScheduler::BackgroundHandling::BackgroundHandling
(ev::loop_ref& loop, int fd, int events, BackgroundHandlerProc* proc, void* data)
 : io(loop), fCallback(proc), fData(data)
{
	set(fd, events);
}


EvTaskScheduler::EventTrigger::EventTrigger
(ev::loop_ref& loop, TaskFunc *proc)
 : async(loop), fCallback(proc), fData(NULL)
{
	do { fTriggerId = ++triggerCounter; } while (fTriggerId == 0);
}

EvTaskScheduler* EvTaskScheduler::createNew(ev::default_loop& loop)
{
	return new EvTaskScheduler(loop);
}

EvTaskScheduler::EvTaskScheduler(ev::default_loop& loop)
  : fMainLoop(loop)
{
}

EvTaskScheduler::~EvTaskScheduler()
{
}

void EvTaskScheduler::delayed_task_thunk(ev::timer& w, int revents)
{
	DelayedTask* task = static_cast<DelayedTask*>(&w);

	// after call the callback, this task may be unscheduled.
	TaskToken token = task->token();

	task->fCallback(task->fData);

	fDelayedTaskMap.erase(token);
}

long EvTaskScheduler::DelayedTask::tokenCounter = 0;

TaskToken EvTaskScheduler::scheduleDelayedTask(int64_t microseconds,
                                               TaskFunc* proc,
                                               void* clientData)
{
	DelayedTask *task = new DelayedTask(fMainLoop, microseconds, proc, clientData);

	task->set<EvTaskScheduler, &EvTaskScheduler::delayed_task_thunk>(this);
	task->start();

	fDelayedTaskMap.emplace(task->token(), task);

	return task->token();
}

void EvTaskScheduler::unscheduleDelayedTask(TaskToken& prevTask)
{
	if (prevTask == NULL) return;

	fDelayedTaskMap.erase(prevTask);

	prevTask = NULL;
}

void EvTaskScheduler::background_handling_thunk(ev::io& w, int revents)
{
	BackgroundHandling* iow = static_cast<BackgroundHandling*>(&w);
	int mask = 0;

	if (revents & ev::READ) mask |= SOCKET_READABLE;
	if (revents & ev::WRITE) mask |= SOCKET_WRITABLE;
	if (revents & ev::ERROR) mask |= SOCKET_EXCEPTION;

	iow->fCallback(iow->fData, mask);
}

void EvTaskScheduler::setBackgroundHandling(int socketNum, int conditionSet,
                                            BackgroundHandlerProc* handlerProc,
                                            void* clientData)
{
	if (socketNum < 0)
		return;

	if (conditionSet == 0) {
		fBackgroundHandlingMap.erase(socketNum);
	}
	else {
		BackgroundHandlingMap::iterator it = fBackgroundHandlingMap.find(socketNum);
		int events = 0;

		if (conditionSet & SOCKET_READABLE) events |= ev::READ;
		if (conditionSet & SOCKET_WRITABLE) events |= ev::WRITE;

		if (it != fBackgroundHandlingMap.end()) {
			BackgroundHandling* iow;

			fBackgroundHandlingMap.erase(socketNum);

			iow = new BackgroundHandling(fMainLoop, socketNum, events,
			                             handlerProc, clientData);
			iow->set<EvTaskScheduler, &EvTaskScheduler::background_handling_thunk>(this);
			iow->start();
			fBackgroundHandlingMap.emplace(socketNum, iow);
		}
		else {
			BackgroundHandling *iow;
			iow = new BackgroundHandling(fMainLoop, socketNum, events,
			                             handlerProc, clientData);
			iow->set<EvTaskScheduler, &EvTaskScheduler::background_handling_thunk>(this);
			iow->start();
			fBackgroundHandlingMap.emplace(socketNum, iow);
		}
	}
}

void EvTaskScheduler::moveSocketHandling(int oldSocketNum, int newSocketNum)
{
	if (oldSocketNum < 0 || newSocketNum < 0) return;

	BackgroundHandlingMap::iterator it = fBackgroundHandlingMap.find(oldSocketNum);
	if (it != fBackgroundHandlingMap.end()) {
		auto iow = std::unique_ptr<BackgroundHandling>(std::move(it->second));
		iow->stop();
		fBackgroundHandlingMap.erase(it);
		iow->set(newSocketNum, iow->events);
		iow->start();
		fBackgroundHandlingMap.insert(std::make_pair(newSocketNum, std::move(iow)));
	}
}

extern bool _terminated;

void EvTaskScheduler::doEventLoop(char volatile* watchVariable)
{
	while (True) {
		fMainLoop.run(ev::ONCE);
		if (*watchVariable || _terminated)
			break;
	}
}

void EvTaskScheduler::event_trigger_thunk(ev::async& w, int revents)
{
	EventTrigger* et = static_cast<EventTrigger*>(&w);

	if (et->fData) {
		et->fCallback(et->fData);
	}
}

uint32_t EvTaskScheduler::EventTrigger::triggerCounter = 0;

EventTriggerId EvTaskScheduler::createEventTrigger(TaskFunc* eventHandlerProc)
{
	EventTrigger *et = new EventTrigger(fMainLoop, eventHandlerProc);

	et->set<EvTaskScheduler, &EvTaskScheduler::event_trigger_thunk>(this);
	et->start();

	fEventTriggerMap.emplace(et->triggerId(), et);

	return et->triggerId();
}

void EvTaskScheduler::deleteEventTrigger(EventTriggerId eventTriggerId)
{
	fEventTriggerMap.erase(eventTriggerId);
}

void EvTaskScheduler::triggerEvent(EventTriggerId eventTriggerId, void* clientData)
{
	EventTriggerMap::iterator it = fEventTriggerMap.find(eventTriggerId);
	if (it != fEventTriggerMap.end()) {
		EventTriggerPtr &et = it->second;
		et->setClientData(clientData);
		et->send();
	}
}

void EvTaskScheduler::internalError()
{
	abort();
}

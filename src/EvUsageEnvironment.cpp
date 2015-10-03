/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*-  */
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
    fSocketMap.clear();
    fDelayedTaskMap.clear();
}

void EvTaskScheduler::delayed_task_thunk(ev::timer& w, int revents)
{
    DelayedTaskWatcher* tw = static_cast<DelayedTaskWatcher*>(&w);

    // after call the callback, this task may be unscheduled.
    TaskToken token = tw->token();

    tw->fCallback(tw->fData);

    fDelayedTaskMap.erase(token);
}

long EvTaskScheduler::DelayedTaskWatcher::tokenCounter = 0;

TaskToken EvTaskScheduler::scheduleDelayedTask(int64_t microseconds,
                                               TaskFunc* proc,
                                               void* clientData)
{
    DelayedTaskWatcher *tw = new DelayedTaskWatcher(fMainLoop);

    tw->set(microseconds / 1000000.0, 0.0);
    tw->set<EvTaskScheduler, &EvTaskScheduler::delayed_task_thunk>(this);
    tw->setHandler(proc, clientData);
    tw->start();

    fDelayedTaskMap.emplace(tw->token(), std::unique_ptr<DelayedTaskWatcher>(tw));

    return tw->token();
}

void EvTaskScheduler::unscheduleDelayedTask(TaskToken& prevTask)
{
    if (prevTask == NULL) return;

    fDelayedTaskMap.erase(prevTask);

    prevTask = NULL;
}

void EvTaskScheduler::socket_io_thunk(ev::io& w, int revents)
{
    SocketIoWatcher* iow = static_cast<SocketIoWatcher*>(&w);
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
        fSocketMap.erase(socketNum);
    }
    else {
        SocketIoMap::iterator it = fSocketMap.find(socketNum);
        int events = 0;

        if (conditionSet & SOCKET_READABLE) events |= ev::READ;
        if (conditionSet & SOCKET_WRITABLE) events |= ev::WRITE;

        if (it != fSocketMap.end()) {
            std::unique_ptr<SocketIoWatcher> &iow = it->second;
            iow->stop();
            iow->set(socketNum, events);
            iow->setHandler(handlerProc, clientData);
            iow->start();
        }
        else {
            SocketIoWatcher *iow = new SocketIoWatcher(fMainLoop);
            iow->set<EvTaskScheduler, &EvTaskScheduler::socket_io_thunk>(this);
            iow->set(socketNum, events);
            iow->setHandler(handlerProc, clientData);
            iow->start();
            fSocketMap.emplace(socketNum, std::unique_ptr<SocketIoWatcher>(iow));
        }
    }
}

void EvTaskScheduler::moveSocketHandling(int oldSocketNum, int newSocketNum)
{
    if (oldSocketNum < 0 || newSocketNum < 0) return;

    SocketIoMap::iterator it = fSocketMap.find(oldSocketNum);
    if (it != fSocketMap.end()) {
        auto iow = std::unique_ptr<SocketIoWatcher>(std::move(it->second));
        iow->stop();
        fSocketMap.erase(it);
        iow->set(newSocketNum, iow->events);
        iow->start();
        fSocketMap.insert(std::make_pair(newSocketNum, std::move(iow)));
    }
}

void EvTaskScheduler::doEventLoop(char volatile* watchVariable)
{
    while (True) {
        fMainLoop.run(ev::ONCE);
        if (*watchVariable)
            break;
    }
}

void EvTaskScheduler::event_trigger_thunk(ev::async& w, int revents)
{
    EventTriggerWatcher* tw = static_cast<EventTriggerWatcher*>(&w);

    if (tw->fData) {
        tw->fCallback(tw->fData);
    }
}

uint32_t EvTaskScheduler::EventTriggerWatcher::triggerCounter = 0;

EventTriggerId EvTaskScheduler::createEventTrigger(TaskFunc* eventHandlerProc)
{
    EventTriggerWatcher *tw = new EventTriggerWatcher(fMainLoop);

    tw->set<EvTaskScheduler, &EvTaskScheduler::event_trigger_thunk>(this);
    tw->setHandler(eventHandlerProc);
    tw->start();

    fEventTriggerMap.emplace(tw->triggerId(), std::unique_ptr<EventTriggerWatcher>(tw));

    return tw->triggerId();
}

void EvTaskScheduler::deleteEventTrigger(EventTriggerId eventTriggerId)
{
    fEventTriggerMap.erase(eventTriggerId);
}

void EvTaskScheduler::triggerEvent(EventTriggerId eventTriggerId, void* clientData)
{
    EventTriggerMap::iterator it = fEventTriggerMap.find(eventTriggerId);
    if (it != fEventTriggerMap.end()) {
        std::unique_ptr<EventTriggerWatcher> &w = it->second;
        w->setClientData(clientData);
        w->send();
    }
}

void EvTaskScheduler::internalError()
{
    abort();
}

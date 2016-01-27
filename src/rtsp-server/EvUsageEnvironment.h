/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * EvUsageEnvironment.h
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

#ifndef __EV_USAGE_ENVIRONMENT_H__
#define __EV_USAGE_ENVIRONMENT_H__

#include <memory>
#include <unordered_set>
#include <unordered_map>
#include <ev++.h>
#include <HashTable.hh>
#include <UsageEnvironment.hh>

class EvTaskScheduler: public TaskScheduler
{
public:
    static EvTaskScheduler* createNew(ev::default_loop& loop);
    virtual ~EvTaskScheduler();
    virtual TaskToken scheduleDelayedTask(int64_t microseconds, TaskFunc* proc,
                                          void* clientData);
    virtual void unscheduleDelayedTask(TaskToken& prevTask);
    virtual void setBackgroundHandling(int socketNum, int conditionSet,
                                       BackgroundHandlerProc* handlerProc,
                                       void* clientData);
    virtual void moveSocketHandling(int oldSocketNum, int newSocketNum);
    virtual void doEventLoop(char volatile* watchVariable = NULL);
    virtual EventTriggerId createEventTrigger(TaskFunc* eventHandlerProc);
    virtual void deleteEventTrigger(EventTriggerId eventTriggerId);
    virtual void triggerEvent(EventTriggerId eventTriggerId, void* clientData = NULL);
    virtual void internalError();

protected:
    EvTaskScheduler(ev::default_loop& loop);

private:
    ev::loop_ref& fMainLoop;

    struct DelayedTaskWatcher : public ev::timer
    {
        DelayedTaskWatcher(ev::loop_ref& loop)
            : timer(loop), fCallback(NULL), fData(NULL)
        {
            do {
                fToken = ++tokenCounter;
            } while (fToken == 0);
        }
        void setHandler(TaskFunc* cb, void* data)
        {
            fCallback = cb;
            fData = data;
        }
        TaskToken token() { return (TaskToken)fToken; }
        TaskFunc* fCallback;
        void* fData;

        long fToken;
        static long tokenCounter;
    };
    struct SocketIoWatcher : public ev::io
    {
        SocketIoWatcher(ev::loop_ref& loop)
            : io(loop), fCallback(NULL), fData(NULL) {}
        void setHandler(BackgroundHandlerProc* cb, void* data)
        {
            fCallback = cb;
            fData = data;
        }
        BackgroundHandlerProc* fCallback;
        void* fData;
    };
    struct EventTriggerWatcher : public ev::async
    {
        EventTriggerWatcher(ev::loop_ref& loop)
            : async(loop), fCallback(NULL), fData(NULL)
        {
            do {
                fTriggerId = ++triggerCounter;
            } while (fTriggerId == 0);
        }
        void setHandler(TaskFunc* cb) { fCallback = cb; }
        void setClientData(void* clientData) { fData = clientData; }
        EventTriggerId triggerId() { return (EventTriggerId)fTriggerId; }
        TaskFunc* fCallback;
        void* fData;
        uint32_t fTriggerId;

        static uint32_t triggerCounter;
    };

    void delayed_task_thunk(ev::timer& w, int revents);
    void socket_io_thunk(ev::io& w, int revents);
    void event_trigger_thunk(ev::async& w, int revents);

    typedef std::unordered_map<int, std::unique_ptr<SocketIoWatcher>> SocketIoMap;
    typedef std::unordered_map<TaskToken, std::unique_ptr<DelayedTaskWatcher>> DelayedTaskMap;
    typedef std::unordered_map<EventTriggerId, std::unique_ptr<EventTriggerWatcher>> EventTriggerMap;
    SocketIoMap    fSocketMap;
    DelayedTaskMap fDelayedTaskMap;
    EventTriggerMap fEventTriggerMap;

};

#endif // __EV_USAGE_ENVIRONMENT_H__

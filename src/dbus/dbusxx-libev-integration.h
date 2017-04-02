/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 *
 *  D-Bus++ - C++ bindings for D-Bus
 *
 *  Copyright (C) 2015  Watson Xu <xuhuashan@gmail.com>
 *
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */


#ifndef __DBUSXX_LIBEV_INTEGRATION_H
#define __DBUSXX_LIBEV_INTEGRATION_H

#include <ev++.h>

#include <dbus-c++/api.h>
#include <dbus-c++/dispatcher.h>

namespace DBus
{

namespace Ev
{

class BusDispatcher;

class DXXAPI BusTimeout : public Timeout
{
private:

	BusTimeout(Timeout::Internal *, ev::loop_ref &, int);

	~BusTimeout();

	void toggle();

	void timeout_handler(ev::timer &, int);

	void _enable();

	void _disable();

private:

	ev::loop_ref &_loop;
	int _priority;
	ev::timer _timer;

	friend class BusDispatcher;
};

class DXXAPI BusWatch : public Watch
{
private:

	BusWatch(Watch::Internal *, ev::loop_ref &, int);

	~BusWatch();

	void toggle();

	void watch_handler(ev::io &, int);

	void _enable();

	void _disable();

private:

	ev::loop_ref &_loop;
	int _priority;
	ev::io _io;

	friend class BusDispatcher;
};

class DXXAPI BusDispatcher : public Dispatcher
{
public:

	BusDispatcher();
	~BusDispatcher();

	void attach(ev::loop_ref *);

	void enter() {}

	void leave() {}

	Timeout *add_timeout(Timeout::Internal *);

	void rem_timeout(Timeout *);

	Watch *add_watch(Watch::Internal *);

	void rem_watch(Watch *);

	void set_priority(int priority);

	void prepare_handler(ev::prepare &, int);
	void check_handler(ev::check &, int);

private:

	ev::loop_ref _loop;
	int _priority;
	ev::check _check;
	ev::prepare _prepare;
};

} /* namespace Ev */

} /* namespace DBus */

#endif//__DBUSXX_LIBEV_INTEGRATION_H

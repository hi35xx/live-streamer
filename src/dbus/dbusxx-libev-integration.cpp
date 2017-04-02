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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include "dbusxx-libev-integration.h"

#include <dbus/dbus.h> // for DBUS_WATCH_*

using namespace DBus;

Ev::BusTimeout::BusTimeout(Timeout::Internal *ti, ev::loop_ref &loop, int priority)
  : Timeout(ti), _loop(loop), _priority(priority), _timer(loop)
{
	if (Timeout::enabled())
		_enable();
}

Ev::BusTimeout::~BusTimeout()
{
	_disable();
}

void Ev::BusTimeout::toggle()
{
	debug_log("ev: timeout %p toggled (%s)", this, Timeout::enabled() ? "on" : "off");

	if (Timeout::enabled())
		_enable();
	else
		_disable();
}

void Ev::BusTimeout::timeout_handler(ev::timer &w, int revents)
{
	handle();
}

void Ev::BusTimeout::_enable()
{
	if (_timer.is_active())
		_disable(); // be sane

	ev_set_priority(static_cast<ev_timer*>(&_timer), _priority);
	_timer.set(Timeout::interval() / 1000.0, 0.0);
	_timer.set<BusTimeout, &BusTimeout::timeout_handler>(this);
	_timer.start();
}

void Ev::BusTimeout::_disable()
{
	if (_timer.is_active())
	{
		_timer.stop();
	}
}

Ev::BusWatch::BusWatch(Watch::Internal *wi, ev::loop_ref &loop, int priority)
  : Watch(wi), _loop(loop), _priority(priority), _io(loop)
{
	if (Watch::enabled())
		_enable();
}

Ev::BusWatch::~BusWatch()
{
	_disable();
}

void Ev::BusWatch::toggle()
{
	debug_log("ev: watch %p toggled (%s)", this, Watch::enabled() ? "on" : "off");

	if (Watch::enabled())
		_enable();
	else
		_disable();
}

void Ev::BusWatch::watch_handler(ev::io &w, int revents)
{
	int flags = 0;
	if (revents & ev::READ)
		flags |= DBUS_WATCH_READABLE;
	if (revents & ev::WRITE)
		flags |= DBUS_WATCH_WRITABLE;
	if (revents & ev::ERROR)
		flags |= DBUS_WATCH_ERROR;

	handle(flags);
}

void Ev::BusWatch::_enable()
{
	if (_io.is_active())
		_disable(); // be sane
	ev_set_priority(static_cast<ev_io*>(&_io), _priority);
	_io.set<BusWatch, &BusWatch::watch_handler>(this);

	int flags = Watch::flags();
	int condition = 0;

	if (flags & DBUS_WATCH_READABLE)
		condition |= ev::READ;
	if (flags & DBUS_WATCH_WRITABLE)
		condition |= ev::WRITE;
	if (flags & DBUS_WATCH_ERROR)
		condition |= ev::ERROR;
	if (flags & DBUS_WATCH_HANGUP)
		condition |= ev::ERROR;

	_io.set(Watch::descriptor(), condition);
	_io.start();
}

void Ev::BusWatch::_disable()
{
	if (!_io.is_active())
		return;

	_io.stop();
}


Ev::BusDispatcher::BusDispatcher()
  : _loop(NULL), _priority(0)
{
}

Ev::BusDispatcher::~BusDispatcher()
{
	if (_prepare.is_active())
		_prepare.stop();
	if (_check.is_active())
		_check.stop();
}

void Ev::BusDispatcher::prepare_handler(ev::prepare &w, int revents)
{
	if (has_something_to_dispatch())
		dispatch_pending();
}

void Ev::BusDispatcher::check_handler(ev::check &w, int revents)
{
	if (has_something_to_dispatch())
		dispatch_pending();
}

void Ev::BusDispatcher::attach(ev::loop_ref *loop)
{
	_loop = loop ? *loop : ev::get_default_loop();

#if EV_MULTIPLICITY
	_prepare.set(_loop);
	_check.set(_loop);
#endif
	_prepare.set<BusDispatcher, &BusDispatcher::prepare_handler>(this);
	_check.set<BusDispatcher, &BusDispatcher::check_handler>(this);
	_prepare.start();
	_check.start();
}

Timeout *Ev::BusDispatcher::add_timeout(Timeout::Internal *wi)
{
	Timeout *t = new Ev::BusTimeout(wi, _loop, _priority);

	debug_log("ev: added timeout %p (%s)", t, t->enabled() ? "on" : "off");

	return t;
}

void Ev::BusDispatcher::rem_timeout(Timeout *t)
{
	debug_log("ev: removed timeout %p", t);

	delete t;
}

Watch *Ev::BusDispatcher::add_watch(Watch::Internal *wi)
{
	Watch *w = new Ev::BusWatch(wi, _loop, _priority);

	debug_log("ev: added watch %p (%s) fd=%d flags=%d",
	          w, w->enabled() ? "on" : "off", w->descriptor(), w->flags()
	          );
	return w;
}

void Ev::BusDispatcher::rem_watch(Watch *w)
{
	debug_log("ev: removed watch %p", w);

	delete w;
}

void Ev::BusDispatcher::set_priority(int priority)
{
	_priority = priority;
}

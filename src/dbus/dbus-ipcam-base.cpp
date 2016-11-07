/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * dbus-ipcam-base.cpp
 * Copyright (C) 2016 Watson Xu <xuhuashan@gmail.com>
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <dbus/dbus.h>
#include <dbus-c++/dbus.h>
#include <ipcam-runtime.h>
#include "dbus-ipcam-base.h"

using namespace DBus;

IpcamBase::IpcamBase(IpcamRuntime &runtime, const Path &path)
  : ObjectAdaptor(runtime.dbus_conn(), path),
    _runtime(runtime)
{
}

void IpcamBase::on_get_property
(DBus::InterfaceAdaptor &interface, const std::string &property, DBus::Variant &value)
{
    value.clear();
	do_property_get(interface, property, value);
}

void IpcamBase::on_set_property
(DBus::InterfaceAdaptor &interface, const std::string &property, const DBus::Variant &value)
{
	do_property_set(interface, property, value);
}

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

#ifdef HAVE_JSONCPP_SUPPORT
#include <json/json.h>
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

IpcamBase::~IpcamBase()
{
}

#ifdef HAVE_JSONCPP_SUPPORT
static void load_iter_from_json
(DBusSignatureIter *type, MessageIter &iter, Json::Value &v)
{
	switch (dbus_signature_iter_get_current_type(type)) {
	case DBUS_TYPE_BOOLEAN:
		iter.append_bool(v.asBool());
		break;
	case DBUS_TYPE_BYTE:
		iter.append_byte(v.asUInt());
		break;
	case DBUS_TYPE_INT16:
		iter.append_int16(v.asInt());
		break;
	case DBUS_TYPE_UINT16:
		iter.append_uint16(v.asUInt());
		break;
	case DBUS_TYPE_INT32:
		iter.append_int32(v.asInt());
		break;
	case DBUS_TYPE_UINT32:
		iter.append_uint32(v.asUInt());
		break;
	case DBUS_TYPE_INT64:
		iter.append_int64(v.asInt64());
		break;
	case DBUS_TYPE_UINT64:
		iter.append_uint64(v.asUInt64());
		break;
	case DBUS_TYPE_DOUBLE:
		iter.append_double(v.asDouble());
		break;
	case DBUS_TYPE_STRING:
		iter.append_string(v.asString().c_str());
		break;
	case DBUS_TYPE_OBJECT_PATH:
		iter.append_path(v.asString().c_str());
		break;
	case DBUS_TYPE_SIGNATURE:
		iter.append_signature(v.asString().c_str());
		break;
	case DBUS_TYPE_ARRAY:
		{
			DBusSignatureIter subtype;
			dbus_signature_iter_recurse(type, &subtype);
			char *signature = dbus_signature_iter_get_signature(&subtype);
			MessageIter subiter = iter.new_array(signature);
			for (Json::Value::iterator vit = v.begin(); vit != v.end(); vit++) {
				load_iter_from_json(&subtype, subiter, *vit);
			}
			iter.close_container(subiter);
		}
		break;
	case DBUS_TYPE_STRUCT:
		{
			DBusSignatureIter subtype;
			dbus_signature_iter_recurse(type, &subtype);
			MessageIter subiter = iter.new_struct();
			int i = 0;
			do {
				load_iter_from_json(&subtype, subiter, v[i++]);
			} while (dbus_signature_iter_next(&subtype));
			iter.close_container(subiter);
		}
		break;
	case DBUS_TYPE_DICT_ENTRY:
	case DBUS_TYPE_VARIANT:
		break;
	default:
		break;
	}
}

static void save_iter_to_json(MessageIter &iter, Json::Value &v)
{
	switch (iter.type()) {
	case DBUS_TYPE_BYTE:
		v = iter.get_byte();
		break;
	case DBUS_TYPE_BOOLEAN:
		v = iter.get_bool();
		break;
	case DBUS_TYPE_INT16:
		v = iter.get_int16();
		break;
	case DBUS_TYPE_UINT16:
		v = iter.get_uint16();
		break;
	case DBUS_TYPE_INT32:
		v = iter.get_int32();
		break;
	case DBUS_TYPE_UINT32:
		v = iter.get_uint32();
		break;
	case DBUS_TYPE_INT64:
		v = iter.get_int64();
		break;
	case DBUS_TYPE_UINT64:
		v = iter.get_uint64();
		break;
	case DBUS_TYPE_DOUBLE:
		v = iter.get_double();
		break;
	case DBUS_TYPE_STRING:
	case DBUS_TYPE_OBJECT_PATH:
	case DBUS_TYPE_SIGNATURE:
		v = iter.get_string();
		break;
	case DBUS_TYPE_ARRAY:
	case DBUS_TYPE_STRUCT:
		{
			MessageIter subiter = iter.recurse();
			v.clear();
			do {
				save_iter_to_json(subiter, v[v.size()]);
			} while (!(++subiter).at_end());
		}
		break;
	case DBUS_TYPE_DICT_ENTRY:
	case DBUS_TYPE_VARIANT:
	default:
		break;
	}
}

void IpcamBase::LoadConfig()
{
	Json::Value &root = _runtime.config_root();

	if (!root.isMember(path()) || !root.isObject()) return;

	Json::Value& object = root[path()];

	if (!object.isObject()) return;

	for (auto iit = object.begin(); iit != object.end(); iit++) {
		InterfaceAdaptor* iadap = find_interface(iit.name());
		if (!iadap || !iit->isObject())
			continue;

		for (auto pit = iit->begin(); pit != iit->end(); pit++) {
			try {
				Variant value;
				do_property_get(*iadap, pit.name(), value);

				Signature sig = value.signature();
				DBusSignatureIter type;
				dbus_signature_iter_init(&type, sig.c_str());
				Variant new_value;
				MessageIter miter = new_value.writer();

				load_iter_from_json(&type, miter, *pit);

				do_property_set(*iadap, pit.name(), new_value);
			} catch (DBus::Error &e) {
				printf("%s.%s: %s\n", iit.name().c_str(), pit.name().c_str(), e.message());
			}
		}
	}
}
#endif

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

#ifdef HAVE_JSONCPP_SUPPORT
	Json::Value &root = _runtime.config_root();
	Json::Value &propval = root[path()][interface.name()][property];
	MessageIter ri = value.reader();
	save_iter_to_json(ri, propval);
	_runtime.SaveConfig();
#endif
}

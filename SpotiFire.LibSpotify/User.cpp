#include "stdafx.h"

#include "User.h"
#define SP_TYPE(type_name, ptrPtr) (type_name *)(void *)ptrPtr

User::User(SpotiFire::Session ^session, sp_user *ptr) {
	SPLock lock;
	_ptr = ptr;
	_session = session;
	sp_user_add_ref(_ptr);
}

User::~User() {
	this->!User();
}

User::!User() {
	SPLock lock;
	sp_user_release(_ptr);
	_ptr = NULL;
}

Session ^User::Session::get() {
	return _session;
}

bool User::IsLoaded::get() {
	return sp_user_is_loaded(_ptr);
}

bool User::IsReady::get() {
	return IsLoaded && !String::IsNullOrEmpty(CanonicalName);
}

String ^User::CanonicalName::get() {
	return UTF8(sp_user_canonical_name(_ptr));
}

String ^User::DisplayName::get() {
	return UTF8(sp_user_display_name(_ptr));
}
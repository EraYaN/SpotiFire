#include "stdafx.h"

#include "Playlistcontainer.h"
#define SP_TYPE(type_name, ptrPtr) (type_name *)(void *)ptrPtr

using namespace System::Runtime::InteropServices;
#define SP_STRING(str) (char *)(void *)Marshal::StringToHGlobalAnsi(str)
#define SP_FREE(str) Marshal::FreeHGlobal((IntPtr)(void *)str)

void SP_CALLCONV loaded(sp_playlistcontainer *pc, void *userdata);

sp_playlistcontainer_callbacks _callbacks = {
	NULL, // playlist added
	NULL, // playlist removed
	NULL, // playlist moved
	&loaded, // loaded
};

void SP_CALLCONV loaded(sp_playlistcontainer *pc, void *userdata);
PlaylistContainer::PlaylistContainer(SpotiFire::Session ^session, sp_playlistcontainer *ptr) {
	SPLock lock;
	_ptr = ptr;
	_session = session;
	sp_playlistcontainer_add_ref(_ptr);
	sp_playlistcontainer_add_callbacks(_ptr, &_callbacks, new gcroot<PlaylistContainer ^>(this));
}

PlaylistContainer::~PlaylistContainer() {
	this->!PlaylistContainer();
}

PlaylistContainer::!PlaylistContainer() {
	SPLock lock;
	sp_playlistcontainer_release(_ptr);
	_ptr = NULL;
}

Session ^PlaylistContainer::Session::get() {
	return _session;
}

bool PlaylistContainer::IsLoaded::get() {
	SPLock lock;
	return sp_playlistcontainer_is_loaded(_ptr);
}

User ^PlaylistContainer::Owner::get() {
	SPLock lock;
	return gcnew User(_session, sp_playlistcontainer_owner(_ptr));
}

ref class $PlaylistContainer$PlaylistList sealed : SPList<Playlist ^>
{
internal:
	PlaylistContainer ^_pc;
	$PlaylistContainer$PlaylistList(PlaylistContainer ^pc) { _pc = pc; }

public:
	virtual int DoCount() override sealed {
		SPLock lock;
		return sp_playlistcontainer_num_playlists(_pc->_ptr);
	}

	virtual Playlist ^DoFetch(int index) override sealed {
		SPLock lock;
		return gcnew Playlist(_pc->_session, sp_playlistcontainer_playlist(_pc->_ptr, index));
	}

	virtual void DoInsert(int index, Playlist ^item) override sealed {
		SPLock lock;
		throw gcnew NotImplementedException("PlaylistList::DoInsert");
	}

	virtual void DoRemove(int index) override sealed {
		SPLock lock;
		throw gcnew NotImplementedException("PlaylistList::DoRemove");
	}

	virtual void DoUpdate(int index, Playlist ^item) override sealed {
		SPLock lock;
		DoRemove(index);
		DoInsert(index - 1, item);
	}
};

IList<Playlist ^> ^PlaylistContainer::Playlists::get() {
	if(_playlists == nullptr) {
		Interlocked::CompareExchange<IList<Playlist ^> ^>(_playlists, gcnew $PlaylistContainer$PlaylistList(this), nullptr);
	}
	return _playlists;
}

//--------------------------------------------
// Meta folder
Error PlaylistContainer::AddFolder(int index, String ^name) {
	SPLock lock;
	marshal_context context;
	return ENUM(Error, sp_playlistcontainer_add_folder(_ptr, index, context.marshal_as<const char *>(name)));
}

PlaylistType PlaylistContainer::GetPlaylistType(int index) {
	SPLock lock;
	return ENUM(PlaylistType, sp_playlistcontainer_playlist_type(_ptr, index));
}

String ^PlaylistContainer::GetFolderName(int index) {
	SPLock lock;
	int count = sp_playlistcontainer_playlist_folder_name(_ptr, index, NULL, 0) + 1;
	std::vector<char> buffer(count);
	sp_playlistcontainer_playlist_folder_name(_ptr, index, buffer.data(), count);
	return UTF8(buffer);
}

UInt64 PlaylistContainer::GetFolderId(int index) {
	SPLock lock;
	return sp_playlistcontainer_playlist_folder_id(_ptr, index);
}

//--------------------------------------------
// Await
void SP_CALLCONV loaded(sp_playlistcontainer *pc, void *userdata) {
	TP0(SP_DATA(PlaylistContainer, userdata), PlaylistContainer::complete);
}

void PlaylistContainer::complete() {
	array<Action ^> ^continuations = nullptr;
	{
		SPLock lock;
		_complete = true;
		if(_continuations != nullptr) {
			continuations = gcnew array<Action ^>(_continuations->Count);
			_continuations->CopyTo(continuations, 0);
			_continuations->Clear();
			_continuations = nullptr;
		}
	}
	if(continuations != nullptr) {
		for(int i = 0; i < continuations->Length; i++)
			if(continuations[i])
				continuations[i]();
	}
}

bool PlaylistContainer::IsComplete::get() {
	SPLock lock;
	return _complete;
}

bool PlaylistContainer::AddContinuation(Action ^continuationAction) {
	SPLock lock;
	if(IsLoaded)
		return false;

	if(_continuations == nullptr)
		_continuations = gcnew List<Action ^>;

	_continuations->Add(continuationAction);
	return true;
}

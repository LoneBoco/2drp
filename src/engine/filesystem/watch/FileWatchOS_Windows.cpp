#ifdef TDRP_WINDOWS

#include "FileWatch.h"

#include <atomic>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <Windows.h>

#if defined(_MSC_VER)
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "ole32.lib")
#pragma warning (disable: 4996)
#endif

namespace tdrp::fs::watch
{

/////////////////////////////

struct Watch
{
	Watch()
		: dir_handle(0), notify_filter(0), watch_id(0), recursive(false), callback(nullptr), stop(false)
	{
		memset(&overlapped, 0, sizeof(OVERLAPPED));
		memset(buffer, 0, sizeof(buffer));
	}

	OVERLAPPED overlapped;
	HANDLE dir_handle;
	BYTE buffer[32 * 1024];
	DWORD notify_filter;

	uint32_t watch_id;
	filesystem::path dir;
	bool recursive;
	watch_cb callback;

	std::atomic<bool> stop;
};

struct WatchOS
{
};

/////////////////////////////

bool RefreshWatch(Watch* watch, bool clear = false);
void DeleteWatch(Watch* watch);

Event translateAction(DWORD action)
{
	switch (action)
	{
	case FILE_ACTION_RENAMED_NEW_NAME:
	case FILE_ACTION_ADDED:
		return Event::Add;

	case FILE_ACTION_RENAMED_OLD_NAME:
	case FILE_ACTION_REMOVED:
		return Event::Delete;

	case FILE_ACTION_MODIFIED:
		return Event::Modified;
	}

	return Event::Invalid;
}

void CALLBACK WatchCallback(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped)
{
	TCHAR szFile[MAX_PATH];
	PFILE_NOTIFY_INFORMATION pNotify;
	Watch* pWatch = (Watch*)lpOverlapped;
	size_t offset = 0;

	if (dwNumberOfBytesTransfered == 0)
		return;

	if (dwErrorCode == ERROR_SUCCESS)
	{
		do
		{
			pNotify = (PFILE_NOTIFY_INFORMATION)&pWatch->buffer[offset];
			offset += pNotify->NextEntryOffset;

#			if defined(UNICODE)
			{
				(void)lstrcpynW(szFile, pNotify->FileName,
					std::min(static_cast<DWORD>(MAX_PATH), static_cast<DWORD>(pNotify->FileNameLength / sizeof(WCHAR) + 1))
				);
			}
#			else
			{
				int count = WideCharToMultiByte(CP_ACP, 0, pNotify->FileName,
					pNotify->FileNameLength / sizeof(WCHAR),
					szFile, MAX_PATH - 1, NULL, NULL
				);
				szFile[count] = TEXT('\0');
			}
#			endif

			if (pWatch->callback != nullptr)
				pWatch->callback(pWatch->watch_id, pWatch->dir, filesystem::path{ szFile }, translateAction(pNotify->Action));
		}
		while (pNotify->NextEntryOffset != 0);
	}

	if (!pWatch->stop)
		RefreshWatch(pWatch);
	else DeleteWatch(pWatch);
}

bool RefreshWatch(Watch* watch, bool clear)
{
	return ReadDirectoryChangesW(
		watch->dir_handle, watch->buffer, sizeof(watch->buffer), watch->recursive,
		watch->notify_filter, NULL, &watch->overlapped, clear ? 0 : WatchCallback) != 0;
}

void DeleteWatch(Watch* watch)
{
	CloseHandle(watch->overlapped.hEvent);
	CloseHandle(watch->dir_handle);

	delete watch;
}

/////////////////////////////

FileWatch::FileWatch()
	: m_last_id(0)
{
}

FileWatch::~FileWatch()
{
	for (auto& w : m_watchers)
	{
		Watch* watch = w.second;

		CancelIo(watch->dir_handle);
		RefreshWatch(watch, true);

		if (!HasOverlappedIoCompleted(&watch->overlapped))
			SleepEx(5, TRUE);

		DeleteWatch(watch);
	}
}

uint32_t FileWatch::Add(const filesystem::path& directory, watch_cb callback, bool recursive)
{
	Watch* watch = new Watch;

	// Create the directory handle.
	watch->dir_handle = CreateFile(directory.c_str(), FILE_LIST_DIRECTORY, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, NULL);
	if (watch->dir_handle == INVALID_HANDLE_VALUE)
	{
		delete watch;
		return 0;
	}

	watch->overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	watch->notify_filter = FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_FILE_NAME;
	watch->recursive = recursive;

	uint32_t id = ++m_last_id;

	watch->watch_id = id;
	watch->dir = directory;
	watch->callback = callback;

	if (!RefreshWatch(watch))
	{
		DeleteWatch(watch);
		return 0;
	}

	m_watchers.insert(std::make_pair(id, watch));
	return id;
}

void FileWatch::Remove(const filesystem::path& directory)
{
	for (auto& w : m_watchers)
	{
		if (directory == w.second->dir)
		{
			Remove(w.first);
			return;
		}
	}
}

void FileWatch::Remove(uint32_t watch_id)
{
	auto i = m_watchers.find(watch_id);
	if (i == m_watchers.end())
		return;

	Watch* watch = i->second;
	m_watchers.erase(i);

	CancelIo(watch->dir_handle);
	RefreshWatch(watch);

	if (HasOverlappedIoCompleted(&watch->overlapped))
	{
		DeleteWatch(watch);
	}
}

void FileWatch::RemoveAll()
{
	for (auto& w : m_watchers)
	{
		Watch* watch = w.second;

		CancelIo(watch->dir_handle);
		RefreshWatch(watch);

		if (HasOverlappedIoCompleted(&watch->overlapped))
		{
			DeleteWatch(watch);
		}
	}

	m_watchers.clear();
}

void FileWatch::Update()
{
	MsgWaitForMultipleObjectsEx(0, NULL, 0, QS_ALLINPUT, MWMO_ALERTABLE);
}

/////////////////////////////

} // end namespace tdrp::fs::watch

#endif // TDRP_WINDOWS

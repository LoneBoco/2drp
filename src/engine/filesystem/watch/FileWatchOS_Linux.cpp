#ifdef TDRP_LINUX

#include "FileWatch.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/inotify.h>

#define BUFF_SIZE ((sizeof(struct inotify_event)+FILENAME_MAX)*1024)

namespace tdrp::fs::watch
{

/////////////////////////////

struct Watch
{
	uint32_t watch_id;
	filesystem::path dir;
	watch_cb callback;
};

struct WatchOS
{
	int fd;
	timeval timeout;
	fd_set descriptor_set;
};

/////////////////////////////

FileWatch::FileWatch()
	: m_last_id(0)
{
	m_watch_os = std::make_unique<WatchOS>();

	m_watch_os->fd = inotify_init();
	if (m_watch_os->fd < 0)
		fprintf(stderr, "Error: %s\n", strerror(errno));

	m_watch_os->timeout.tv_sec = 0;
	m_watch_os->timeout.tv_usec = 0;

	FD_ZERO(&m_watch_os->descriptor_set);
}

FileWatch::~FileWatch()
{
	RemoveAll();
}

uint32_t FileWatch::Add(const filesystem::path& directory, watch_cb callback, bool recursive)
{
	int wd = inotify_add_watch(m_watch_os->fd, directory.c_str(),
		IN_CLOSE_WRITE | IN_MOVED_TO | IN_CREATE | IN_MOVED_FROM | IN_DELETE);

	if (wd < 0)
		return 0;

	Watch* watch = new Watch;
	watch->watch_id = wd;
	watch->dir = directory;
	watch->callback = callback;

	m_watchers.insert(std::make_pair(wd, watch));
	return wd;
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

	inotify_rm_watch(m_watch_os->fd, watch->watch_id);
	delete watch;
}

void FileWatch::RemoveAll()
{
	for (auto& w : m_watchers)
	{
		inotify_rm_watch(m_watch_os->fd, w.second->watch_id);
		delete w.second;
	}

	m_watchers.clear();
}

void FileWatch::Update()
{
	FD_SET(m_watch_os->fd, &m_watch_os->descriptor_set);

	int ret = select(m_watch_os->fd + 1, &m_watch_os->descriptor_set, NULL, NULL, &m_watch_os->timeout);
	if (ret < 0)
	{
		perror("select");
	}
	else if (FD_ISSET(m_watch_os->fd, &m_watch_os->descriptor_set))
	{
		ssize_t len, i = 0;
		char action[81 + FILENAME_MAX] = { 0 };
		char buff[BUFF_SIZE] = { 0 };

		len = read(m_watch_os->fd, buff, BUFF_SIZE);

		while (i < len)
		{
			struct inotify_event *pevent = (struct inotify_event *)&buff[i];

			auto iter = m_watchers.find(pevent->wd);
			if (iter != m_watchers.end())
			{
				Watch* watch = iter->second;
				filesystem::path filename{ pevent->name };

				Event e = Event::Invalid;
				if (IN_CLOSE_WRITE & pevent->mask)
					e = Event::Modified;
				if (IN_MOVED_TO & pevent->mask || IN_CREATE & pevent->mask)
					e = Event::Add;
				if (IN_MOVED_FROM & pevent->mask || IN_DELETE & pevent->mask)
					e = Event::Delete;

				watch->callback(watch->watch_id, watch->dir, filename, Event::Modified);
			}

			i += sizeof(struct inotify_event) + pevent->len;
		}
	}
}

/////////////////////////////

} // end namespace tdrp::fs::watch

#endif // TDRP_LINUX

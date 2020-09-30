#pragma once

#include <mutex>

#include "engine/common.h"

namespace tdrp
{

class DownloadManager
{
public:
	DownloadManager() = default;
	~DownloadManager() = default;

	DownloadManager(const DownloadManager& other) = delete;
	DownloadManager(DownloadManager&& other) = delete;
	DownloadManager& operator=(const DownloadManager& other) = delete;
	DownloadManager& operator=(DownloadManager&& other) = delete;
	bool operator==(const DownloadManager& other) = delete;

public:
	void AddToQueue(const std::string& file);
	void AddToQueue(std::list<std::string>&& batch);
	void InformComplete(const std::string& file);
	
	void TryClearProcessingFlag();

public:
	std::atomic<bool> FilesInQueue = false;

private:
	std::list<std::string> m_download_list;
	std::atomic<bool> m_processing;
	mutable std::mutex m_queue_mutex;
};


inline void DownloadManager::TryClearProcessingFlag()
{
	std::scoped_lock guard(m_queue_mutex);
	if (m_download_list.empty())
		FilesInQueue = false;
}

} // end namespace tdrp

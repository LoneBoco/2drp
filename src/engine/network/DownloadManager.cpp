#include "engine/server/Server.h"
#include "engine/network/DownloadManager.h"

#include "engine/network/Packet.h"
#include "engine/network/PacketsInc.h"

namespace tdrp::network
{

void DownloadManager::AddToQueue(const std::string& file)
{
	packet::FileRequest request;
	auto newfile = request.add_file();
	*newfile = file;

	std::scoped_lock guard(m_queue_mutex);
	m_download_list.push_back(file);
	FilesInQueue = true;

	m_server->Send(0, network::PACKETID(network::Packets::FILEREQUEST), network::Channel::RELIABLE, request);
}

void DownloadManager::AddToQueue(std::list<std::string>&& batch)
{
	packet::FileRequest request;
	for (auto& entry : batch)
	{
		auto newfile = request.add_file();
		*newfile = entry;
	}

	std::scoped_lock guard(m_queue_mutex);
	m_download_list.splice(std::end(m_download_list), batch);
	FilesInQueue = true;

	m_server->Send(0, network::PACKETID(network::Packets::FILEREQUEST), network::Channel::RELIABLE, request);
}

void DownloadManager::InformComplete(const std::string& file)
{
	std::scoped_lock guard(m_queue_mutex);
	(void)std::remove_if(std::begin(m_download_list), std::end(m_download_list), [&file](const auto& entry) { return entry == file; });

	if (m_download_list.empty())
		FilesInQueue = false;
}

} // end namespace tdrp::network

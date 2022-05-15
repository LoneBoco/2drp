#pragma once

#include "engine/common.h"
#include "engine/scene/ObjectAttributes.h"


namespace tdrp::server
{

class Player;

class Account
{
public:
	Account(Player* player) : m_player(player) {}
	~Account() = default;

	Account(const Account& other) = delete;
	Account(Account&& other) = delete;
	Account& operator=(const Account& other) = delete;
	Account& operator=(Account&& other) = delete;
	
public:
	void Load(const std::string& name);
	void Save();

public:
	ObjectAttributes Flags;

protected:
	Player* m_player;
	std::string m_name;
};

} // end namespace tdrp::server

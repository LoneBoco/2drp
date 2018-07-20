#pragma once

#include "engine/common.h"
#include "engine/scene/ObjectAttributes.h"

namespace tdrp::server
{

class Account
{
public:
	Account() = default;
	~Account() = default;

	Account(const Account& other) = delete;
	Account(Account&& other) = delete;
	Account& operator=(const Account& other) = delete;
	Account& operator=(Account&& other) = delete;
	
public:
	ObjectAttributes Flags;

protected:
	std::string m_name;
};

} // end namespace tdrp::server

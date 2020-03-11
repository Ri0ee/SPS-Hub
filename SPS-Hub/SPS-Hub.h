#pragma once

#include <iostream>
#include <string>
#include <map>
#include <fstream>

#include "json.hpp"
#include "httplib.h"
#include "Conf-Manager.h"

struct PowerSocket {
	PowerSocket(std::string address_ = "0.0.0.0") : address(address_) {}

	enum class SocketState {
		ACTIVE,
		INACTIVE,
		UNKNOWN
	} state = SocketState::UNKNOWN;

	std::string address;
	bool notified = false;
	
	float latest_data = -1;
};
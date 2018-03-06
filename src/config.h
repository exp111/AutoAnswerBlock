#pragma once
#include <vector>
#include <string>

class Config
{
public:
	Config() { };
	~Config() { };

	std::vector<std::string> whiteListUIDs = {};
	double lastTimestamp = -1;
	int delay = 500;
};

extern Config* config;
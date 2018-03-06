#pragma once

class Config
{
public:
	Config() { };
	~Config() { };

	double lastTimestamp = -1;
	int delay = 500;
};

extern Config* config;
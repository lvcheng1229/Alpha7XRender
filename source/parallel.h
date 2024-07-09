#pragma once
#include <vector>
#include <thread>
class CThreadPool
{
public:
private:
	std::vector<std::thread> threads;
};
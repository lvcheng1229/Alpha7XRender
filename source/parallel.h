#pragma once
#include <vector>
#include <thread>
#include <mutex>
#include <functional>
#include <glm/vec2.hpp>
#include "common.h"

class CThreadPool;

class CParallelJob
{
public:
	static CThreadPool* thread_pool;

	virtual bool haveWork() const = 0;
	virtual void runStep(std::unique_lock<std::mutex>* lock) = 0;

	inline bool finished() { return (!haveWork()) && (active_workers == 0); };
private:
	friend class CThreadPool;

	int active_workers = 0;
	CParallelJob* prev = nullptr, * next = nullptr;
};

class CThreadPool
{
public:
	explicit CThreadPool(int num_threads);
	~CThreadPool();

	size_t size() const { return threads.size(); }

	std::unique_lock<std::mutex> addToJobList(CParallelJob* job);
	void removeFromJobList(CParallelJob* job);
	
	void workOrWait(std::unique_lock<std::mutex>* lock);

private:
	void worker();

	std::vector<std::thread> threads;
	mutable std::mutex mutex;
	CParallelJob* job_list = nullptr;
	std::condition_variable job_list_condition;
	bool shut_down_threads = false;
};



void parallelFor2D(glm::u32vec2 bound_min, glm::u32vec2 bound_max, std::function<void(glm::u32vec2 bound_min, glm::u32vec2 bound_max)> func);

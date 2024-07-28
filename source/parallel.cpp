#include "parallel.h"

CThreadPool* CParallelJob::thread_pool;

class CParallelJob2D : public CParallelJob
{
public:
	CParallelJob2D(glm::u32vec2 bound_min, glm::u32vec2 bound_max, glm::uint32 chunk_size, std::function<void(glm::u32vec2, glm::u32vec2)> func)
		: bound_min(bound_min)
		, bound_max(bound_max)
		, next_start(bound_min)
		, chunk_size(chunk_size)
		, func(func) {}

	bool haveWork() const override{ return next_start.y < bound_max.y; }
	
	void runStep(std::unique_lock<std::mutex>* lock)override;

private:
	glm::u32vec2 bound_min;
	glm::u32vec2 bound_max;

	glm::u32vec2 next_start;
	
	glm::uint32 chunk_size;
	
	std::function<void(glm::u32vec2 bound_min, glm::u32vec2 bound_max)> func;
};

void CParallelJob2D::runStep(std::unique_lock<std::mutex>* lock)
{
	glm::u32vec2 min_pixel = next_start;
	glm::u32vec2 max_pixel = next_start + glm::u32vec2(chunk_size, chunk_size);

	next_start.x += chunk_size;
	if (next_start.x >= bound_max.x) 
	{
		next_start.x = bound_max.x;
		next_start.y += chunk_size;
	}

	if (!haveWork())
	{
		CParallelJob::thread_pool->removeFromJobList(this);
	}

	lock->unlock();
	func(min_pixel, max_pixel);

}
void parallelFor2D(glm::u32vec2 bound_min, glm::u32vec2 bound_max, std::function<void(glm::u32vec2, glm::u32vec2)> func)
{
	if (bound_min.x >= bound_max.x || bound_min.y >= bound_max.y)
	{
		return;
	}

	glm::uint32 total_are = (bound_max.y - bound_min.y) * (bound_max.x - bound_min.x);
	glm::uint32 per_thread_are = total_are / (CParallelJob::thread_pool ? CParallelJob::thread_pool->size() : 1);
	glm::uint32 tile_size = (std::sqrt)(per_thread_are);

	CParallelJob2D loop(bound_min, bound_max, tile_size, std::move(func));
	std::unique_lock<std::mutex> lock = CParallelJob::thread_pool->addToJobList(&loop);
	while (!loop.finished())
	{
		CParallelJob::thread_pool->workOrWait(&lock);
	}
}

void CThreadPool::workOrWait(std::unique_lock<std::mutex>* lock)
{
	CParallelJob* job = job_list;

	while (job && !job->haveWork())
	{
		job = job->next;
	}

	if (job)
	{
		job->active_workers++;
		job->runStep(lock);
		lock->lock();
		job->active_workers--;
		if (job->finished())
		{
			job_list_condition.notify_all();;
		}
	}
	else
	{
		job_list_condition.wait(*lock);
	}
}

void CThreadPool::worker()
{
	std::unique_lock<std::mutex> lock(mutex);
	while (!shut_down_threads)
	{
		workOrWait(&lock);
	}
}


CThreadPool::CThreadPool(int num_threads)
{
	for (int i = 0; i < num_threads - 1; ++i)
	{
		threads.push_back(std::thread(&CThreadPool::worker, this));
	}
}

CThreadPool::~CThreadPool()
{
	assert_t(false);
}

std::unique_lock<std::mutex> CThreadPool::addToJobList(CParallelJob* job)
{
	std::unique_lock<std::mutex> lock(mutex);

	// add to header
	if (job_list)
	{
		job_list->prev = job;
	}

	job->next = job_list;
	job_list = job;

	job_list_condition.notify_all();
	return lock;
}

void CThreadPool::removeFromJobList(CParallelJob* job)
{
	if (job->prev)
	{
		job->prev->next = job->next;
	}
	else 
	{
		job_list = job->next;
	}
	
	if (job->next)
	{
		job->next->prev = job->prev;
	}
}
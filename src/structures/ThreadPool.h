#pragma once

#include <queue>
#include <vector>
#include <concepts>

#include <functional>

#include <mutex>
#include <thread>
#include <condition_variable>

template <typename T>
concept Job = std::is_function<T>::value;

//requires(T t) {
//	{ std::is_function<t>}
//};

template <typename T = std::function<void()>>
class ThreadPool {
public:
	ThreadPool(int n) {
		for(int i = 0; i < n; ++i)
			m_threads.emplace_back(std::thread(&ThreadPool::loop, this));
	}
	~ThreadPool() {
		stop();
	}

	void add(T job) {
		{
			std::unique_lock lock(m_mutex);
			m_jobs.push(job);
		}
		m_cv.notify_one(); // Notifying only one because only one job is added so only one thread can be working on it?
	}

	void stop()
	{
		{
			std::unique_lock lock(m_mutex);
			m_terminate = true;
		}
		m_cv.notify_all();

		for (auto& t : m_threads) {
			t.join();
		}
		m_threads.clear();
	}

private:
	void loop() {
		while (true) {
			T job;
			{
				std::unique_lock lock(m_mutex);
				m_cv.wait(lock, [this] {
					return !m_jobs.empty() || m_terminate;
					});

				if (m_terminate) {
					printf("Terminate\n");
					break;
				}
				job = m_jobs.front();
				m_jobs.pop();
			}
			job();
		}
		printf("Threadpool ended\n");
	}
	bool m_terminate = false;
	std::condition_variable m_cv;
	std::mutex m_mutex;

	std::vector<std::thread> m_threads;
	std::queue<T> m_jobs;
};
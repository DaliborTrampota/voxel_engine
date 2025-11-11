#pragma once

#include <atomic>
#include <concepts>
#include <future>
#include <memory>
#include <queue>
#include <vector>

#include <functional>

#include <condition_variable>
#include <mutex>
#include <thread>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif

using Job = std::function<void()>;
class ThreadPool {
  public:
    ThreadPool(int n) {
        if (n == 0)
            throw std::runtime_error("Thread pool thread count is 0!");

        for (int i = 0; i < n; ++i)
            m_threads.emplace_back(std::thread(&ThreadPool::loop, this));
    }
    ~ThreadPool() { stop(); }

    void add(Job job) {
        // clang-format off
        {
            std::unique_lock lock(m_mutex);
            m_jobs.push(job);
        }
        m_cv.notify_one();  // Notifying only one because only one job is added so
                            // only one thread can be working on it?
        // clang-format on
    }

    std::future<void> addBatch(std::vector<Job> batch) {
        auto promise = std::make_shared<std::promise<void>>();
        auto remainingJobs = std::make_shared<std::atomic<int>>(batch.size());
        std::future<void> future = promise->get_future();

        for (const auto& job : batch) {
            add([job, promise, remainingJobs]() {
                job();

                if (remainingJobs->fetch_sub(1) == 1) {
                    promise->set_value();
                }
            });
        }

        return future;
    }

    void stop() {
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

    void resume() { m_pause = false; }

    void pause() { m_pause = true; }

  private:
    void loop() {
#ifdef _WIN32
        SetThreadDescription(GetCurrentThread(), L"ThreadPool");
#endif
        while (true) {
            Job job;
            {
                std::unique_lock lock(m_mutex);
                m_cv.wait(lock, [this] { return !m_jobs.empty() || m_terminate || m_pause; });

                if (m_terminate) {
                    break;
                }
                job = m_jobs.front();
                m_jobs.pop();
            }
            job();
        }
    }
    bool m_terminate = false;
    bool m_pause = false;
    std::condition_variable m_cv;
    std::mutex m_mutex;

    std::vector<std::thread> m_threads;
    std::queue<Job> m_jobs;
};
#pragma once

#include "commonheader.h"

#include "camera.h"
#include "hitable.h"

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

class Renderer;

// task for multi-threaded raytracing comprising multiple lines
struct RenderTask
{
    Renderer* renderer;

    int minLine;
    int maxLine; // exclusive - the max line is not rendered

    const Camera& camera;
    const Hitable& world;

    // viewport information
    glm::vec3 lowerLeft;
    glm::vec3 vertical;
    glm::vec3 horizontal;

    void operator()();
};

class RenderThreadPool
{
public:
    RenderThreadPool(int numThreads = std::thread::hardware_concurrency());
    ~RenderThreadPool();

    RenderThreadPool(const RenderThreadPool&) = delete;
    RenderThreadPool& operator=(const RenderThreadPool&) = delete;
    RenderThreadPool(RenderThreadPool&&) = delete;
    RenderThreadPool& operator=(RenderThreadPool&&) = delete;

    // (synchronized) method for adding a new task to the queue
    void AddTask(RenderTask r);

    // wait until all tasks are finished (i.e. task counter == 0)
    void WaitForTasks();

    // functionality for setting task counter to set the number of jobs and control when they are finished (unsynchronized!!!)
    void SetTaskCounter(int c) { m_taskCounter = c; }

    size_t GetNumThreads() const { return m_threads.size(); }

protected:
    // task queue
    std::queue<RenderTask> m_taskQueue;
    std::mutex m_queueMutex;
    std::condition_variable m_queueCondition;

    // counter for checking if all current tasks are done (needs to be set up front and is decreased once for each task that is finished)
    int m_taskCounter;
    std::mutex m_taskCounterMutex;
    std::condition_variable m_taskCounterCondition;

    // thread pool
    std::vector<std::thread> m_threads;
    bool m_stopThreads;   // for signaling threads to stop working
};



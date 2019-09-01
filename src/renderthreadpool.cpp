#include "renderthreadpool.h"

#include "renderer.h"

void RenderTask::operator()() { 
    renderer->RenderLines(minLine, maxLine, camera, world, lowerLeft, vertical, horizontal);
}

RenderThreadPool::RenderThreadPool(int numThreads) : m_taskCounter(0), m_stopThreads(false)
{
    numThreads = std::max(numThreads, 1);

    for (int i = 0; i < numThreads; ++i)
        m_threads.push_back(std::thread{
            /* lambda function for thread pool loop */
            [&]() {
                std::unique_lock<std::mutex> tLck(m_taskCounterMutex, std::defer_lock);
                std::unique_lock<std::mutex> qLck(m_queueMutex, std::defer_lock);
                while (true) {
                    // wait for new task or stop signal
                    qLck.lock();
                    m_queueCondition.wait(qLck, [&]() {return (!m_taskQueue.empty() || m_stopThreads); });
                    if (m_stopThreads)
                    {
                        qLck.unlock();
                        break;
                    }
                    auto t = m_taskQueue.front();
                    m_taskQueue.pop();
                    qLck.unlock();
                    // compute task
                    t();
                    // decrease task counter after finishing task computation and notify waiting threads
                    tLck.lock();
                    m_taskCounter--;
                    tLck.unlock();
                    m_taskCounterCondition.notify_all();
                }
            }
        }
    );
}

RenderThreadPool::~RenderThreadPool()
{
    // set stop condition variable and join threads
    m_stopThreads = true;
    m_queueCondition.notify_all();
    for (auto& t : m_threads)
        t.join();
}

void RenderThreadPool::AddTask(RenderTask t)
{
    std::lock_guard<std::mutex> lck(m_queueMutex);
    m_taskQueue.push(t);
    m_queueCondition.notify_one();
}

void RenderThreadPool::WaitForTasks()
{
    std::unique_lock<std::mutex> lck(m_taskCounterMutex);
    m_taskCounterCondition.wait(lck, [&]() {return (m_taskCounter == 0); });
    lck.unlock();
}

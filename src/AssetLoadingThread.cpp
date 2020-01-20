#include "AssetLoadingThread.h"

#include "Image.h"
#include "vulkan/EngineInternal.h"

#include <deque>
#include <future>
#include <mutex>
#include <thread>

using namespace std;
using namespace CR::Graphics;

namespace {
	thread m_thread;
	atomic_bool m_running;
	mutex m_requestMutex;
	condition_variable m_notify;
	deque<AssetLoadingThread::task_t> m_requests;

	void ThreadMain() {
		while(m_running.load()) {
			AssetLoadingThread::task_t request;
			{
				unique_lock<mutex> lock(m_requestMutex);
				if(m_requests.empty()) { m_notify.wait(lock); }

				if(!m_requests.empty()) {
					request = move(m_requests.front());
					m_requests.pop_front();
				}
			}
			if(request) { request(); }
		}
		m_requests.clear();
	}
}    // namespace

void AssetLoadingThread::Init() {
	m_thread = thread([]() { ThreadMain(); });
}

void AssetLoadingThread::Shutdown() {
	m_running.store(false);
	m_notify.notify_one();
	m_thread.join();
}

std::shared_ptr<std::atomic_bool> AssetLoadingThread::LoadAsset(task_t&& a_task) {
	std::shared_ptr<std::atomic_bool> result = make_shared<std::atomic_bool>(false);
	task_t wrappedTask                       = [result = result, task = move(a_task)]() mutable {
        task();
        result->store(true);
	};
	{
		unique_lock<mutex> lock(m_requestMutex);
		m_requests.push_back(move(wrappedTask));
	}
	m_notify.notify_one();
	return result;
}

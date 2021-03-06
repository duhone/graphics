﻿#include "AssetLoadingThread.h"

#include "CommandPool.h"
#include "EngineInternal.h"

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
		CommandPool cmdPool(CommandPool::PoolType::Transfer);
		while(m_running.load(memory_order_acquire)) {
			AssetLoadingThread::task_t request;
			{
				unique_lock<mutex> lock(m_requestMutex);
				if(m_requests.empty()) { m_notify.wait(lock); }

				if(!m_requests.empty()) {
					request = move(m_requests.front());
					m_requests.pop_front();
				}
			}
			if(request) {
				CommandBuffer cmdBuffer;
				auto getCmdBuffer = [&]() -> CommandBuffer& {
					cmdBuffer = cmdPool.CreateCommandBuffer();
					cmdBuffer.Begin();
					return cmdBuffer;
				};
				auto submit = [&]() {
					cmdBuffer.End();

					vk::SubmitInfo subInfo;
					subInfo.commandBufferCount = 1;
					subInfo.pCommandBuffers    = &cmdBuffer.GetHandle();
					GetTransferQueue().submit(subInfo, vk::Fence{});
					GetTransferQueue().waitIdle();
				};

				request(getCmdBuffer, submit);
			}
		}
		m_requests.clear();
	}
}    // namespace

void AssetLoadingThread::Init() {
	m_running.store(true, memory_order_release);
	m_thread = thread([]() { ThreadMain(); });
}

void AssetLoadingThread::Shutdown() {
	m_running.store(false, memory_order_release);
	m_notify.notify_one();
	m_thread.join();
}

std::shared_ptr<std::atomic_bool> AssetLoadingThread::LoadAsset(task_t&& a_task) {
	std::shared_ptr<std::atomic_bool> result = make_shared<std::atomic_bool>(false);

	task_t wrappedTask = [result = result, task = move(a_task)](fu2::unique_function<CommandBuffer&()> getCmdBuffer,
	                                                            fu2::unique_function<void()> submit) mutable {
		task(move(getCmdBuffer), move(submit));
		result->store(true, memory_order_release);
	};
	{
		unique_lock<mutex> lock(m_requestMutex);
		m_requests.push_back(move(wrappedTask));
	}
	m_notify.notify_one();
	return result;
}

#pragma once

#include "VulkanWindows.h"

namespace CR::Graphics {
	class Event {
	  public:
		Event();
		~Event();
		Event(const Event&)    = delete;
		Event(Event&& a_other) = delete;
		Event& operator=(const Event&) = delete;
		Event& operator=(Event&& a_other) = delete;

		operator vk::Event() { return m_event; }

	  private:
		vk::Event m_event;
	};
}    // namespace CR::Graphics

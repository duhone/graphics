#include "Event.h"

#include "EngineInternal.h"

using namespace CR::Graphics;

Event::Event() {
	m_event = GetDevice().createEvent(vk::EventCreateInfo{});
}

Event::~Event() {
	GetDevice().destroyEvent(m_event);
}

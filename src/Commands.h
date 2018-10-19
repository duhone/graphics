#pragma once

namespace CR::Graphics {
	struct Commands {
		Commands()                = default;
		virtual ~Commands()       = default;
		Commands(const Commands&) = delete;
		Commands& operator=(const Commands&) = delete;
	};
}    // namespace CR::Graphics

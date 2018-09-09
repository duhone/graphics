#pragma once
#include "math/Types.h"
#include <memory>

namespace CR::Graphics {
	struct Image {
		Image()         = default;
		Image(Image&)   = delete;
		Image& operator=(Image&) = delete;
		virtual ~Image()         = default;
	};

	enum class ImageType {
		AsRGB,
		BC1,
		BC3,
		BC4,
		BC5,
	};

	std::unique_ptr<Image> CreateImage(ImageType a_type, uint a_width, uint a_height, uint a_arrayLayers, uint a_mips);
}    // namespace CR::Graphics

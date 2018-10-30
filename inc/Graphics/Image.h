#pragma once
#include <atomic>
#include <memory>
#include <vector>

namespace CR::Graphics {
	struct Image {
		Image()        = default;
		Image(Image&)  = delete;
		Image& operator=(Image&) = delete;
		virtual ~Image()         = default;

		struct ImageUpdate {
			std::atomic_bool Ready{false};
		};

		virtual std::shared_ptr<ImageUpdate> UpdateImage(std::vector<uint8_t>&& data, uint32_t a_arrayIndex) = 0;
	};

	enum class ImageType {
		sRGBAUncompressed,    // Use for runtime generated textures
		sRGBACompressed       // BC7 on PC, ASTC on mobile
	};

	std::unique_ptr<Image> CreateImage(ImageType a_type, uint32_t a_width, uint32_t a_height, uint32_t a_arrayLayers,
	                                   uint32_t a_mips);
}    // namespace CR::Graphics

#pragma once

#include "core/Span.h"

#include "vulkan/vulkan.hpp"

#include <cstddef>
#include <memory>
#include <vector>

namespace CR::Graphics {
	struct CreatePipelineArgs {
		Core::Span<std::byte> ShaderModule;    // crsm file
	};

	struct Pipeline {
		Pipeline() = default;
		Pipeline(const CreatePipelineArgs& a_args);
		~Pipeline();
		Pipeline(const Pipeline&) = delete;
		Pipeline(Pipeline&& a_other);
		Pipeline& operator=(const Pipeline&) = delete;
		Pipeline& operator                   =(Pipeline&& a_other);

		operator bool() const { return m_pipeline; }

		const vk::Pipeline& GetHandle() const { return m_pipeline; }
		const vk::PipelineLayout& GetLayout() const { return m_pipeLineLayout; }

	  private:
		void Free();

		vk::PipelineLayout m_pipeLineLayout;
		vk::Pipeline m_pipeline;
		vk::DescriptorSetLayout m_descriptorSetLayout;
	};
}    // namespace CR::Graphics

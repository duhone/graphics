#pragma once

#include "DescriptorPool.h"

#include "core/Span.h"

#include "vulkan/vulkan.hpp"

#include <cstddef>
#include <memory>
#include <vector>

namespace CR::Graphics {
	struct CreatePipelineArgs {
		Core::Span<const std::byte> ShaderModule;    // crsm file
	};

	class Pipeline {
	  public:
		Pipeline() = default;
		Pipeline(const CreatePipelineArgs& a_args);
		~Pipeline();
		Pipeline(const Pipeline&) = delete;
		Pipeline(Pipeline&& a_other) noexcept;
		Pipeline& operator=(const Pipeline&) = delete;
		Pipeline& operator                   =(Pipeline&& a_other) noexcept;

		operator bool() const { return (bool)m_pipeline; }

		[[nodiscard]] const vk::Pipeline& GetHandle() const { return m_pipeline; }
		[[nodiscard]] const vk::PipelineLayout& GetLayout() const { return m_pipeLineLayout; }
		[[nodiscard]] const vk::DescriptorSetLayout& GetDescLayout() const { return m_descriptorSetLayout; }

		void Frame(vk::DescriptorSet& a_set);

	  private:
		void Free();

		vk::PipelineLayout m_pipeLineLayout;
		vk::Pipeline m_pipeline;
		vk::DescriptorSetLayout m_descriptorSetLayout;
		vk::Sampler m_sampler;

		uint32_t m_lastTextureVersion{0};
	};
}    // namespace CR::Graphics

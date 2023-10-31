#pragma once
#include "_iglib_vertex.h" // <- for variouse stuff

namespace ig
{
	class IndexBuffer
	{
	public:
		IndexBuffer();
		IndexBuffer(size_t size, const uint32_t *indcies = nullptr);

		size_t get_size() const;
		uint32_t get_id() const;

		BufferUsage get_usage() const;
		void set_usage(BufferUsage usage);

		
		void generate(size_t size, const uint32_t *indcies = nullptr);

		void update(const uint32_t *indcies, const size_t indcies_count, const uint32_t offset);

		/// @brief will update the entire current buffer with @p vertcies, @p vertices should be an array with size not smaller then the buffer size
		///
		/// same as index_buffer.update(indcies, index_buffer.get_size(), 0)
		void update(const uint32_t *indcies);

	private:
		struct _buffer
		{
			~_buffer();

			uint32_t id;
		};
		std::shared_ptr<_buffer> m_buf;
		BufferUsage m_usage;
		size_t m_size;
	};
}

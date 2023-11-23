#pragma once
#include "_iglib_vertex.h" // <- for variouse stuff

namespace ig
{
	class IndexBuffer
	{
	public:
#ifdef HIGH_RANGE_INDECIES
		using index_type = unsigned int;
#else
		using index_type = unsigned short;
#endif

		IndexBuffer();
		IndexBuffer(size_t size, const index_type *indcies = nullptr);

		size_t size() const;
		uint32_t get_id() const;

		BufferUsage get_usage() const;
		void set_usage(BufferUsage usage);

		
		void generate(size_t size, const index_type *indcies = nullptr);

		void update(const index_type *indcies, const size_t indcies_count, const uint32_t offset);

		/// @brief will update the entire current buffer with @p vertcies, @p vertices should be an array with size not smaller then the buffer size
		///
		/// same as index_buffer.update(indcies, index_buffer.get_size(), 0)
		void update(const index_type *indcies);

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

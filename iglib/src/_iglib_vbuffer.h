#pragma once
#include "_iglib_vertex.h"

namespace ig
{
	typedef unsigned int VBufferName_t;

	enum class VBufferUsage
	{
		StreamWrite = 0x88E0,
		StreamDraw = 0x88E0,
		StreamRead,
		StreamCopy,

		StaticWrite = 0x88E4,
		StaticDraw = 0x88E4,
		StaticRead,
		StaticCopy,

		DynamicWrite = 0x88E8,
		DynamicDraw = 0x88E8,
		DynamicRead,
		DynamicCopy
	};

	template <typename _T, int _GL_TYPE>
	class VBuffer
	{
	public:
		using element_type = _T;
		using this_type = VBuffer<_T, _GL_TYPE>;
		static constexpr int GLType = _GL_TYPE;
		static constexpr bool Generic = std::is_void_v<_T>;

		VBuffer();
		VBuffer( size_t size, VBufferUsage usage, const element_type *data = nullptr );
		~VBuffer();

		VBuffer( const this_type &copy );
		VBuffer( this_type &&move ) noexcept;

		this_type &operator=( const this_type &copy );
		this_type &operator=( this_type &&move ) noexcept;

		VBufferUsage usage() const;

		size_t size() const;
		size_t size_bytes() const;

		/// @brief 
		/// @param size 
		/// @param data 
		void create( size_t size, VBufferUsage usage, const element_type *data = nullptr );

		/// @brief deletes the buffer and sets the name to zero
		void free();

		/// @brief 
		/// @param data the data to update the buffer with
		/// @param start the starting element of the buffer to be updated
		/// @param end the last element of the buffer to be updated
		void update( const element_type *data, size_t start = 0, size_t end = 0 );

		/// @brief 
		/// @param data the data to update the buffer with
		/// @param start_byte the starting byte of the buffer to be updated
		/// @param end_byte the last byte of the buffer to be updated
		void update_bytes( const void *data, size_t start_byte = 0, size_t end_byte = 0 );

		/// @brief bind to use in later api calls
		void bind() const;

		/// @brief weather this buffer is bound
		/// @brief buffers of different _GL_TYPEs can be bound at the same time
		bool is_bound() const;

		/// @brief what buffer name is bound currently
		/// @brief buffers of different _GL_TYPEs can be bound at the same time
		static VBufferName_t get_bound();
		static void clear_bound();


		// pushes the currently bound buffer name to a stack and clears bound vbuffer
		static void push_bound_name();
		// pops the top buffer name and binds it
		static void pop_bound_name();

		inline VBufferName_t get_name() const noexcept {
			return m_name;
		}

	private:
		VBufferName_t m_name;
	};

	using RawVertexBuffer = VBuffer<void, 0x8892>;
	using Index8Buffer = VBuffer<uint8_t, 0x8893>;
	using Index16Buffer = VBuffer<uint16_t, 0x8893>;
	using Index32Buffer = VBuffer<uint32_t, 0x8893>;
	using UniformBuffer = VBuffer<void, 0x8A11>;
	using TextureBuffer = VBuffer<void, 0x8C2A>;



}

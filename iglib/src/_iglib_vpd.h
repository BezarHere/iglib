#pragma once
#include "_iglib_vertex.h"

namespace ig
{
	typedef unsigned int VPDName_t;

	constexpr int NormalizedVPDAttributeTypeBit = 0x10000;

	enum class VPDAttributeType
	{
		Byte = 0x1400,
		UnsignedByte,
		Short,
		UnsignedShort,
		Int,
		UnsignedInt,

		NormalizedByte = Byte | NormalizedVPDAttributeTypeBit,
		NormalizedUnsignedByte = UnsignedByte | NormalizedVPDAttributeTypeBit,
		NormalizedShort = Short | NormalizedVPDAttributeTypeBit,
		NormalizedUnsignedShort = UnsignedShort | NormalizedVPDAttributeTypeBit,
		NormalizedInt = Int | NormalizedVPDAttributeTypeBit,
		NormalizedUnsignedInt = UnsignedInt | NormalizedVPDAttributeTypeBit,

		HalfFloat = 0x140B,
		Float = 0x1406,
		Double = 0x140A
	};

	enum class VPDAttributeSize
	{
		Single = 1,

		Double = 2,
		Vec2 = 2,

		Trible = 3,
		RGB = 3,
		Vec3 = 3,

		Qoudruble = 4,
		Vec4 = 4,
		RGBA = 4,
		Color = 4
	};

	struct VPDAttribute
	{
		VPDAttributeType type;
		VPDAttributeSize size;
	};


	class VertexPipelineDescriptor
	{
	public:
		using attrs_collection = std::vector<VPDAttribute>;

		// some (& nearly all modren) gpus support more then 16 but 16 is just large enough to not be a proplem
		static constexpr size_t MaxVertexAttributes = 16ull;

		// updates the vpd on dtor
		struct AttributesInterface
		{
			inline AttributesInterface( VertexPipelineDescriptor &parent )
				: m_parent{ parent }, attributes{ parent.m_attrs } {
			}

			// TODO: implement
			//AttributesInterface( AttributesInterface &&move );
			//AttributesInterface &operator=( AttributesInterface &&move );

			~AttributesInterface();


			attrs_collection &attributes;
		private:
			VertexPipelineDescriptor &m_parent;
		};

		VertexPipelineDescriptor();
		~VertexPipelineDescriptor();

		VertexPipelineDescriptor( const VertexPipelineDescriptor &copy );
		VertexPipelineDescriptor( VertexPipelineDescriptor &&move ) noexcept;
		VertexPipelineDescriptor &operator=( const VertexPipelineDescriptor &copy );
		VertexPipelineDescriptor &operator=( VertexPipelineDescriptor &&move ) noexcept;

		// only binds, doesn't apply/enable the vertex array attribs
		void bind() const;

		void setup() const;

		static VPDName_t get_bound();
		static void clear_bound();
		static void clear_setup();

		/// @param stride a stride of zero will be defaulted to the vertex size.
		///	              it's allways better to set the stride either zero or greater/equal to the vertex size 
		void set_stride( uint32_t stride );
		void set_offset( uint32_t offset );

		// interfaces can be used to edit the attributes of VPD
		AttributesInterface create_interface();

		inline VPDName_t get_name() const noexcept {
			return m_name;
		}

		inline const attrs_collection &get_attrs() const noexcept {
			return m_attrs;
		}

		inline uint32_t get_stride() const noexcept {
			return m_stride;
		}

		inline uint32_t get_offset() const noexcept {
			return m_offset;
		}

		size_t get_vertex_size() const;

	private:
		void _validate_attributes();

	private:
		VPDName_t m_name;
		attrs_collection m_attrs;
		uint32_t m_stride;
		uint32_t m_offset;
	};

	using VPDAttributeInterface = VertexPipelineDescriptor::AttributesInterface;

}

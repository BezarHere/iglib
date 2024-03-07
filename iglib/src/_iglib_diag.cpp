#include "pch.h"
#include "_iglib_diag.h"

struct GLFieldRegister
{
	std::array<char, 96> name; // <- null terminated
	GLint pname;
};

static constexpr GLFieldRegister GLRegistry[] =
{
	{ { "active_texture" }, GL_ACTIVE_TEXTURE },
	{ { "aliased_line_width_range" }, GL_ALIASED_LINE_WIDTH_RANGE },
	{ { "array_buffer_binding" }, GL_ARRAY_BUFFER_BINDING },
	{ { "blend" }, GL_BLEND },
	{ { "blend_color" }, GL_BLEND_COLOR },
	{ { "blend_dst_alpha" }, GL_BLEND_DST_ALPHA },
	{ { "blend_dst_rgb" }, GL_BLEND_DST_RGB },
	{ { "blend_equation_alpha" }, GL_BLEND_EQUATION_ALPHA },
	{ { "blend_equation_rgb" }, GL_BLEND_EQUATION_RGB },
	{ { "blend_src_alpha" }, GL_BLEND_SRC_ALPHA },
	{ { "blend_src_rgb" }, GL_BLEND_SRC_RGB },
	{ { "color_clear_value" }, GL_COLOR_CLEAR_VALUE },
	{ { "color_logic_op" }, GL_COLOR_LOGIC_OP },
	{ { "color_writemask" }, GL_COLOR_WRITEMASK },
	{ { "compressed_texture_formats" }, GL_COMPRESSED_TEXTURE_FORMATS },
	{ { "context_flags" }, GL_CONTEXT_FLAGS },
	{ { "cull_face" }, GL_CULL_FACE },
	{ { "current_program" }, GL_CURRENT_PROGRAM },
	{ { "debug_group_stack_depth" }, GL_DEBUG_GROUP_STACK_DEPTH },
	{ { "depth_clear_value" }, GL_DEPTH_CLEAR_VALUE },
	{ { "depth_func" }, GL_DEPTH_FUNC },
	{ { "depth_range" }, GL_DEPTH_RANGE },
	{ { "depth_test" }, GL_DEPTH_TEST },
	{ { "depth_writemask" }, GL_DEPTH_WRITEMASK },
	{ { "dispatch_indirect_buffer_binding" }, GL_DISPATCH_INDIRECT_BUFFER_BINDING },
	{ { "dither" }, GL_DITHER },
	{ { "doublebuffer" }, GL_DOUBLEBUFFER },
	{ { "draw_buffer" }, GL_DRAW_BUFFER },
	{ { "draw_framebuffer_binding" }, GL_DRAW_FRAMEBUFFER_BINDING },
	{ { "element_array_buffer_binding" }, GL_ELEMENT_ARRAY_BUFFER_BINDING },
	{ { "fragment_shader_derivative_hint" }, GL_FRAGMENT_SHADER_DERIVATIVE_HINT },
	{ { "implementation_color_read_format" }, GL_IMPLEMENTATION_COLOR_READ_FORMAT },
	{ { "implementation_color_read_type" }, GL_IMPLEMENTATION_COLOR_READ_TYPE },
	{ { "layer_provoking_vertex" }, GL_LAYER_PROVOKING_VERTEX },
	{ { "line_smooth" }, GL_LINE_SMOOTH },
	{ { "line_smooth_hint" }, GL_LINE_SMOOTH_HINT },
	{ { "line_width" }, GL_LINE_WIDTH },
	{ { "logic_op_mode" }, GL_LOGIC_OP_MODE },
	{ { "major_version" }, GL_MAJOR_VERSION },
	{ { "max_3d_texture_size" }, GL_MAX_3D_TEXTURE_SIZE },
	{ { "max_array_texture_layers" }, GL_MAX_ARRAY_TEXTURE_LAYERS },
	{ { "max_clip_distances" }, GL_MAX_CLIP_DISTANCES },
	{ { "max_color_texture_samples" }, GL_MAX_COLOR_TEXTURE_SAMPLES },
	{ { "max_combined_atomic_counters" }, GL_MAX_COMBINED_ATOMIC_COUNTERS },
	{ { "max_combined_compute_uniform_components" }, GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS },
	{ { "max_combined_fragment_uniform_components" }, GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS },
	{ { "max_combined_geometry_uniform_components" }, GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS },
	{ { "max_combined_shader_storage_blocks" }, GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS },
	{ { "max_combined_texture_image_units" }, GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS },
	{ { "max_combined_uniform_blocks" }, GL_MAX_COMBINED_UNIFORM_BLOCKS },
	{ { "max_combined_vertex_uniform_components" }, GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS },
	{ { "max_compute_atomic_counters" }, GL_MAX_COMPUTE_ATOMIC_COUNTERS },
	{ { "max_compute_atomic_counter_buffers" }, GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS },
	{ { "max_compute_shader_storage_blocks" }, GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS },
	{ { "max_compute_texture_image_units" }, GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS },
	{ { "max_compute_uniform_blocks" }, GL_MAX_COMPUTE_UNIFORM_BLOCKS },
	{ { "max_compute_uniform_components" }, GL_MAX_COMPUTE_UNIFORM_COMPONENTS },
	{ { "max_compute_work_group_count" }, GL_MAX_COMPUTE_WORK_GROUP_COUNT },
	{ { "max_compute_work_group_invocations" }, GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS },
	{ { "max_compute_work_group_size" }, GL_MAX_COMPUTE_WORK_GROUP_SIZE },
	{ { "max_cube_map_texture_size" }, GL_MAX_CUBE_MAP_TEXTURE_SIZE },
	{ { "max_debug_group_stack_depth" }, GL_MAX_DEBUG_GROUP_STACK_DEPTH },
	{ { "max_depth_texture_samples" }, GL_MAX_DEPTH_TEXTURE_SAMPLES },
	{ { "max_draw_buffers" }, GL_MAX_DRAW_BUFFERS },
	{ { "max_dual_source_draw_buffers" }, GL_MAX_DUAL_SOURCE_DRAW_BUFFERS },
	{ { "max_elements_indices" }, GL_MAX_ELEMENTS_INDICES },
	{ { "max_elements_vertices" }, GL_MAX_ELEMENTS_VERTICES },
	{ { "max_element_index" }, GL_MAX_ELEMENT_INDEX },
	{ { "max_fragment_atomic_counters" }, GL_MAX_FRAGMENT_ATOMIC_COUNTERS },
	{ { "max_fragment_input_components" }, GL_MAX_FRAGMENT_INPUT_COMPONENTS },
	{ { "max_fragment_shader_storage_blocks" }, GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS },
	{ { "max_fragment_uniform_blocks" }, GL_MAX_FRAGMENT_UNIFORM_BLOCKS },
	{ { "max_fragment_uniform_components" }, GL_MAX_FRAGMENT_UNIFORM_COMPONENTS },
	{ { "max_fragment_uniform_vectors" }, GL_MAX_FRAGMENT_UNIFORM_VECTORS },
	{ { "max_framebuffer_height" }, GL_MAX_FRAMEBUFFER_HEIGHT },
	{ { "max_framebuffer_layers" }, GL_MAX_FRAMEBUFFER_LAYERS },
	{ { "max_framebuffer_samples" }, GL_MAX_FRAMEBUFFER_SAMPLES },
	{ { "max_framebuffer_width" }, GL_MAX_FRAMEBUFFER_WIDTH },
	{ { "max_geometry_atomic_counters" }, GL_MAX_GEOMETRY_ATOMIC_COUNTERS },
	{ { "max_geometry_input_components" }, GL_MAX_GEOMETRY_INPUT_COMPONENTS },
	{ { "max_geometry_output_components" }, GL_MAX_GEOMETRY_OUTPUT_COMPONENTS },
	{ { "max_geometry_shader_storage_blocks" }, GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS },
	{ { "max_geometry_texture_image_units" }, GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS },
	{ { "max_geometry_uniform_blocks" }, GL_MAX_GEOMETRY_UNIFORM_BLOCKS },
	{ { "max_geometry_uniform_components" }, GL_MAX_GEOMETRY_UNIFORM_COMPONENTS },
	{ { "max_integer_samples" }, GL_MAX_INTEGER_SAMPLES },
	{ { "max_label_length" }, GL_MAX_LABEL_LENGTH },
	{ { "max_program_texel_offset" }, GL_MAX_PROGRAM_TEXEL_OFFSET },
	{ { "max_rectangle_texture_size" }, GL_MAX_RECTANGLE_TEXTURE_SIZE },
	{ { "max_renderbuffer_size" }, GL_MAX_RENDERBUFFER_SIZE },
	{ { "max_sample_mask_words" }, GL_MAX_SAMPLE_MASK_WORDS },
	{ { "max_server_wait_timeout" }, GL_MAX_SERVER_WAIT_TIMEOUT },
	{ { "max_shader_storage_buffer_bindings" }, GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS },
	{ { "max_tess_control_atomic_counters" }, GL_MAX_TESS_CONTROL_ATOMIC_COUNTERS },
	{ { "max_tess_control_shader_storage_blocks" }, GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS },
	{ { "max_tess_evaluation_atomic_counters" }, GL_MAX_TESS_EVALUATION_ATOMIC_COUNTERS },
	{ { "max_tess_evaluation_shader_storage_blocks" }, GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS },
	{ { "max_texture_buffer_size" }, GL_MAX_TEXTURE_BUFFER_SIZE },
	{ { "max_texture_image_units" }, GL_MAX_TEXTURE_IMAGE_UNITS },
	{ { "max_texture_lod_bias" }, GL_MAX_TEXTURE_LOD_BIAS },
	{ { "max_texture_size" }, GL_MAX_TEXTURE_SIZE },
	{ { "max_uniform_block_size" }, GL_MAX_UNIFORM_BLOCK_SIZE },
	{ { "max_uniform_buffer_bindings" }, GL_MAX_UNIFORM_BUFFER_BINDINGS },
	{ { "max_uniform_locations" }, GL_MAX_UNIFORM_LOCATIONS },
	{ { "max_varying_components" }, GL_MAX_VARYING_COMPONENTS },
	{ { "max_varying_floats" }, GL_MAX_VARYING_FLOATS },
	{ { "max_varying_vectors" }, GL_MAX_VARYING_VECTORS },
	{ { "max_vertex_atomic_counters" }, GL_MAX_VERTEX_ATOMIC_COUNTERS },
	{ { "max_vertex_attribs" }, GL_MAX_VERTEX_ATTRIBS },
	{ { "max_vertex_attrib_bindings" }, GL_MAX_VERTEX_ATTRIB_BINDINGS },
	{ { "max_vertex_attrib_relative_offset" }, GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET },
	{ { "max_vertex_output_components" }, GL_MAX_VERTEX_OUTPUT_COMPONENTS },
	{ { "max_vertex_shader_storage_blocks" }, GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS },
	{ { "max_vertex_texture_image_units" }, GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS },
	{ { "max_vertex_uniform_blocks" }, GL_MAX_VERTEX_UNIFORM_BLOCKS },
	{ { "max_vertex_uniform_components" }, GL_MAX_VERTEX_UNIFORM_COMPONENTS },
	{ { "max_vertex_uniform_vectors" }, GL_MAX_VERTEX_UNIFORM_VECTORS },
	{ { "max_viewports" }, GL_MAX_VIEWPORTS },
	{ { "max_viewport_dims" }, GL_MAX_VIEWPORT_DIMS },
	{ { "minor_version" }, GL_MINOR_VERSION },
	{ { "min_map_buffer_alignment" }, GL_MIN_MAP_BUFFER_ALIGNMENT },
	{ { "min_program_texel_offset" }, GL_MIN_PROGRAM_TEXEL_OFFSET },
	{ { "num_compressed_texture_formats" }, GL_NUM_COMPRESSED_TEXTURE_FORMATS },
	{ { "num_extensions" }, GL_NUM_EXTENSIONS },
	{ { "num_program_binary_formats" }, GL_NUM_PROGRAM_BINARY_FORMATS },
	{ { "num_shader_binary_formats" }, GL_NUM_SHADER_BINARY_FORMATS },
	{ { "pack_alignment" }, GL_PACK_ALIGNMENT },
	{ { "pack_image_height" }, GL_PACK_IMAGE_HEIGHT },
	{ { "pack_lsb_first" }, GL_PACK_LSB_FIRST },
	{ { "pack_row_length" }, GL_PACK_ROW_LENGTH },
	{ { "pack_skip_images" }, GL_PACK_SKIP_IMAGES },
	{ { "pack_skip_pixels" }, GL_PACK_SKIP_PIXELS },
	{ { "pack_skip_rows" }, GL_PACK_SKIP_ROWS },
	{ { "pack_swap_bytes" }, GL_PACK_SWAP_BYTES },
	{ { "pixel_pack_buffer_binding" }, GL_PIXEL_PACK_BUFFER_BINDING },
	{ { "pixel_unpack_buffer_binding" }, GL_PIXEL_UNPACK_BUFFER_BINDING },
	{ { "point_fade_threshold_size" }, GL_POINT_FADE_THRESHOLD_SIZE },
	{ { "point_size" }, GL_POINT_SIZE },
	{ { "point_size_granularity" }, GL_POINT_SIZE_GRANULARITY },
	{ { "point_size_range" }, GL_POINT_SIZE_RANGE },
	{ { "polygon_offset_factor" }, GL_POLYGON_OFFSET_FACTOR },
	{ { "polygon_offset_fill" }, GL_POLYGON_OFFSET_FILL },
	{ { "polygon_offset_line" }, GL_POLYGON_OFFSET_LINE },
	{ { "polygon_offset_point" }, GL_POLYGON_OFFSET_POINT },
	{ { "polygon_offset_units" }, GL_POLYGON_OFFSET_UNITS },
	{ { "polygon_smooth" }, GL_POLYGON_SMOOTH },
	{ { "polygon_smooth_hint" }, GL_POLYGON_SMOOTH_HINT },
	{ { "primitive_restart_index" }, GL_PRIMITIVE_RESTART_INDEX },
	{ { "program_binary_formats" }, GL_PROGRAM_BINARY_FORMATS },
	{ { "program_pipeline_binding" }, GL_PROGRAM_PIPELINE_BINDING },
	{ { "program_point_size" }, GL_PROGRAM_POINT_SIZE },
	{ { "provoking_vertex" }, GL_PROVOKING_VERTEX },
	{ { "read_buffer" }, GL_READ_BUFFER },
	{ { "read_framebuffer_binding" }, GL_READ_FRAMEBUFFER_BINDING },
	{ { "renderbuffer_binding" }, GL_RENDERBUFFER_BINDING },
	{ { "sampler_binding" }, GL_SAMPLER_BINDING },
	{ { "samples" }, GL_SAMPLES },
	{ { "sample_buffers" }, GL_SAMPLE_BUFFERS },
	{ { "sample_coverage_invert" }, GL_SAMPLE_COVERAGE_INVERT },
	{ { "sample_coverage_value" }, GL_SAMPLE_COVERAGE_VALUE },
	{ { "sample_mask_value" }, GL_SAMPLE_MASK_VALUE },
	{ { "scissor_box" }, GL_SCISSOR_BOX },
	{ { "scissor_test" }, GL_SCISSOR_TEST },
	{ { "shader_compiler" }, GL_SHADER_COMPILER },
	{ { "shader_storage_buffer_binding" }, GL_SHADER_STORAGE_BUFFER_BINDING },
	{ { "shader_storage_buffer_offset_alignment" }, GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT },
	{ { "shader_storage_buffer_size" }, GL_SHADER_STORAGE_BUFFER_SIZE },
	{ { "shader_storage_buffer_start" }, GL_SHADER_STORAGE_BUFFER_START },
	{ { "smooth_line_width_granularity" }, GL_SMOOTH_LINE_WIDTH_GRANULARITY },
	{ { "smooth_line_width_range" }, GL_SMOOTH_LINE_WIDTH_RANGE },
	{ { "stencil_back_fail" }, GL_STENCIL_BACK_FAIL },
	{ { "stencil_back_func" }, GL_STENCIL_BACK_FUNC },
	{ { "stencil_back_pass_depth_fail" }, GL_STENCIL_BACK_PASS_DEPTH_FAIL },
	{ { "stencil_back_pass_depth_pass" }, GL_STENCIL_BACK_PASS_DEPTH_PASS },
	{ { "stencil_back_ref" }, GL_STENCIL_BACK_REF },
	{ { "stencil_back_value_mask" }, GL_STENCIL_BACK_VALUE_MASK },
	{ { "stencil_back_writemask" }, GL_STENCIL_BACK_WRITEMASK },
	{ { "stencil_clear_value" }, GL_STENCIL_CLEAR_VALUE },
	{ { "stencil_fail" }, GL_STENCIL_FAIL },
	{ { "stencil_func" }, GL_STENCIL_FUNC },
	{ { "stencil_pass_depth_fail" }, GL_STENCIL_PASS_DEPTH_FAIL },
	{ { "stencil_pass_depth_pass" }, GL_STENCIL_PASS_DEPTH_PASS },
	{ { "stencil_ref" }, GL_STENCIL_REF },
	{ { "stencil_test" }, GL_STENCIL_TEST },
	{ { "stencil_value_mask" }, GL_STENCIL_VALUE_MASK },
	{ { "stencil_writemask" }, GL_STENCIL_WRITEMASK },
	{ { "stereo" }, GL_STEREO },
	{ { "subpixel_bits" }, GL_SUBPIXEL_BITS },
	{ { "texture_binding_1d" }, GL_TEXTURE_BINDING_1D },
	{ { "texture_binding_1d_array" }, GL_TEXTURE_BINDING_1D_ARRAY },
	{ { "texture_binding_2d" }, GL_TEXTURE_BINDING_2D },
	{ { "texture_binding_2d_array" }, GL_TEXTURE_BINDING_2D_ARRAY },
	{ { "texture_binding_2d_multisample" }, GL_TEXTURE_BINDING_2D_MULTISAMPLE },
	{ { "texture_binding_2d_multisample_array" }, GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY },
	{ { "texture_binding_3d" }, GL_TEXTURE_BINDING_3D },
	{ { "texture_binding_buffer" }, GL_TEXTURE_BINDING_BUFFER },
	{ { "texture_binding_cube_map" }, GL_TEXTURE_BINDING_CUBE_MAP },
	{ { "texture_binding_rectangle" }, GL_TEXTURE_BINDING_RECTANGLE },
	{ { "texture_buffer_offset_alignment" }, GL_TEXTURE_BUFFER_OFFSET_ALIGNMENT },
	{ { "texture_compression_hint" }, GL_TEXTURE_COMPRESSION_HINT },
	{ { "timestamp" }, GL_TIMESTAMP },
	{ { "transform_feedback_buffer_binding" }, GL_TRANSFORM_FEEDBACK_BUFFER_BINDING },
	{ { "transform_feedback_buffer_size" }, GL_TRANSFORM_FEEDBACK_BUFFER_SIZE },
	{ { "transform_feedback_buffer_start" }, GL_TRANSFORM_FEEDBACK_BUFFER_START },
	{ { "uniform_buffer_binding" }, GL_UNIFORM_BUFFER_BINDING },
	{ { "uniform_buffer_offset_alignment" }, GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT },
	{ { "uniform_buffer_size" }, GL_UNIFORM_BUFFER_SIZE },
	{ { "uniform_buffer_start" }, GL_UNIFORM_BUFFER_START },
	{ { "unpack_alignment" }, GL_UNPACK_ALIGNMENT },
	{ { "unpack_image_height" }, GL_UNPACK_IMAGE_HEIGHT },
	{ { "unpack_lsb_first" }, GL_UNPACK_LSB_FIRST },
	{ { "unpack_row_length" }, GL_UNPACK_ROW_LENGTH },
	{ { "unpack_skip_images" }, GL_UNPACK_SKIP_IMAGES },
	{ { "unpack_skip_pixels" }, GL_UNPACK_SKIP_PIXELS },
	{ { "unpack_skip_rows" }, GL_UNPACK_SKIP_ROWS },
	{ { "unpack_swap_bytes" }, GL_UNPACK_SWAP_BYTES },
	{ { "vertex_array_binding" }, GL_VERTEX_ARRAY_BINDING },
	{ { "vertex_binding_divisor" }, GL_VERTEX_BINDING_DIVISOR },
	{ { "vertex_binding_offset" }, GL_VERTEX_BINDING_OFFSET },
	{ { "vertex_binding_stride" }, GL_VERTEX_BINDING_STRIDE },
	{ { "viewport" }, GL_VIEWPORT },
	{ { "viewport_bounds_range" }, GL_VIEWPORT_BOUNDS_RANGE },
	{ { "viewport_index_provoking_vertex" }, GL_VIEWPORT_INDEX_PROVOKING_VERTEX },
	{ { "viewport_subpixel_bits" }, GL_VIEWPORT_SUBPIXEL_BITS },
};

static inline void load_state_field( GLint pname, ig::diag::GLStateField &field ) {

	for (size_t i = 0; i < std::size( GLRegistry ); i++)
	{
		if (GLRegistry[ i ].pname == pname)
		{
			field.name = GLRegistry[ i ].name.data();
			break;
		}
	}

	(void)glGetError();

	glGetIntegerv( pname, &field.integer );
	glGetFloatv( pname, &field.real );

	const GLenum error = glGetError();
	field.supported = error != GL_INVALID_ENUM;
}

namespace ig
{
	namespace diag
	{
		OpenglStateFields OpenglStateFields::create() {
			OpenglStateFields states{};

			load_state_field( GL_ACTIVE_TEXTURE, states.active_texture );
			load_state_field( GL_ALIASED_LINE_WIDTH_RANGE, states.aliased_line_width_range );
			load_state_field( GL_ARRAY_BUFFER_BINDING, states.array_buffer_binding );
			load_state_field( GL_BLEND, states.blend );
			load_state_field( GL_BLEND_COLOR, states.blend_color );
			load_state_field( GL_BLEND_DST_ALPHA, states.blend_dst_alpha );
			load_state_field( GL_BLEND_DST_RGB, states.blend_dst_rgb );
			load_state_field( GL_BLEND_EQUATION_ALPHA, states.blend_equation_alpha );
			load_state_field( GL_BLEND_EQUATION_RGB, states.blend_equation_rgb );
			load_state_field( GL_BLEND_SRC_ALPHA, states.blend_src_alpha );
			load_state_field( GL_BLEND_SRC_RGB, states.blend_src_rgb );
			load_state_field( GL_COLOR_CLEAR_VALUE, states.color_clear_value );
			load_state_field( GL_COLOR_LOGIC_OP, states.color_logic_op );
			load_state_field( GL_COLOR_WRITEMASK, states.color_writemask );
			load_state_field( GL_COMPRESSED_TEXTURE_FORMATS, states.compressed_texture_formats );
			load_state_field( GL_CONTEXT_FLAGS, states.context_flags );
			load_state_field( GL_CULL_FACE, states.cull_face );
			load_state_field( GL_CURRENT_PROGRAM, states.current_program );
			load_state_field( GL_DEBUG_GROUP_STACK_DEPTH, states.debug_group_stack_depth );
			load_state_field( GL_DEPTH_CLEAR_VALUE, states.depth_clear_value );
			load_state_field( GL_DEPTH_FUNC, states.depth_func );
			load_state_field( GL_DEPTH_RANGE, states.depth_range );
			load_state_field( GL_DEPTH_TEST, states.depth_test );
			load_state_field( GL_DEPTH_WRITEMASK, states.depth_writemask );
			load_state_field( GL_DISPATCH_INDIRECT_BUFFER_BINDING, states.dispatch_indirect_buffer_binding );
			load_state_field( GL_DITHER, states.dither );
			load_state_field( GL_DOUBLEBUFFER, states.doublebuffer );
			load_state_field( GL_DRAW_BUFFER, states.draw_buffer );
			load_state_field( GL_DRAW_FRAMEBUFFER_BINDING, states.draw_framebuffer_binding );
			load_state_field( GL_ELEMENT_ARRAY_BUFFER_BINDING, states.element_array_buffer_binding );
			load_state_field( GL_FRAGMENT_SHADER_DERIVATIVE_HINT, states.fragment_shader_derivative_hint );
			load_state_field( GL_IMPLEMENTATION_COLOR_READ_FORMAT, states.implementation_color_read_format );
			load_state_field( GL_IMPLEMENTATION_COLOR_READ_TYPE, states.implementation_color_read_type );
			load_state_field( GL_LAYER_PROVOKING_VERTEX, states.layer_provoking_vertex );
			load_state_field( GL_LINE_SMOOTH, states.line_smooth );
			load_state_field( GL_LINE_SMOOTH_HINT, states.line_smooth_hint );
			load_state_field( GL_LINE_WIDTH, states.line_width );
			load_state_field( GL_LOGIC_OP_MODE, states.logic_op_mode );
			load_state_field( GL_MAJOR_VERSION, states.major_version );
			load_state_field( GL_MAX_3D_TEXTURE_SIZE, states.max_3d_texture_size );
			load_state_field( GL_MAX_ARRAY_TEXTURE_LAYERS, states.max_array_texture_layers );
			load_state_field( GL_MAX_CLIP_DISTANCES, states.max_clip_distances );
			load_state_field( GL_MAX_COLOR_TEXTURE_SAMPLES, states.max_color_texture_samples );
			load_state_field( GL_MAX_COMBINED_ATOMIC_COUNTERS, states.max_combined_atomic_counters );
			load_state_field( GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS, states.max_combined_compute_uniform_components );
			load_state_field( GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS, states.max_combined_fragment_uniform_components );
			load_state_field( GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS, states.max_combined_geometry_uniform_components );
			load_state_field( GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS, states.max_combined_shader_storage_blocks );
			load_state_field( GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, states.max_combined_texture_image_units );
			load_state_field( GL_MAX_COMBINED_UNIFORM_BLOCKS, states.max_combined_uniform_blocks );
			load_state_field( GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS, states.max_combined_vertex_uniform_components );
			load_state_field( GL_MAX_COMPUTE_ATOMIC_COUNTERS, states.max_compute_atomic_counters );
			load_state_field( GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS, states.max_compute_atomic_counter_buffers );
			load_state_field( GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS, states.max_compute_shader_storage_blocks );
			load_state_field( GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS, states.max_compute_texture_image_units );
			load_state_field( GL_MAX_COMPUTE_UNIFORM_BLOCKS, states.max_compute_uniform_blocks );
			load_state_field( GL_MAX_COMPUTE_UNIFORM_COMPONENTS, states.max_compute_uniform_components );
			load_state_field( GL_MAX_COMPUTE_WORK_GROUP_COUNT, states.max_compute_work_group_count );
			load_state_field( GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, states.max_compute_work_group_invocations );
			load_state_field( GL_MAX_COMPUTE_WORK_GROUP_SIZE, states.max_compute_work_group_size );
			load_state_field( GL_MAX_CUBE_MAP_TEXTURE_SIZE, states.max_cube_map_texture_size );
			load_state_field( GL_MAX_DEBUG_GROUP_STACK_DEPTH, states.max_debug_group_stack_depth );
			load_state_field( GL_MAX_DEPTH_TEXTURE_SAMPLES, states.max_depth_texture_samples );
			load_state_field( GL_MAX_DRAW_BUFFERS, states.max_draw_buffers );
			load_state_field( GL_MAX_DUAL_SOURCE_DRAW_BUFFERS, states.max_dual_source_draw_buffers );
			load_state_field( GL_MAX_ELEMENTS_INDICES, states.max_elements_indices );
			load_state_field( GL_MAX_ELEMENTS_VERTICES, states.max_elements_vertices );
			load_state_field( GL_MAX_ELEMENT_INDEX, states.max_element_index );
			load_state_field( GL_MAX_FRAGMENT_ATOMIC_COUNTERS, states.max_fragment_atomic_counters );
			load_state_field( GL_MAX_FRAGMENT_INPUT_COMPONENTS, states.max_fragment_input_components );
			load_state_field( GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS, states.max_fragment_shader_storage_blocks );
			load_state_field( GL_MAX_FRAGMENT_UNIFORM_BLOCKS, states.max_fragment_uniform_blocks );
			load_state_field( GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, states.max_fragment_uniform_components );
			load_state_field( GL_MAX_FRAGMENT_UNIFORM_VECTORS, states.max_fragment_uniform_vectors );
			load_state_field( GL_MAX_FRAMEBUFFER_HEIGHT, states.max_framebuffer_height );
			load_state_field( GL_MAX_FRAMEBUFFER_LAYERS, states.max_framebuffer_layers );
			load_state_field( GL_MAX_FRAMEBUFFER_SAMPLES, states.max_framebuffer_samples );
			load_state_field( GL_MAX_FRAMEBUFFER_WIDTH, states.max_framebuffer_width );
			load_state_field( GL_MAX_GEOMETRY_ATOMIC_COUNTERS, states.max_geometry_atomic_counters );
			load_state_field( GL_MAX_GEOMETRY_INPUT_COMPONENTS, states.max_geometry_input_components );
			load_state_field( GL_MAX_GEOMETRY_OUTPUT_COMPONENTS, states.max_geometry_output_components );
			load_state_field( GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS, states.max_geometry_shader_storage_blocks );
			load_state_field( GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS, states.max_geometry_texture_image_units );
			load_state_field( GL_MAX_GEOMETRY_UNIFORM_BLOCKS, states.max_geometry_uniform_blocks );
			load_state_field( GL_MAX_GEOMETRY_UNIFORM_COMPONENTS, states.max_geometry_uniform_components );
			load_state_field( GL_MAX_INTEGER_SAMPLES, states.max_integer_samples );
			load_state_field( GL_MAX_LABEL_LENGTH, states.max_label_length );
			load_state_field( GL_MAX_PROGRAM_TEXEL_OFFSET, states.max_program_texel_offset );
			load_state_field( GL_MAX_RECTANGLE_TEXTURE_SIZE, states.max_rectangle_texture_size );
			load_state_field( GL_MAX_RENDERBUFFER_SIZE, states.max_renderbuffer_size );
			load_state_field( GL_MAX_SAMPLE_MASK_WORDS, states.max_sample_mask_words );
			load_state_field( GL_MAX_SERVER_WAIT_TIMEOUT, states.max_server_wait_timeout );
			load_state_field( GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, states.max_shader_storage_buffer_bindings );
			load_state_field( GL_MAX_TESS_CONTROL_ATOMIC_COUNTERS, states.max_tess_control_atomic_counters );
			load_state_field( GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS, states.max_tess_control_shader_storage_blocks );
			load_state_field( GL_MAX_TESS_EVALUATION_ATOMIC_COUNTERS, states.max_tess_evaluation_atomic_counters );
			load_state_field( GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS, states.max_tess_evaluation_shader_storage_blocks );
			load_state_field( GL_MAX_TEXTURE_BUFFER_SIZE, states.max_texture_buffer_size );
			load_state_field( GL_MAX_TEXTURE_IMAGE_UNITS, states.max_texture_image_units );
			load_state_field( GL_MAX_TEXTURE_LOD_BIAS, states.max_texture_lod_bias );
			load_state_field( GL_MAX_TEXTURE_SIZE, states.max_texture_size );
			load_state_field( GL_MAX_UNIFORM_BLOCK_SIZE, states.max_uniform_block_size );
			load_state_field( GL_MAX_UNIFORM_BUFFER_BINDINGS, states.max_uniform_buffer_bindings );
			load_state_field( GL_MAX_UNIFORM_LOCATIONS, states.max_uniform_locations );
			load_state_field( GL_MAX_VARYING_COMPONENTS, states.max_varying_components );
			load_state_field( GL_MAX_VARYING_FLOATS, states.max_varying_floats );
			load_state_field( GL_MAX_VARYING_VECTORS, states.max_varying_vectors );
			load_state_field( GL_MAX_VERTEX_ATOMIC_COUNTERS, states.max_vertex_atomic_counters );
			load_state_field( GL_MAX_VERTEX_ATTRIBS, states.max_vertex_attribs );
			load_state_field( GL_MAX_VERTEX_ATTRIB_BINDINGS, states.max_vertex_attrib_bindings );
			load_state_field( GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET, states.max_vertex_attrib_relative_offset );
			load_state_field( GL_MAX_VERTEX_OUTPUT_COMPONENTS, states.max_vertex_output_components );
			load_state_field( GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS, states.max_vertex_shader_storage_blocks );
			load_state_field( GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, states.max_vertex_texture_image_units );
			load_state_field( GL_MAX_VERTEX_UNIFORM_BLOCKS, states.max_vertex_uniform_blocks );
			load_state_field( GL_MAX_VERTEX_UNIFORM_COMPONENTS, states.max_vertex_uniform_components );
			load_state_field( GL_MAX_VERTEX_UNIFORM_VECTORS, states.max_vertex_uniform_vectors );
			load_state_field( GL_MAX_VIEWPORTS, states.max_viewports );
			load_state_field( GL_MAX_VIEWPORT_DIMS, states.max_viewport_dims );
			load_state_field( GL_MINOR_VERSION, states.minor_version );
			load_state_field( GL_MIN_MAP_BUFFER_ALIGNMENT, states.min_map_buffer_alignment );
			load_state_field( GL_MIN_PROGRAM_TEXEL_OFFSET, states.min_program_texel_offset );
			load_state_field( GL_NUM_COMPRESSED_TEXTURE_FORMATS, states.num_compressed_texture_formats );
			load_state_field( GL_NUM_EXTENSIONS, states.num_extensions );
			load_state_field( GL_NUM_PROGRAM_BINARY_FORMATS, states.num_program_binary_formats );
			load_state_field( GL_NUM_SHADER_BINARY_FORMATS, states.num_shader_binary_formats );
			load_state_field( GL_PACK_ALIGNMENT, states.pack_alignment );
			load_state_field( GL_PACK_IMAGE_HEIGHT, states.pack_image_height );
			load_state_field( GL_PACK_LSB_FIRST, states.pack_lsb_first );
			load_state_field( GL_PACK_ROW_LENGTH, states.pack_row_length );
			load_state_field( GL_PACK_SKIP_IMAGES, states.pack_skip_images );
			load_state_field( GL_PACK_SKIP_PIXELS, states.pack_skip_pixels );
			load_state_field( GL_PACK_SKIP_ROWS, states.pack_skip_rows );
			load_state_field( GL_PACK_SWAP_BYTES, states.pack_swap_bytes );
			load_state_field( GL_PIXEL_PACK_BUFFER_BINDING, states.pixel_pack_buffer_binding );
			load_state_field( GL_PIXEL_UNPACK_BUFFER_BINDING, states.pixel_unpack_buffer_binding );
			load_state_field( GL_POINT_FADE_THRESHOLD_SIZE, states.point_fade_threshold_size );
			load_state_field( GL_POINT_SIZE, states.point_size );
			load_state_field( GL_POINT_SIZE_GRANULARITY, states.point_size_granularity );
			load_state_field( GL_POINT_SIZE_RANGE, states.point_size_range );
			load_state_field( GL_POLYGON_OFFSET_FACTOR, states.polygon_offset_factor );
			load_state_field( GL_POLYGON_OFFSET_FILL, states.polygon_offset_fill );
			load_state_field( GL_POLYGON_OFFSET_LINE, states.polygon_offset_line );
			load_state_field( GL_POLYGON_OFFSET_POINT, states.polygon_offset_point );
			load_state_field( GL_POLYGON_OFFSET_UNITS, states.polygon_offset_units );
			load_state_field( GL_POLYGON_SMOOTH, states.polygon_smooth );
			load_state_field( GL_POLYGON_SMOOTH_HINT, states.polygon_smooth_hint );
			load_state_field( GL_PRIMITIVE_RESTART_INDEX, states.primitive_restart_index );
			load_state_field( GL_PROGRAM_BINARY_FORMATS, states.program_binary_formats );
			load_state_field( GL_PROGRAM_PIPELINE_BINDING, states.program_pipeline_binding );
			load_state_field( GL_PROGRAM_POINT_SIZE, states.program_point_size );
			load_state_field( GL_PROVOKING_VERTEX, states.provoking_vertex );
			load_state_field( GL_READ_BUFFER, states.read_buffer );
			load_state_field( GL_READ_FRAMEBUFFER_BINDING, states.read_framebuffer_binding );
			load_state_field( GL_RENDERBUFFER_BINDING, states.renderbuffer_binding );
			load_state_field( GL_SAMPLER_BINDING, states.sampler_binding );
			load_state_field( GL_SAMPLES, states.samples );
			load_state_field( GL_SAMPLE_BUFFERS, states.sample_buffers );
			load_state_field( GL_SAMPLE_COVERAGE_INVERT, states.sample_coverage_invert );
			load_state_field( GL_SAMPLE_COVERAGE_VALUE, states.sample_coverage_value );
			load_state_field( GL_SAMPLE_MASK_VALUE, states.sample_mask_value );
			load_state_field( GL_SCISSOR_BOX, states.scissor_box );
			load_state_field( GL_SCISSOR_TEST, states.scissor_test );
			load_state_field( GL_SHADER_COMPILER, states.shader_compiler );
			load_state_field( GL_SHADER_STORAGE_BUFFER_BINDING, states.shader_storage_buffer_binding );
			load_state_field( GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT, states.shader_storage_buffer_offset_alignment );
			load_state_field( GL_SHADER_STORAGE_BUFFER_SIZE, states.shader_storage_buffer_size );
			load_state_field( GL_SHADER_STORAGE_BUFFER_START, states.shader_storage_buffer_start );
			load_state_field( GL_SMOOTH_LINE_WIDTH_GRANULARITY, states.smooth_line_width_granularity );
			load_state_field( GL_SMOOTH_LINE_WIDTH_RANGE, states.smooth_line_width_range );
			load_state_field( GL_STENCIL_BACK_FAIL, states.stencil_back_fail );
			load_state_field( GL_STENCIL_BACK_FUNC, states.stencil_back_func );
			load_state_field( GL_STENCIL_BACK_PASS_DEPTH_FAIL, states.stencil_back_pass_depth_fail );
			load_state_field( GL_STENCIL_BACK_PASS_DEPTH_PASS, states.stencil_back_pass_depth_pass );
			load_state_field( GL_STENCIL_BACK_REF, states.stencil_back_ref );
			load_state_field( GL_STENCIL_BACK_VALUE_MASK, states.stencil_back_value_mask );
			load_state_field( GL_STENCIL_BACK_WRITEMASK, states.stencil_back_writemask );
			load_state_field( GL_STENCIL_CLEAR_VALUE, states.stencil_clear_value );
			load_state_field( GL_STENCIL_FAIL, states.stencil_fail );
			load_state_field( GL_STENCIL_FUNC, states.stencil_func );
			load_state_field( GL_STENCIL_PASS_DEPTH_FAIL, states.stencil_pass_depth_fail );
			load_state_field( GL_STENCIL_PASS_DEPTH_PASS, states.stencil_pass_depth_pass );
			load_state_field( GL_STENCIL_REF, states.stencil_ref );
			load_state_field( GL_STENCIL_TEST, states.stencil_test );
			load_state_field( GL_STENCIL_VALUE_MASK, states.stencil_value_mask );
			load_state_field( GL_STENCIL_WRITEMASK, states.stencil_writemask );
			load_state_field( GL_STEREO, states.stereo );
			load_state_field( GL_SUBPIXEL_BITS, states.subpixel_bits );
			load_state_field( GL_TEXTURE_BINDING_1D, states.texture_binding_1d );
			load_state_field( GL_TEXTURE_BINDING_1D_ARRAY, states.texture_binding_1d_array );
			load_state_field( GL_TEXTURE_BINDING_2D, states.texture_binding_2d );
			load_state_field( GL_TEXTURE_BINDING_2D_ARRAY, states.texture_binding_2d_array );
			load_state_field( GL_TEXTURE_BINDING_2D_MULTISAMPLE, states.texture_binding_2d_multisample );
			load_state_field( GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY, states.texture_binding_2d_multisample_array );
			load_state_field( GL_TEXTURE_BINDING_3D, states.texture_binding_3d );
			load_state_field( GL_TEXTURE_BINDING_BUFFER, states.texture_binding_buffer );
			load_state_field( GL_TEXTURE_BINDING_CUBE_MAP, states.texture_binding_cube_map );
			load_state_field( GL_TEXTURE_BINDING_RECTANGLE, states.texture_binding_rectangle );
			load_state_field( GL_TEXTURE_BUFFER_OFFSET_ALIGNMENT, states.texture_buffer_offset_alignment );
			load_state_field( GL_TEXTURE_COMPRESSION_HINT, states.texture_compression_hint );
			load_state_field( GL_TIMESTAMP, states.timestamp );
			load_state_field( GL_TRANSFORM_FEEDBACK_BUFFER_BINDING, states.transform_feedback_buffer_binding );
			load_state_field( GL_TRANSFORM_FEEDBACK_BUFFER_SIZE, states.transform_feedback_buffer_size );
			load_state_field( GL_TRANSFORM_FEEDBACK_BUFFER_START, states.transform_feedback_buffer_start );
			load_state_field( GL_UNIFORM_BUFFER_BINDING, states.uniform_buffer_binding );
			load_state_field( GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, states.uniform_buffer_offset_alignment );
			load_state_field( GL_UNIFORM_BUFFER_SIZE, states.uniform_buffer_size );
			load_state_field( GL_UNIFORM_BUFFER_START, states.uniform_buffer_start );
			load_state_field( GL_UNPACK_ALIGNMENT, states.unpack_alignment );
			load_state_field( GL_UNPACK_IMAGE_HEIGHT, states.unpack_image_height );
			load_state_field( GL_UNPACK_LSB_FIRST, states.unpack_lsb_first );
			load_state_field( GL_UNPACK_ROW_LENGTH, states.unpack_row_length );
			load_state_field( GL_UNPACK_SKIP_IMAGES, states.unpack_skip_images );
			load_state_field( GL_UNPACK_SKIP_PIXELS, states.unpack_skip_pixels );
			load_state_field( GL_UNPACK_SKIP_ROWS, states.unpack_skip_rows );
			load_state_field( GL_UNPACK_SWAP_BYTES, states.unpack_swap_bytes );
			load_state_field( GL_VERTEX_ARRAY_BINDING, states.vertex_array_binding );
			load_state_field( GL_VERTEX_BINDING_DIVISOR, states.vertex_binding_divisor );
			load_state_field( GL_VERTEX_BINDING_OFFSET, states.vertex_binding_offset );
			load_state_field( GL_VERTEX_BINDING_STRIDE, states.vertex_binding_stride );
			load_state_field( GL_VIEWPORT, states.viewport );
			load_state_field( GL_VIEWPORT_BOUNDS_RANGE, states.viewport_bounds_range );
			load_state_field( GL_VIEWPORT_INDEX_PROVOKING_VERTEX, states.viewport_index_provoking_vertex );
			load_state_field( GL_VIEWPORT_SUBPIXEL_BITS, states.viewport_subpixel_bits );

			return states;
		}
	}
}

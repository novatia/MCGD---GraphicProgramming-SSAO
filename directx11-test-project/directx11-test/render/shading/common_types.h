#pragma once

namespace xtest {
namespace render {
namespace shading {

	// used to bind a cbuffer to a shader 
	// warning: the value of the enum is used to select the shader slot
	enum class CBufferFrequency
	{
		per_object = 0,
		per_frame = 1,
		rarely_changed = 2,
		per_object_ambient_occlusion = 3,
		unknown

	};


	// used to bind a resource to the corrisponding shader
	enum class ShaderTarget
	{
		vertex_shader,
		pixel_shader,
		unknown
	};


	// used to bind a texture to a shader
	// warning: the value of the enum is used to select the shader slot
	enum class TextureUsage
	{
		color = 0,
		normal = 1,
		glossiness = 2,
		normal_depth_map = 3,
		random_vec_map = 4,
		shadow_map = 10,
		uknown
	};


	// used to bind a texture sampler to a shader
	// warning: the value of the enum is used to select the shader slot
	enum class SamplerUsage
	{
		common_textures = 0,
		normal_depth_map = 1,
		random_vec = 2,
		shadow_map = 10,
		unknown
	};


} // shading
} // render
} // xtest

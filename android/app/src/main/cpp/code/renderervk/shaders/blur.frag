#version 450

// 3-tap gaussian blur
// exploiting linear filtering with -1.2 0 +1.2 texture offsets and 5 6 5 weighting
// to emulate 5-tap blur

layout(set = 0, binding = 0) uniform sampler2DArray texture0;

layout(location = 0) in vec2 tex_coord0;
layout(location = 1) in flat uint view_index;

layout(location = 0) out vec4 out_color;

layout(constant_id = 0) const float texoffset_x = 0.0;
layout(constant_id = 1) const float texoffset_y = 0.0;

#ifdef USE_EXTRACT
// Foveated split's first blur pass: bloom_extract_fov.frag's logic is folded in per tap
layout(constant_id = 3) const float threshold = 0.6;
layout(constant_id = 5) const int extract_mode = 0;
layout(constant_id = 6) const int base_modulate = 0;

vec3 extract( vec3 base )
{
	const vec3 luma = vec3( 0.2126, 0.7152, 0.0722 );
	const float v = dot( luma, base );
	bool bright;

	if ( extract_mode == 1 ) {
		bright = ( base.r + base.g + base.b ) * 0.33333333 >= threshold;
	} else if ( extract_mode == 2 ) {
		bright = v >= threshold;
	} else {
		bright = base.r >= threshold || base.g >= threshold || base.b >= threshold;
	}
	if ( !bright ) {
		return vec3( 0.0 );
	}
	if ( base_modulate == 1 ) {
		return base * base;
	}
	if ( base_modulate != 0 ) {
		return base * v;
	}
	return base;
}
#define TAP( coord ) extract( texture( texture0, coord ).rgb )
#else
#define TAP( coord ) texture( texture0, coord ).rgb
#endif

void main()
{
	vec2 tex_coord1 = tex_coord0;
	vec2 tex_coord2 = tex_coord0;

	tex_coord1.x += texoffset_x;
	tex_coord1.y += texoffset_y;

	tex_coord2.x -= texoffset_x;
	tex_coord2.y -= texoffset_y;

	float layer = float(view_index);
	vec3 base = TAP( vec3(tex_coord0, layer) ) * (6.0 / 16.0)
		+ TAP( vec3(tex_coord1, layer) ) * (5.0 / 16.0)
		+ TAP( vec3(tex_coord2, layer) ) * (5.0 / 16.0);

	out_color = vec4( base, 1.0 );
}

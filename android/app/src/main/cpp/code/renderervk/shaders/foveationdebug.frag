#version 450
#extension GL_EXT_fragment_invocation_density : require

layout(location = 0) out vec4 out_color;

// gl_FragSizeEXT is the fragment the device actually rasterized, not the density we
// asked it for, so this shows what the map became rather than what it said.
void main() {
	int area = gl_FragSizeEXT.x * gl_FragSizeEXT.y;

	vec3 tint;
	if (area <= 1)      tint = vec3(0.0, 0.0, 0.0);   // 1x1, untinted
	else if (area <= 2) tint = vec3(0.0, 1.0, 0.0);   // 2x1 or 1x2
	else if (area <= 4) tint = vec3(1.0, 1.0, 0.0);   // 2x2
	else if (area <= 8) tint = vec3(1.0, 0.5, 0.0);   // 4x2 or 2x4
	else                tint = vec3(1.0, 0.0, 0.0);   // 4x4

	out_color = vec4(tint, area <= 1 ? 0.0 : 0.35);
}

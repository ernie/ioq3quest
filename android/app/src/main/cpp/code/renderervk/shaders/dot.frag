#version 450

// Flare visibility probe: each fragment adds one to the counter the vertex stage picked;
// RB_TestFlare reads both back a frame later and resets them on the CPU.
layout(set = 0, binding = 0) buffer SSBO {
	uint passed;
	uint total;
};

layout(location = 0) flat in int counter;

layout(location = 0) out vec4 out_color;
layout(early_fragment_tests) in; // the depth test must decide before we count

void main() {
	if ( counter != 0 ) {
		atomicAdd( total, 1u );
	} else {
		atomicAdd( passed, 1u );
	}
	discard;
}

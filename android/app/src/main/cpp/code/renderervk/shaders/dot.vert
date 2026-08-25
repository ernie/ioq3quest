#version 450
#extension GL_EXT_multiview : enable

// Flare visibility probe, POINT_LIST variant. This pipeline uses
// vk.pipeline_layout_storage (SSBO only, no eyeProj UBO binding), so the
// caller computes both eyes' clip-space probe positions on the CPU with the
// same eyeProj matrices the scene was rendered with and pushes them here;
// gl_ViewIndex picks the slot. gl_PointSize is required for point topology
// (no maintenance5 on this driver). This ViewIndex+PointSize+POINT_LIST
// combination hangs NVIDIA desktop GPUs (NVIDIA bug 6413598): flip
// FLARE_PROBE_POINT_LIST in vk.h to the triangle variant if Adreno
// misbehaves.
//
// The SSBO reset (sampled = 0) lives on the CPU in RB_TestFlare: multiview
// gives no cross-view ordering between vertex and fragment invocations, so a
// vertex-stage reset could clobber the other view's fragment pass.
layout(push_constant) uniform Transform {
	vec4 clipPos[2];   // per-eye clip-space probe positions
	vec4 params;       // xy: ~2px clip extent per unit w (triangle variant only)
};

layout(location = 0) in vec3 in_position; // unused; satisfies the pipeline's vertex input

out gl_PerVertex {
	vec4 gl_Position;
	float gl_PointSize;
};

void main() {
	gl_Position = clipPos[gl_ViewIndex];
	gl_PointSize = 1.0;
}

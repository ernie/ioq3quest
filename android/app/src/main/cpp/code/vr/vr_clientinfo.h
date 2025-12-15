#if !defined(vr_clientinfo_h)
#define vr_clientinfo_h

#include "vr_base.h"

#define NUM_WEAPON_SAMPLES      10

#define THUMB_LEFT  0
#define THUMB_RIGHT 1

typedef struct {
    float fov_x;
    float fov_y;
    float fov_angle_up;    // Raw OpenXR FOV angle in radians (positive = up)
    float fov_angle_down;  // Raw OpenXR FOV angle in radians (negative = down)
    float fov_angle_left;  // Raw OpenXR FOV angle in radians (negative = left)
    float fov_angle_right; // Raw OpenXR FOV angle in radians (positive = right)

    qboolean weapon_stabilised;
    qboolean weapon_zoomed;
    float weapon_zoomLevel;
    qboolean right_handed;
    qboolean virtual_screen;
    qboolean local_server;
    qboolean single_player;
    qboolean use_fake_6dof;
    vrFollowMode_t follow_mode;
    qboolean weapon_select;
    qboolean weapon_select_autoclose;
    qboolean weapon_select_using_thumbstick;
    qboolean no_crosshair;

    int realign; // used to realign the fake 6DoF playspace in a multiplayer game

    int clientNum;
    vec3_t clientviewangles; //orientation in the client - we use this in the cgame
    float clientview_yaw_last; // Don't use this, it is just for calculating delta!
    float clientview_yaw_delta;

    vec3_t hmdposition;
    vec3_t hmdorigin; //used to recenter the mp fake 6DoF playspace
    vec3_t hmdposition_last; // Don't use this, it is just for calculating delta!
    vec3_t hmdposition_delta;

    vec3_t hmdorientation;
    vec3_t hmdorientation_last; // Don't use this, it is just for calculating delta!
    vec3_t hmdorientation_delta;
	
    vec3_t weaponangles;
    vec3_t calculated_weaponangles; //Calculated as the angle required to hit the point that the controller is pointing at, but coming from the view origin
    vec3_t weaponangles_last; // Don't use this, it is just for calculating delta!
    vec3_t weaponangles_delta;

    vec3_t weaponoffset;
    vec3_t weaponoffset_last[2];
    vec3_t weaponposition;

    vec3_t offhandangles;
    vec3_t offhandangles2;
    vec3_t offhandoffset;
    vec3_t offhandoffset_last[2];
    vec3_t offhandposition;

    vec2_t thumbstick_location[2]; //left / right thumbstick locations - used in cgame

    float menuYaw;
    int *menuCursorX;
    int *menuCursorY;
    qboolean menuLeftHanded;

    float recenterYaw;
    float superSampling;

    qboolean recenter_follow_camera; // flag to trigger camera recentering in follow mode
    float snapTurnYaw;                // yaw rotation to apply (like CL_SnapTurn), set by cgame
    qboolean first_person_following;  // true when in first-person follow mode

    int *scoreboardCursorX;           // cursor X for scoreboard click-to-follow
    int *scoreboardCursorY;           // cursor Y for scoreboard click-to-follow

    // SP intermission overlay anchoring (world-fixed UI during podium view)
    qboolean sp_intermission_active;
    float sp_intermission_yaw;        // The yaw angle the overlay is anchored to

    // Overlay buffer dimensions (may differ from glConfig when supersampling is used)
    int overlayWidth;
    int overlayHeight;
} vr_clientinfo_t;

#endif //vr_clientinfo_h
#ifndef Define_h__
#define Define_h__

#define FRAME_BUFFER_WIDTH				800/*640*/
#define FRAME_BUFFER_HEIGHT				600/*480*/

#define RANDOM_COLOR			XMFLOAT4(rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX))

#define EPSILON					1.0e-10f


#define PLAYER_SPEED					1.5f

#define ANIMATION_TYPE_ONCE				0
#define ANIMATION_TYPE_LOOP				1
#define ANIMATION_TYPE_PINGPONG			2

#define ANIMATION_CALLBACK_EPSILON		0.0165f



//Light.hlsl과 맞추기
//#define MAX_LIGHTS				4 
//#define MAX_DEPTH_TEXTURES		MAX_LIGHTS
#define MAX_LIGHTS						4 
#define MAX_MATERIALS					16 
#define MAX_DEPTH_TEXTURES		MAX_LIGHTS

#define POINT_LIGHT						1
#define SPOT_LIGHT						2
#define DIRECTIONAL_LIGHT				3

#define _DEPTH_BUFFER_WIDTH		(FRAME_BUFFER_WIDTH * 8)
#define _DEPTH_BUFFER_HEIGHT	(FRAME_BUFFER_HEIGHT * 8)

#define DELTA_X					(1.0f / _DEPTH_BUFFER_WIDTH)
#define DELTA_Y					(1.0f / _DEPTH_BUFFER_HEIGHT)

#define _WITH_LOCAL_VIEWER_HIGHLIGHTING
#define _WITH_THETA_PHI_CONES

#define _PLANE_WIDTH			300
#define _PLANE_HEIGHT			300

//RootParameter
#define RP_DEPTH_BUFFER	15
#define RP_TO_LIGHT	16

#endif // Define_h__
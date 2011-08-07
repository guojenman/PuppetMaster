#pragma once
#include "cinder/CinderResources.h"

#define RES_SHADOWMAP_VERT		CINDER_RESOURCE( ../shaders/, shadowMap_vert.glsl, 128, GLSL )
#define RES_SHADOWMAP_FRAG		CINDER_RESOURCE( ../shaders/, shadowMap_frag.glsl, 129, GLSL )
#define RES_RANDTEXTURE			CINDER_RESOURCE( ../resources/, randomNormals.png, 130, IMAGE )


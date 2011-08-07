/*
 
 Cinder Deferred Shading and Post Production Utilities


 SSAO Shader coming from:

 http://www.gamedev.net/topic/556187-the-best-ssao-ive-seen/
 http://www.gamedev.net/page/reference/index.html/_/reference/programming/140/lighting-and-shading/a-simple-and-practical-approach-to-ssao-r2753

 Compositing and Colorimetry Shaders coming from:

 http://mouaif.wordpress.com/2009/01/05/photoshop-math-with-glsl-shaders/


 Copyright (c) 2011, Simon Geilfus
 All rights reserved.

 http://antivj.com | http://naninteractive.com | http://kinesis.be 

 Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and
	the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
	the following disclaimer in the documentation and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
*/
#pragma once

const char* Shader_PostProdVERT = 
	"#version 120  \n"
	"void main() {"
		"gl_TexCoord[0] = gl_MultiTexCoord0;"
		"gl_TexCoord[0].s = gl_TexCoord[0].s;"
		"gl_TexCoord[0].t = gl_TexCoord[0].t;"
		"gl_Position = ftransform();"
		"gl_FrontColor = gl_Color;"
	"} ";


const char* Shader_FillGBufferVERT = 
	"#version 120  \n"
	"varying vec4 pos;"
	"varying vec3 normal;"
	"varying float depth;  \n"

	"varying vec4 albedo;"
	"varying vec4 diffuse;"
	"varying vec4 specular;  \n"
   
	"uniform float near;"
	"uniform float far;  \n"

	"void main(){"
		"pos				= gl_ModelViewMatrix * gl_Vertex;"
		"normal             = normalize( gl_NormalMatrix * gl_Normal );"
		"depth				= (-pos.z-near)/(far-near);  \n"
	
		"albedo				= gl_FrontMaterial.diffuse;"
		"diffuse			= gl_FrontMaterial.diffuse;"
		"specular			= gl_FrontMaterial.specular;"
		"specular.a			= gl_FrontMaterial.shininess;"
		"//albedo = gl_FrontMaterial.emission + (gl_LightModel.ambient * gl_FrontMaterial.ambient);  \n"

		"gl_Position        = gl_ModelViewProjectionMatrix * gl_Vertex;"
		"gl_FrontColor		= gl_Color;"
	"}";

const char* Shader_FillGBufferFRAG = 
	"#version 120  \n"
	"varying vec4 albedo;"
	"varying vec4 diffuse;"
	"varying vec4 specular;"
	"varying vec4 pos;"
	"varying vec3 normal;"
	"varying float depth;  \n"

	"void main(){"
		"gl_FragData[0] = pos;"
		"gl_FragData[1] = vec4(normal.x * 0.5f + 0.5f, normal.y * 0.5f + 0.5f, normal.z * 0.5f + 0.5f, depth );"
		"gl_FragData[2] = gl_Color;"
	"}";


const char* Shader_AmbientOcclusionFRAG = 
	"#version 120 \n"
	"#extension GL_EXT_gpu_shader4 : enable \n"
	"uniform sampler2D albedo;"
	"uniform sampler2D normals;"
	"uniform sampler2D positions;"
	"uniform sampler2D random; \n"

	"uniform float sampleRadius;"
	"uniform float intensity;"
	"uniform float scale;"
	"uniform float bias;"
	"uniform float jitter;"
	"uniform float selfOcclusion;"
	"uniform vec2 screenSize;"
	"uniform vec2 invScreenSize;\n"

	"vec3 getPosition(vec2 uv){"
	  "return texture2D(positions, uv).xyz;"
	"}\n"

	"vec3 getNormal(vec2 uv){"
	  "return normalize(texture2D(normals, uv).xyz * 2.0 - 1.0);"
	"}\n"

	"vec2 getRandom(vec2 uv){"
	  "return texture2D(random, screenSize * uv / jitter ).xy;"
	"}\n"

	"float doAmbientOcclusion(vec2 tcoord, vec2 uv, vec3 p, vec3 cnorm){"
	  "vec3 diff = getPosition(tcoord + uv) - p;"
	  "vec3 v = normalize(diff);"
	  "float d = length(diff)*scale;"
	  "return max(0.0-selfOcclusion,dot(cnorm,v)-bias)*(1.0/(1.0+d*d))*intensity;"
	"}\n"

	"void main(){"
		"vec2 vec[4];"
		"vec[0] = vec2(1,0);"
		"vec[1] = vec2(0,1);"
		"vec[2] = vec2(-1,0);"
		"vec[3] = vec2(0,-1);"
		"vec2 uv = gl_TexCoord[0].st;"
		"uv.y = 1.0 - uv.y;\n"

		"float depth = texture2D(normals, uv).a;"
		"if( depth < 0.00001 || depth > 0.999 ) discard;\n"

		"vec3 position = getPosition(uv);"
		"vec3 normal = getNormal(uv);"
		"vec2 rand = getRandom(uv);"
		"float rad = sampleRadius/position.z;  \n"
	
		"int iterations = 4;"
		"float ao = 0.0;\n"

		"for (int j = 0; j < iterations; ++j) { "
			"vec2 coord1 = reflect(vec[j],rand)*rad; "
			"vec2 coord2 = vec2(coord1.x*0.707 - coord1.y*0.707, coord1.x*0.707 + coord1.y*0.707); "
			"ao += doAmbientOcclusion(uv,coord1*0.5, position, normal); "
			"ao += doAmbientOcclusion(uv,coord2*1.5, position, normal); "
			"ao += doAmbientOcclusion(uv,coord1*3.75, position, normal); "
			"ao += doAmbientOcclusion(uv,coord2, position, normal);  "
		"} \n"

		"ao/=float(iterations)*4.0; "
		"ao+= selfOcclusion;\n"

		"ao = 1.0 - ao;"
		"gl_FragColor = vec4(ao, ao, ao, 1.0);"
	"}";


const char* Shader_VerticalBlurFRAG = 
	"uniform float strength;"
	"uniform sampler2D sceneTex;"
	"void main(){"
		"vec4 color = vec4(0.0);"
		"vec2 uv = gl_TexCoord[0].xy;"
		"uv.y = 1.0 - uv.y;\n"
		"color = texture2D(sceneTex, uv) * 0.2270270270;"
		"color += texture2D(sceneTex, uv + strength*vec2(0.0, 1.3846153846)) * 0.3162162162;"
		"color += texture2D(sceneTex, uv - strength*vec2(0.0, 1.3846153846)) * 0.3162162162;"
		"color += texture2D(sceneTex, uv + strength*vec2(0.0, 3.2307692308)) * 0.0702702703;"
		"color += texture2D(sceneTex, uv - strength*vec2(0.0, 3.2307692308)) * 0.0702702703;"
		"gl_FragColor = color;"
	"}";

const char* Shader_HorizontalBlurFRAG = 
	"uniform float strength;"
	"uniform sampler2D sceneTex;"
	"void main(){"
		"vec4 color = vec4(0.0);"
		"vec2 uv = gl_TexCoord[0].xy;"
		"uv.y = 1.0 - uv.y;\n"
		"color = texture2D(sceneTex, uv) * 0.2270270270;"
		"color += texture2D(sceneTex, uv + strength*vec2(1.3846153846, 0.0)) * 0.3162162162;"
		"color += texture2D(sceneTex, uv - strength*vec2(1.3846153846, 0.0)) * 0.3162162162;"
		"color += texture2D(sceneTex, uv + strength*vec2(3.2307692308, 0.0)) * 0.0702702703;"
		"color += texture2D(sceneTex, uv - strength*vec2(3.2307692308, 0.0)) * 0.0702702703; "
		"gl_FragColor = color;"
	"}";



const char* Shader_VerticalBlurDepthDependantFRAG = 
	"uniform float strength;"
	"uniform sampler2D sceneTex;"
	"uniform sampler2D depth;"
	"void main(){"
		"vec4 color = vec4(0.0);"
		"vec2 uv = gl_TexCoord[0].xy;"
		"uv.y = 1.0 - uv.y;\n"
		"float d =  strength * ( 1.0 - texture2D( depth, uv ).a );"
		"color = texture2D(sceneTex, uv) * 0.2270270270;"
		"color += texture2D(sceneTex, uv + d*vec2(0.0, 1.3846153846)) * 0.3162162162;"
		"color += texture2D(sceneTex, uv - d*vec2(0.0, 1.3846153846)) * 0.3162162162;"
		"color += texture2D(sceneTex, uv + d*vec2(0.0, 3.2307692308)) * 0.0702702703;"
		"color += texture2D(sceneTex, uv - d*vec2(0.0, 3.2307692308)) * 0.0702702703;"
		"gl_FragColor = color;"
	"}";

const char* Shader_HorizontalBlurDepthDependantFRAG = 
	"uniform float strength;"
	"uniform sampler2D sceneTex;"
	"uniform sampler2D depth;"
	"void main(){"
		"vec4 color = vec4(0.0);"
		"vec2 uv = gl_TexCoord[0].xy;"
		"uv.y = 1.0 - uv.y;\n"
		"float d =  strength * ( 1.0 - texture2D( depth, uv ).a );"
		"color = texture2D(sceneTex, uv) * 0.2270270270;"
		"color += texture2D(sceneTex, uv + d*vec2(1.3846153846, 0.0)) * 0.3162162162;"
		"color += texture2D(sceneTex, uv - d*vec2(1.3846153846, 0.0)) * 0.3162162162;"
		"color += texture2D(sceneTex, uv + d*vec2(3.2307692308, 0.0)) * 0.0702702703;"
		"color += texture2D(sceneTex, uv - d*vec2(3.2307692308, 0.0)) * 0.0702702703; "
		"gl_FragColor = color;"
	"}";

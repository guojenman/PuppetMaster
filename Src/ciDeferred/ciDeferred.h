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

#include "cinder/gl/gl.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Texture.h"

#include "cinder/Surface.h"
#include "cinder/ImageIo.h"

#include "cinder/app/AppBasic.h"
#include "cinder/CinderResources.h"
#include "Resources.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace gl;


namespace ciDeferred {

	enum { GBUFFER_POSITION, GBUFFER_NORMAL_DEPTH, GBUFFER_ALBEDO };
	enum { POSTPROD_SSAO, POSTPROD_LIGHTACCUM };
	enum { POSTPROD_SLOT0, POSTPROD_SLOT1, POSTPROD_SLOT2, POSTPROD_SLOT3, POSTPROD_SLOT4 };

	class ciGBuffer {
	public:

		ciGBuffer();
		ciGBuffer( int width, int height );

		void bindBuffers( float nearPlane = 0.1f, float farPlane = 100.0f );
		void unbindBuffers();
	
		Texture& getTexture( GLuint attachment );

		void bindTexture( GLuint attachment, GLuint textureUnit = 0 );
		void unbindTexture( GLuint attachment, GLuint textureUnit = 0 );


	protected:
		Fbo			mFbo;	
		GlslProg	mShader;
	};

	class ciPostProd {
	public:

		ciPostProd();
		ciPostProd( int width, int height, Fbo::Format format = Fbo::Format() );

		void ambientOcclusion( ciGBuffer* source, float sampleRadius = 0.4f, float intensity = 2.5f, float scale = 0.34f, float bias = 0.05f, float jitter = 64.0f, float selfOcclusion = 0.12f );
		void ambientOcclusion( Texture& positions, Texture& normals, Texture& albedo, float sampleRadius = 0.4f, float intensity = 2.5f, float scale = 0.34f, float bias = 0.05f, float jitter = 64.0f, float selfOcclusion = 0.12f );

		void blur( Texture& source, GLuint destination, float strength = 1.0f );
		void blurDepthDependant( Texture& source, Texture& depth, GLuint destination, float strength = 1.0f );

		void verticalBlur( Texture& source, GLuint destination, float strength = 1.0f );
		void horizontalBlur( Texture& source, GLuint destination, float strength = 1.0f );
		void verticalBlurDepthDependant( Texture& source, Texture& depth, GLuint destination, float strength = 1.0f );
		void horizontalBlurDepthDependant( Texture& source, Texture& depth, GLuint destination, float strength = 1.0f );
	
		void colorCorrection( Texture& source, GLuint destination, float contrast, float saturation, float brightness );
		void colorCorrection( Texture& source, GLuint destination, float gamma );
		void colorCorrection( Texture& source, GLuint destination, float gamma, float minInput, float maxInput, float minOutput, float maxOutput );
		void colorCorrection( Texture& source, GLuint destination, float gamma, Vec3f minInput, Vec3f maxInput, Vec3f minOutput, Vec3f maxOutput );
		void colorCorrection( Texture& source, GLuint destination, float contrast, float saturation, float brightness, float gamma, float minInput, float maxInput, float minOutput, float maxOutput );
		void colorCorrection( Texture& source, GLuint destination, float contrast, float saturation, float brightness, float gamma, Vec3f minInput, Vec3f maxInput, Vec3f minOutput, Vec3f maxOutput );
	
		void blend( Texture& sourceA, Texture& sourceB, GLuint destination );

		void blendAndColorCorrection( Texture& sourceA, Texture& sourceB, GLuint destination, float contrast, float saturation, float brightness );
		void blendAndColorCorrection( Texture& sourceA, Texture& sourceB, GLuint destination, float gamma );
		void blendAndColorCorrection( Texture& sourceA, Texture& sourceB, GLuint destination, float gamma, float minInput, float maxInput, float minOutput, float maxOutput );
		void blendAndColorCorrection( Texture& sourceA, Texture& sourceB, GLuint destination, float gamma, Vec3f minInput, Vec3f maxInput, Vec3f minOutput, Vec3f maxOutput );
		void blendAndColorCorrection( Texture& sourceA, Texture& sourceB, GLuint destination, float contrast, float saturation, float brightness, float gamma, float minInput, float maxInput, float minOutput, float maxOutput );
		void blendAndColorCorrection( Texture& sourceA, Texture& sourceB, GLuint destination, float contrast, float saturation, float brightness, float gamma, Vec3f minInput, Vec3f maxInput, Vec3f minOutput, Vec3f maxOutput );
	
		Texture& getTexture( GLuint attachment );

		void bindTexture( GLuint attachment, GLuint textureUnit = 0 );
		void unbindTexture( GLuint attachment, GLuint textureUnit = 0 );

	protected:
		Fbo			mFbo;

		GlslProg	mSSAOShader;
		Texture		mSSAORandTexture;
	
		GlslProg	mDirectionalLightShader;
		GlslProg	mMultiPointLightShader;
		
		GlslProg	mVerticalBlurShader;
		GlslProg	mHorizontalBlurShader;
		GlslProg	mVerticalBlurDepthDependantShader;
		GlslProg	mHorizontalBlurDepthDependantShader;

		GlslProg	mCorrectionConSatBriShader;
		GlslProg	mCorrectionGammaShader;
		GlslProg	mCorrectionLevelsShader;
		GlslProg	mCorrectionLevelsPerChannelShader;
		GlslProg	mCorrectionConSatBriAndLevelsPerChannelShader;
		
		GlslProg	mDepthOfFieldShader[3];
		GlslProg	mCameraMotionBlurShader;
	};

};
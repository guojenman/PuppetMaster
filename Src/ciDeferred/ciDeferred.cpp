#include "ciDeferred.h"
#include "ciDeferredShaders.h"

namespace ciDeferred {

	ciGBuffer::ciGBuffer(){
	}
	ciGBuffer::ciGBuffer( int width, int height ){
		gl::Fbo::Format format;
		format.enableColorBuffer( true, 3 );
		format.enableDepthBuffer( true, false );
		format.setColorInternalFormat( GL_RGBA32F_ARB );
		format.setMinFilter( GL_NEAREST );
		format.setMagFilter( GL_NEAREST );
		format.setWrap( GL_CLAMP, GL_CLAMP );
		
		mFbo = gl::Fbo( width, height, format );

		try { mShader = gl::GlslProg( Shader_FillGBufferVERT, Shader_FillGBufferFRAG ); }
		catch( gl::GlslProgCompileExc exc ) { app::console() << exc.what() << endl; }
	}

	void ciGBuffer::bindBuffers( float nearPlane, float farPlane ){
		mFbo.bindFramebuffer();
		gl::setViewport( mFbo.getBounds() );
		GLenum buffers[] = { GL_COLOR_ATTACHMENT0_EXT + GBUFFER_POSITION, GL_COLOR_ATTACHMENT0_EXT + GBUFFER_NORMAL_DEPTH, GL_COLOR_ATTACHMENT0_EXT + GBUFFER_ALBEDO };
		glDrawBuffers(3, buffers);

		mShader.bind();
		mShader.uniform( "near", nearPlane );
		mShader.uniform( "far", farPlane );
	}
	void ciGBuffer::unbindBuffers(){
		mShader.unbind();
		glDrawBuffer( GL_COLOR_ATTACHMENT0_EXT );
		mFbo.unbindFramebuffer();
	}

	Texture& ciGBuffer::getTexture( GLuint attachment ){
		return mFbo.getTexture( attachment );
	}

	void ciGBuffer::bindTexture( GLuint attachment, GLuint textureUnit ){
		mFbo.getTexture( attachment ).bind( textureUnit );
	}
	void ciGBuffer::unbindTexture( GLuint attachment, GLuint textureUnit ){
		mFbo.getTexture( attachment ).bind( textureUnit );
	}




	ciPostProd::ciPostProd(){
	}
	ciPostProd::ciPostProd( int width, int height, Fbo::Format format ){
	
		// Fbo

		format.enableColorBuffer( true, 5 );
		format.enableDepthBuffer( false, false );
		
		mFbo = gl::Fbo( width, height, format );
	

		// Ambient Occlusion

		try { mSSAOShader = gl::GlslProg( Shader_PostProdVERT, Shader_AmbientOcclusionFRAG ); }
		catch( gl::GlslProgCompileExc exc ) { app::console() << exc.what() << endl; }
	
		gl::Texture::Format texFormat;
		texFormat.setWrap( GL_REPEAT, GL_REPEAT );
		texFormat.setMinFilter( GL_NEAREST );
		texFormat.setMagFilter( GL_NEAREST );

		try{
		mSSAORandTexture = gl::Texture( loadImage( loadResource( RES_RANDTEXTURE ) ), texFormat );
		} catch (ResourceLoadExc exc){ app::console() << exc.what() << endl; }


		// Blur
		
		try { mVerticalBlurShader = gl::GlslProg( Shader_PostProdVERT, Shader_VerticalBlurFRAG ); }
		catch( gl::GlslProgCompileExc exc ) { app::console() << exc.what() << endl; }

		try { mHorizontalBlurShader = gl::GlslProg( Shader_PostProdVERT, Shader_HorizontalBlurFRAG ); }
		catch( gl::GlslProgCompileExc exc ) { app::console() << exc.what() << endl; }
		
		try { mVerticalBlurDepthDependantShader = gl::GlslProg( Shader_PostProdVERT, Shader_VerticalBlurDepthDependantFRAG ); }
		catch( gl::GlslProgCompileExc exc ) { app::console() << exc.what() << endl; }

		try { mHorizontalBlurDepthDependantShader = gl::GlslProg( Shader_PostProdVERT, Shader_HorizontalBlurDepthDependantFRAG ); }
		catch( gl::GlslProgCompileExc exc ) { app::console() << exc.what() << endl; }

	}
	

	void ciPostProd::ambientOcclusion( ciGBuffer* source, float sampleRadius, float intensity, float scale, float bias, float jitter, float selfOcclusion ){
		ambientOcclusion( source->getTexture( GBUFFER_POSITION ), source->getTexture( GBUFFER_NORMAL_DEPTH ), source->getTexture( GBUFFER_ALBEDO ), sampleRadius, intensity, scale, bias, jitter, selfOcclusion );
	}
	void ciPostProd::ambientOcclusion( Texture& positions, Texture& normals, Texture& albedo, float sampleRadius, float intensity, float scale, float bias, float jitter, float selfOcclusion ){
		Area lastViewport = gl::getViewport();

		gl::disableDepthRead();
		gl::disableDepthWrite();

		mFbo.bindFramebuffer();
		glDrawBuffer( GL_COLOR_ATTACHMENT0_EXT + POSTPROD_SSAO );

		gl::clear( ColorA::white(), false );
	
		gl::setViewport( mFbo.getBounds() );
		gl::setMatricesWindow( mFbo.getSize() );
		
		positions.bind( 0 );
		normals.bind( 1 );
		albedo.bind( 2 );

		mSSAORandTexture.bind( 3 );

		mSSAOShader.bind();
	
		mSSAOShader.uniform( "positions", 0 );
		mSSAOShader.uniform( "normals", 1 );
		mSSAOShader.uniform( "albedo", 2 );
		mSSAOShader.uniform( "random", 3 );
	
		mSSAOShader.uniform( "sampleRadius", sampleRadius );
		mSSAOShader.uniform( "intensity", intensity );
		mSSAOShader.uniform( "scale", scale );
		mSSAOShader.uniform( "bias", bias );
		mSSAOShader.uniform( "jitter", jitter );
		mSSAOShader.uniform( "selfOcclusion", selfOcclusion );
		mSSAOShader.uniform( "screenSize", Vec2f( mFbo.getSize() ) );
		mSSAOShader.uniform( "invScreenSize", Vec2f( 1.0f / (float) mFbo.getWidth(), 1.0f / (float) mFbo.getHeight() ) );

		gl::drawSolidRect( mFbo.getBounds() );
	
		mSSAOShader.unbind();

		mSSAORandTexture.unbind( 3 );
		albedo.unbind( 2 );
		normals.unbind( 1 );
		positions.unbind( 0 );
	
		glDrawBuffer( GL_COLOR_ATTACHMENT0_EXT );

		mFbo.unbindFramebuffer();

		gl::setViewport( lastViewport );
	}

	void ciPostProd::blur( Texture& source, GLuint destination, float strength ){
		Area lastViewport = gl::getViewport();
		mFbo.bindFramebuffer();
		glDrawBuffer( GL_COLOR_ATTACHMENT0_EXT + POSTPROD_SLOT3 );
		
		gl::setViewport( mFbo.getBounds() );
		gl::setMatricesWindow( mFbo.getSize() );
	
		source.bind( 0 );

		mVerticalBlurShader.bind();
		mVerticalBlurShader.uniform( "strength", strength * 0.00025f);	
		mVerticalBlurShader.uniform( "sceneTex", 0 );	

		gl::drawSolidRect( mFbo.getBounds() );
		
		mVerticalBlurShader.unbind();

		source.unbind( 0 );


		glDrawBuffer( GL_COLOR_ATTACHMENT0_EXT + destination );

		gl::setViewport( mFbo.getBounds() );
		gl::setMatricesWindow( mFbo.getSize() );
	
		getTexture( POSTPROD_SLOT3 ).bind( 0 );

		mHorizontalBlurShader.bind();
		mHorizontalBlurShader.uniform( "strength", strength * 0.00025f);	
		mHorizontalBlurShader.uniform( "sceneTex", 0 );	

		gl::drawSolidRect( mFbo.getBounds() );

		mHorizontalBlurShader.unbind();	
	
		getTexture( POSTPROD_SLOT3 ).unbind( 0 );
		mFbo.unbindFramebuffer();

		gl::setViewport( lastViewport );
	}

	void ciPostProd::blurDepthDependant( Texture& source, Texture& depth, GLuint destination, float strength ){
		Area lastViewport = gl::getViewport();
		mFbo.bindFramebuffer();
		glDrawBuffer( GL_COLOR_ATTACHMENT0_EXT + POSTPROD_SLOT3 );
		
		gl::setViewport( mFbo.getBounds() );
		gl::setMatricesWindow( mFbo.getSize() );
	
		source.bind( 0 );
		depth.bind( 1 );

		mVerticalBlurDepthDependantShader.bind();
		mVerticalBlurDepthDependantShader.uniform( "strength", strength * 0.00025f);	
		mVerticalBlurDepthDependantShader.uniform( "sceneTex", 0 );	
		mVerticalBlurDepthDependantShader.uniform( "depth", 1 );	

		gl::drawSolidRect( mFbo.getBounds() );
		
		mVerticalBlurDepthDependantShader.unbind();
		
		depth.unbind( 1 );
		source.unbind( 0 );


		glDrawBuffer( GL_COLOR_ATTACHMENT0_EXT + destination );

		gl::setViewport( mFbo.getBounds() );
		gl::setMatricesWindow( mFbo.getSize() );
	
		getTexture( POSTPROD_SLOT3 ).bind( 0 );
		depth.bind( 1 );

		mHorizontalBlurDepthDependantShader.bind();
		mHorizontalBlurDepthDependantShader.uniform( "strength", strength * 0.00025f);	
		mHorizontalBlurDepthDependantShader.uniform( "sceneTex", 0 );	
		mHorizontalBlurDepthDependantShader.uniform( "depth", 1 );	

		gl::drawSolidRect( mFbo.getBounds() );

		mHorizontalBlurDepthDependantShader.unbind();	
	
		depth.unbind( 1 );
		getTexture( POSTPROD_SLOT3 ).unbind( 0 );
		mFbo.unbindFramebuffer();

		gl::setViewport( lastViewport );
	}
	
	void ciPostProd::verticalBlur( Texture& source, GLuint destination, float strength ){
		Area lastViewport = gl::getViewport();
		mFbo.bindFramebuffer();
		glDrawBuffer( GL_COLOR_ATTACHMENT0_EXT + destination );
		
		gl::setViewport( mFbo.getBounds() );
		gl::setMatricesWindow( mFbo.getSize() );
	
		source.bind( 0 );

		mVerticalBlurShader.bind();
		mVerticalBlurShader.uniform( "strength", strength * 0.001f);	
		mVerticalBlurShader.uniform( "sceneTex", 0 );	

		gl::drawSolidRect( mFbo.getBounds() );
		
		mVerticalBlurShader.unbind();

		source.unbind( 0 );
		mFbo.unbindFramebuffer();

		gl::setViewport( lastViewport );
	}
	void ciPostProd::horizontalBlur( Texture& source, GLuint destination, float strength ){
		Area lastViewport = gl::getViewport();
		mFbo.bindFramebuffer();
		glDrawBuffer( GL_COLOR_ATTACHMENT0_EXT + destination );

		gl::setViewport( mFbo.getBounds() );
		gl::setMatricesWindow( mFbo.getSize() );
	
		source.bind( 0 );

		mHorizontalBlurShader.bind();
		mHorizontalBlurShader.uniform( "strength", strength * 0.001f);	
		mHorizontalBlurShader.uniform( "sceneTex", 0 );	

		gl::drawSolidRect( mFbo.getBounds() );

		mHorizontalBlurShader.unbind();	
	
		source.unbind( 0 );
		mFbo.unbindFramebuffer();

		gl::setViewport( lastViewport );
	}
	void ciPostProd::verticalBlurDepthDependant( Texture& source, Texture& depth, GLuint destination, float strength ){
		Area lastViewport = gl::getViewport();
		mFbo.bindFramebuffer();
		glDrawBuffer( GL_COLOR_ATTACHMENT0_EXT + destination );
		
		gl::setViewport( mFbo.getBounds() );
		gl::setMatricesWindow( mFbo.getSize() );
	
		source.bind( 0 );
		depth.bind( 1 );

		mVerticalBlurDepthDependantShader.bind();
		mVerticalBlurDepthDependantShader.uniform( "strength", strength * 0.001f);	
		mVerticalBlurDepthDependantShader.uniform( "sceneTex", 0 );	
		mVerticalBlurDepthDependantShader.uniform( "depth", 1 );	

		gl::drawSolidRect( mFbo.getBounds() );
		
		mVerticalBlurShader.unbind();
		
		depth.unbind( 1 );
		source.unbind( 0 );
		mFbo.unbindFramebuffer();

		gl::setViewport( lastViewport );
	}
	void ciPostProd::horizontalBlurDepthDependant( Texture& source, Texture& depth, GLuint destination, float strength ){
		Area lastViewport = gl::getViewport();
		mFbo.bindFramebuffer();
		glDrawBuffer( GL_COLOR_ATTACHMENT0_EXT + destination );

		gl::setViewport( mFbo.getBounds() );
		gl::setMatricesWindow( mFbo.getSize() );
	
		source.bind( 0 );
		depth.bind( 1 );

		mHorizontalBlurDepthDependantShader.bind();
		mHorizontalBlurDepthDependantShader.uniform( "strength", strength * 0.001f);	
		mHorizontalBlurDepthDependantShader.uniform( "sceneTex", 0 );
		mHorizontalBlurDepthDependantShader.uniform( "depth", 1 );		

		gl::drawSolidRect( mFbo.getBounds() );

		mHorizontalBlurDepthDependantShader.unbind();	
	
		depth.unbind( 1 );
		source.unbind( 0 );
		mFbo.unbindFramebuffer();

		gl::setViewport( lastViewport );
	}
	
	void ciPostProd::colorCorrection( Texture& source, GLuint destination, float contrast, float saturation, float brightness ){
	}
	void ciPostProd::colorCorrection( Texture& source, GLuint destination, float gamma ){
	}
	void ciPostProd::colorCorrection( Texture& source, GLuint destination, float gamma, float minInput, float maxInput, float minOutput, float maxOutput ){
	}
	void ciPostProd::colorCorrection( Texture& source, GLuint destination, float gamma, Vec3f minInput, Vec3f maxInput, Vec3f minOutput, Vec3f maxOutput ){
	}
	void ciPostProd::colorCorrection( Texture& source, GLuint destination, float contrast, float saturation, float brightness, float gamma, float minInput, float maxInput, float minOutput, float maxOutput ){
	}
	void ciPostProd::colorCorrection( Texture& source, GLuint destination, float contrast, float saturation, float brightness, float gamma, Vec3f minInput, Vec3f maxInput, Vec3f minOutput, Vec3f maxOutput ){
	}
	
	Texture& ciPostProd::getTexture( GLuint attachment ){
		return mFbo.getTexture( attachment );
	}

	void ciPostProd::bindTexture( GLuint attachment, GLuint textureUnit ){
		mFbo.getTexture( attachment ).bind( textureUnit );
	}
	void ciPostProd::unbindTexture( GLuint attachment, GLuint textureUnit ){
		mFbo.getTexture( attachment ).unbind( textureUnit );
	}

};

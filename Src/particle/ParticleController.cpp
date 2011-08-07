/*
 * ParticleController.cpp
 *
 *  Created on: Aug 7, 2011
 *      Author: mariogonzalez
 */

#include "ParticleController.h"
#include "cinder/app/App.h"
#include "cinder/ImageIo.h"
#include "cinder/Rand.h"
#include "Resources.h"
using namespace ci;
using namespace ci::app;


// global variables
int			counter = 0;
float		floorLevel = 100.0f;
ci::gl::Texture *particleImg, *emitterImg;
bool		ALLOWFLOOR = false;
bool		ALLOWGRAVITY = false;
bool		ALLOWPERLIN = false;
bool		ALLOWTRAILS = false;
ci::Vec3f		gravity( 0, -0.15f, 0 );
const int	CINDER_FACTOR = 1; // how many times more particles than the Java version

ParticleController::ParticleController() {
	std::string base_path = ci::app::App::get()->getAppPath() + "/Contents/Resources/";

	ALLOWPERLIN = true;
	ALLOWGRAVITY = true;
	ALLOWFLOOR = true;
//ALLOWTRAILS = true;


	//ci::gl::Texture( loadImage( loadResource( RES_PARTICLE ) ) );
	particleImg = new gl::Texture( ci::loadImage( loadResource( RES_PARTICLE ) ) );
	emitterImg = new gl::Texture( ci::loadImage( loadResource( RES_EMITTER ) ) );
	mouseIsDown = false;
}

ParticleController::~ParticleController() {
	// TODO Auto-generated destructor stub

}

void ParticleController::keyDown( ci::app::KeyEvent event )
{
	char key = event.getChar();

	if( ( key == 'g' ) || ( key == 'G' ) )
		ALLOWGRAVITY = ! ALLOWGRAVITY;
	else if( ( key == 'p' ) || ( key == 'P' ) )
		ALLOWPERLIN = ! ALLOWPERLIN;
	else if( ( key == 't' ) || ( key == 'T' ) )
		ALLOWTRAILS = ! ALLOWTRAILS;
	else if( ( key == 'l' ) || ( key == 'L' ) )
		ALLOWFLOOR = ! ALLOWFLOOR;
}



void ParticleController::update( RagDoll* aRagDoll )
{
	glDepthMask( GL_FALSE );
	glDisable( GL_DEPTH_TEST );

	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE );


	// For each emitter, get the position of the limb at that location in the ragdoll
	for(std::vector<Emitter*>::iterator it= _emitters.begin(); it!= _emitters.end(); ++it ) {
		btTransform trans;
		aRagDoll->m_bodies[ (*it)->jointID ]->getMotionState()->getWorldTransform(trans);
		(*it)->exist( ci::Vec3f( trans.getOrigin().getX()*1000, trans.getOrigin().getY()*1000, trans.getOrigin().getZ()*1000 ) );
	}


	glEnable( GL_DEPTH_TEST );
	glDepthMask( GL_TRUE );

	glDisable (GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	ci::gl::color(1, 1, 1);
}


void ParticleController::createEmitterWithJointID( int aJointID ) {
	_emitters.push_back( new Emitter(aJointID) );
}

Emitter* ParticleController::getEmitterWithJointID( int aJointID ) {
	for(std::vector<Emitter*>::iterator it= _emitters.begin(); it!= _emitters.end(); ++it ) {
		if( (*it)->jointID == aJointID )
			return (*it);
	}

	return NULL;
}


void ParticleController::draw()
{
//
//	glClearColor( 0, 0, 0, 0 );
//		glClear( GL_COLOR_BUFFER_BIT );
//
//		// to accommodate resizable screen, we'll recalculate where the floor should be every frame just in case it's changed
//		floorLevel = 2 / 3.0f * getWindowHeight();
//		floorLevel = -1000;
//
//		// Turns on additive blending so we can draw a bunch of glowing images without
//		// needing to do any depth testing.
//		glDepthMask( GL_FALSE );
//		glDisable( GL_DEPTH_TEST );
//		glEnable( GL_BLEND );
//		glBlendFunc( GL_SRC_ALPHA, GL_ONE );
//
//
//		for(std::vector<Emitter*>::iterator it= _emitters.begin(); it!= _emitters.end(); ++it ) {
//			(*it)->exist( ci::Vec2i(300, 100) );
//		}
}

// It would be faster to just make QUADS calls directly to the loc
// without dealing with pushing and popping for every particle. The reason
// I am doing it this longer way is due to a billboarding problem which will come
// up later on.
void renderImage( Vec3f _loc, float _diam, Color _col, float _alpha )
{
	_loc *= 0.001f;
	_diam *= 0.03;
	glPushMatrix();
	glTranslatef( _loc.x, _loc.y, _loc.z );
	glScalef( _diam, _diam, _diam );
	glColor4f( _col.r, _col.g, _col.b, _alpha );
	glBegin( GL_QUADS );
		glTexCoord2f(0, 0);    glVertex2f(-.5, -.5);
		glTexCoord2f(1, 0);    glVertex2f( .5, -.5);
		glTexCoord2f(1, 1);    glVertex2f( .5,  .5);
		glTexCoord2f(0, 1);    glVertex2f(-.5,  .5);
	glEnd();
	glPopMatrix();
}



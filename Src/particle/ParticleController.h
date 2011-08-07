/*
 * ParticleController.h
 *
 *  Created on: Aug 7, 2011
 *      Author: mariogonzalez
 */

#ifndef PARTICLECONTROLLER_H_
#define PARTICLECONTROLLER_H_

#include "cinder/app/KeyEvent.h"
#include "cinder/gl/Texture.h"
#include "cinder/Vector.h"
#include "../RagDoll.h"
#include "Particle.h"
#include "Emitter.h"


class ParticleController {
public:
	ParticleController();
	void update( RagDoll* aRagDoll );
	void draw();
	void keyDown( ci::app::KeyEvent event );

	void createEmitterWithJointID( int aJointID );
	Emitter* getEmitterWithJointID( int aJointID );
	std::vector<Emitter*> _emitters;
	bool mouseIsDown;

	virtual ~ParticleController();
};

#endif /* PARTICLECONTROLLER_H_ */

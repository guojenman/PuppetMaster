/*
 * AudioNode.h
 *
 *  Created on: Aug 6, 2011
 *      Author: mariogonzalez
 */

#ifndef AUDIONODE_H_
#define AUDIONODE_H_

#include <string>
#include "AudioClient.h"
#include "../RagDoll.h"


class AudioNode {
public:
	AudioNode( int aJointID, std::string sampleFile );
	virtual ~AudioNode();
	void update();
	void reset();



	int jointID;
	std::string sampleFile;
	AudioClient client;
};

#endif /* AUDIONODE_H_ */

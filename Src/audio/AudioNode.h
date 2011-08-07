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
#include "XnTypes.h"


class AudioNode {
public:
	AudioNode( XnSkeletonJoint aJointID, std::string sampleFile );
	virtual ~AudioNode();
	void update();



	XnSkeletonJoint jointID;
	std::string sampleFile;
	AudioClient client;
};

#endif /* AUDIONODE_H_ */

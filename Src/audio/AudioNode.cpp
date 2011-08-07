/*
 * AudioNode.cpp
 *
 *  Created on: Aug 6, 2011
 *      Author: mariogonzalez
 */

#include "AudioNode.h"


AudioNode::AudioNode( XnSkeletonJoint aJointID, std::string aSampleFile ) {
	sampleFile = aSampleFile;
	jointID = aJointID;
	client = AudioClient::AudioClient();

	std::cout << sampleFile << std::endl;
//	client.play("Resources/" + sampleFile );
}

void AudioNode::update() {
//	client.update();
}

AudioNode::~AudioNode() {
	// TODO Auto-generated destructor stub
}

/*
 * AudioNode.cpp
 *
 *  Created on: Aug 6, 2011
 *      Author: mariogonzalez
 */

#include "AudioNode.h"


AudioNode::AudioNode( int aJointID, std::string aSampleFile ) {
	sampleFile = aSampleFile;
	jointID = aJointID;
	client = AudioClient::AudioClient();
	client.play(sampleFile );
}

void AudioNode::update() {
	client.update();
}

void AudioNode::reset() {
	client.reset();
}
AudioNode::~AudioNode() {
	// TODO Auto-generated destructor stub
}

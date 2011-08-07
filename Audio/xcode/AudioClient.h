//
//  AudioClient.h
//  Template
//
//  Created by Eric Stallworth on 8/6/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "FmodexPlayer.h"
#include "Resources.h"
#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"

class AudioClient{
public:
    bool isPlaying;
	void start();
	void stop();    
	void update();
	void play(string resource);
    void reset();
    void togglePause();
    void decreaseVolume(float diff);
    void increaseVolume(float diff);
    void decreaseSpeed(float diff);
    void increaseSpeed(float diff);
	bool clientIsReady();
	FmodexPlayer player;
	float speed;
	float pan;	
};
#include "FmodexPlayer.h"
#include "Resources.h"
#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "AudioClient.h"
using namespace ci;
using namespace ci::app;
using namespace std;


void AudioClient::update() {
	FmodexUpdate();
}

void AudioClient::play(string resource) {
	player.loadSound(resource);
	player.setVolume(0.75f);
	player.setLoop(false);
	player.play();
	speed = 1.f;
	pan = 0.5f;
    player.setPan(pan);
	player.setSpeed(speed);
    isPlaying = true;
}

void AudioClient::togglePause()
{
    if(player.bPaused)
    {
        player.setPaused(false);
        isPlaying = true;
    }
    else
    {
        player.setPaused(true);
        isPlaying = false;
    }
}

void AudioClient::stop()
{
    player.setLoop(false);
    isPlaying = false;
    player.stop();
}

void AudioClient::reset()
{
    player.stop();
    isPlaying = false;
	player.setPan(pan);
	player.setSpeed(speed);
    player.setVolume(0.75f);
    player.play();
    isPlaying = true;
}

void AudioClient::decreaseVolume(float diff)
{
    if(player.volume > diff && isPlaying){
        player.setVolume(player.volume - diff);
    }
}

void AudioClient::increaseVolume(float diff)
{
    if(isPlaying){
        player.setVolume(player.volume + diff);
    }
}

void AudioClient::increaseSpeed(float diff)
{
    //mess with pan
    //player.setPan(player.pan + diff);
    if(isPlaying){
        player.setSpeed(player.speed + diff);
    }
}

void AudioClient::decreaseSpeed(float diff)
{
    if(player.speed > diff && isPlaying){
        player.setSpeed(player.speed - diff);
    }
}

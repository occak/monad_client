//
//  Disc.h
//  multiDiscTest
//
//  Created by cemcakmak on 22/02/15.
//
//

#pragma once

#ifndef __multiDiscTest__Disc__
#define __multiDiscTest__Disc__

#include <stdio.h>
#include "ofMain.h"
#include "ofxMSAPerlin.h"
//#include "barPerlin.h"

class Disc{

    public:
    
    void setup();
    void update();
    
    //getter&setter functions
    
    int getDiscIndex() const;
    int setDiscIndex(int value);
    
    float getRadius(int index) const;
    float setRadius(int index, float size);
    
    float getThickness(int index) const;
    void setThickness(int index, float size);
    
    int getDensity(int index) const;
    int setDensity(int index, float newDensity);
    
    float getRotation(int index) const;
    float setRotation(int index, float newRotation);
    //for the groove object to draw
    float getRotationSpeed(int index) const;
    void setRotationSpeed(int index, float addSpeed);
    //for relative speeds
    float getNetRotationSpeed(int index) const;
    void setNetRotationSpeed(int index, float addSpeed);
    
    int getTexture(int index) const;
    int setTexture(int index, int type);
    
    float getPosition(int index) const;
    float setPosition(int index, float newPosition);
    
    float getPosOffset(int index) const;
    float setPosOffset(int index, float newOffset);
    
    float getLife() const;
    float setLife(float cost);
    
    float getEnvelope(int index, int section) const;
    float setEnvelope(int index, int type);
    
    int isMute(int index) const;
    void setMute(int index, int onOff);
    
    int isMoving(int index) const;
    void setMoving(int index, int moving);
    
    int getCounter(int index) const;
    void setCounter(int index, int value);
    
    int getSeed(int index) const;
    void setSeed(int index, int value);
    
    void zMotionSetup(int index, int seed);
//
    float origin = 10;
    int selected = -1;
    vector<int> resetPerlin;
    
private:
    
    int discIndex;
    float life;
    
    vector<int> texture;
    
    vector<float> rotation;
    vector<float> rotationSpeed;
    
    vector<int> discSelected;
    vector<float> radii;
    vector<float> density;
    
    vector<float> zPosition;
    vector<float> posOffset;
    
    //adsr
    vector< vector <float> > envelope;
    
    //mute
    vector<int> mute;
    
    // z-motion
    vector<int> perlin;
    vector<float> counter;
    vector<int> seed;
    vector<msa::Perlin *> zMotion;
    
//    Perlin *barPerlin;
    
    };


#endif /* defined(__multiDiscTest__Disc__) */
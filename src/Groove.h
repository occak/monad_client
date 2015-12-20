//
//  Groove.h
//  multiDiscTest
//
//  Created by cemcakmak on 26/02/15.
//
//

#pragma once

#ifndef __multiDiscTest__Groove__
#define __multiDiscTest__Groove__

#include <stdio.h>
#include <math.h>
#include <iostream>
#include <string>
#include <sstream>
#include "ofMain.h"
#include "Disc.h"
#include "Player.h"




class Groove{
    
public:
    
    void setup(Disc* disc, Player *player, vector<Player *> otherPlayers);
    void update();
    void draw();
    
    vector<ofRectangle> lifeBar;
    
    float flipsideDist = 0.2;;
    bool turn;
    Disc* disc;
    Player* me;
    vector<Player *> otherPlayers;
    
    vector<ofMesh> mesh;
    
};


#endif /* defined(__multiDiscTest__Groove__) */
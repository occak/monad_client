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
#include "ofMain.h"
#include "Disc.h"
#include "Player.h"




class Groove{
    
public:
    
    void setup(Disc* disc, Player *player, vector<Player *> otherPlayers);
    void update();
    void draw();
    
    vector<ofRectangle> lifeBar;
    
    bool turn = true;
    Disc* disc;
    Player* me;
    vector<Player *> otherPlayers;
    
    vector<ofMesh> mesh;
    
};


#endif /* defined(__multiDiscTest__Groove__) */
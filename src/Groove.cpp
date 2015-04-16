//
//  Groove.cpp
//  multiDiscTest
//
//  Created by cemcakmak on 26/02/15.
//
//

#include "Groove.h"

void Groove::setup(Disc* disc, Player* player, vector<Player *> otherPlayers){
    
    //groove should not operate without getting disc
    this->disc = disc;
    this->me = player;
    this->otherPlayers = otherPlayers;
    
    
    for(int i = 0; i < disc->getDiscIndex(); i++){
        
        // meshmeshmesh
        
        ofMesh _mesh;
        _mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
        _mesh.enableIndices();
        mesh.push_back(_mesh);
        
        //generate mesh
        int resolution = 70;
        
        for(int a = 0; a <= 360; a += 360/resolution){
            
            ofVec3f posin(disc->getRadius(i-1)*cos(a*PI/180),
                          disc->getRadius(i-1)*sin(a*PI/180),
                          disc->getPosition(i));
            
            ofVec3f posout(disc->getRadius(i)*cos(a*PI/180),
                           disc->getRadius(i)*sin(a*PI/180),
                           disc->getPosition(i));
            
            mesh[i].addVertex(posin);
            mesh[i].addColor(ofFloatColor(1.0, 1.0, 1.0));
            mesh[i].addVertex(posout);
            mesh[i].addColor(ofFloatColor(1.0, 1.0, 1.0));
            
        }
        
        for(int a = 0; a < mesh[i].getNumVertices(); a++){
            ofVec3f vert = mesh[i].getVertex(a);
            mesh[i].addIndex(a);
        }
    }
    
    // life bar & life
    for(int i = 0; i < otherPlayers.size()+1; i++){
        ofRectangle thisBar;
        lifeBar.push_back(thisBar);
    }
}

void Groove::update(){
    
    ofRectangle myBar;
    float myHeight = ofMap(me->getLife(), 0, 100, 0, ofGetHeight());
    myBar.x = ofGetWidth()/2-10;
    myBar.y = -ofGetHeight()/2+(ofGetHeight()-myHeight);
    myBar.width = 10;
    myBar.height = myHeight;
    lifeBar[0] = myBar;
    
    for(int i = 0; i < otherPlayers.size(); i++){
        ofRectangle thisBar;
        float thisHeight = ofMap(otherPlayers[i]->getLife(), 0, 100, 0, ofGetHeight());
        thisBar.x = (ofGetWidth()/2)-(10*(i+2));
        thisBar.y = -ofGetHeight()/2+(ofGetHeight()-thisHeight);
        thisBar.width = 10;
        thisBar.height = thisHeight;
        lifeBar[i+1] = thisBar;
    }
    
    
    // update mesh position
    for(int i = 0; i < disc->getDiscIndex(); i++){
        
        
        float angle = 0;
        float inc = (360/70);
        for (int j = 0; j < mesh[i].getNumVertices(); j += 2 ) {
            
            ofVec3f vert1 = mesh[i].getVertex(j);
            vert1.x = disc->getRadius(i-1)*cos(angle*PI/180);
            vert1.y = disc->getRadius(i-1)*sin(angle*PI/180);
            vert1.z = disc->getPosition(i)-.1;
            mesh[i].setVertex(j, vert1);
            
            ofVec3f vert2 = mesh[i].getVertex(j+1);
            vert2.x = disc->getRadius(i)*cos(angle*PI/180);
            vert2.y = disc->getRadius(i)*sin(angle*PI/180);
            vert2.z = disc->getPosition(i)-.1;
            mesh[i].setVertex(j+1, vert2);
            
            angle += inc;
            
        }
    }
}


void Groove::draw(){
    
    
    ofSetColor(33);
    ofFill();
    ofSetSphereResolution(20);
    ofDrawSphere(0, 0, disc->origin);
    
    for(int i = 0; i < disc->getDiscIndex(); i++){
        
        
        //rotate
        
        if(turn) ofRotate(disc->setRotation(i, disc->getRotationSpeed(i)), 0, 0, 1);
        else ofRotate(disc->getRotation(i), 0, 0, 1);
        
        
        mesh[i].draw();
        
        //draw circles
        ofSetLineWidth(3);
        ofNoFill();
        ofSetColor(33);
        if( i != me->getDiscIndex() && disc->isMute(i) == 1) ofSetColor(ofColor::lightGrey);
        if( otherPlayers.size() > 0){
            for (int j = 0; j < otherPlayers.size(); j++) {
                if( i == otherPlayers[j]->getDiscIndex() && disc->isMute(otherPlayers[j]->getDiscIndex()) == 0) ofSetColor(otherPlayers[j]->getColor());
                else if( i == otherPlayers[j]->getDiscIndex() && disc->isMute(otherPlayers[j]->getDiscIndex()) == 1) ofSetColor(ofColor::lightPink);
                else continue;
            }
        }
        if( i == me->getDiscIndex() && disc->isMute(me->getDiscIndex()) == 0) ofSetColor(me->getColor());
        if( i == me->getDiscIndex() && disc->isMute(me->getDiscIndex()) == 1) ofSetColor(ofColor::lightPink);
        
        ofSetCircleResolution(70);
        ofCircle(0,0,disc->getPosition(i), disc->getRadius(i-1));
        ofCircle(0,0,disc->getPosition(i), disc->getRadius(i));
        
        //        if( i != me->getDiscIndex() && disc->isMute(i) == 1) ofSetColor(ofColor::lightGrey);
        //        else if( i == me->getDiscIndex() && disc->isMute(me->getDiscIndex()) == 1) ofSetColor(ofColor::lightPink);
        //        else if( i == me->getDiscIndex() && disc->isMute(me->getDiscIndex()) == 0) ofSetColor(me->getColor());
        //        else if( otherPlayers.size() > 0){
        //            for (int j = 0; j < otherPlayers.size(); j++) {
        //                if( i == otherPlayers[j]->getDiscIndex() && disc->isMute(otherPlayers[j]->getDiscIndex()) == 0) ofSetColor(otherPlayers[j]->getColor());
        //                else if( i == otherPlayers[j]->getDiscIndex() && disc->isMute(otherPlayers[j]->getDiscIndex()) == 1) ofSetColor(ofColor::lightPink);
        //                else continue;
        //            }
        //        }
        //        else ofSetColor(33);
        ofFill();
        
        //get texture type and draw
        
        switch(disc->getTexture(i)){
                
            case 0: //blank
                
                break;
                
            case 1: //line
                
                for(int a = 0; a < 360; a++){
                    
                    if(a % disc->getDensity(i) == 0){
                        
                        ofLine( disc->getRadius(i-1)*cos(a*PI/180) ,  //x1
                               disc->getRadius(i-1)*sin(a*PI/180),    //y1
                               disc->getPosition(i),                  //z1
                               disc->getRadius(i)*cos(a*PI/180),      //x2
                               disc->getRadius(i)*sin(a*PI/180),      //y2
                               disc->getPosition(i));                 //z2
                        
                    }
                }
                
                break;
                
            case 2: //triangle
                
                for(int a = 0; a < 360; a++){
                    
                    if(a % disc->getDensity(i) == 0){
                        
                        ofSetPolyMode(OF_POLY_WINDING_ODD);
                        ofBeginShape();
                        ofVertex(disc->getRadius(i-1)*cos((a - disc->getDensity(i))*PI/180),
                                 disc->getRadius(i-1)*sin((a - disc->getDensity(i))*PI/180),
                                 disc->getPosition(i));
                        ofVertex(disc->getRadius(i)*cos((a - disc->getDensity(i)/2)*PI/180),
                                 disc->getRadius(i)*sin((a - disc->getDensity(i)/2)*PI/180),
                                 disc->getPosition(i));
                        ofVertex(disc->getRadius(i-1)*cos(a*PI/180),
                                 disc->getRadius(i-1)*sin(a*PI/180),
                                 disc->getPosition(i));
                        ofVertex(disc->getRadius(i-1)*cos((a - disc->getDensity(i)/2)*PI/180),
                                 disc->getRadius(i-1)*sin((a - disc->getDensity(i)/2)*PI/180),
                                 disc->getPosition(i));
                        ofEndShape();
                        
                    }
                }
                
                break;
            case 3: //saw
                
                for(int a = 0; a < 360; a++){
                    
                    if(a % disc->getDensity(i) == 0){
                        
                        ofSetPolyMode(OF_POLY_WINDING_ODD);
                        ofBeginShape();
                        ofVertex(disc->getRadius(i)*cos((a - disc->getDensity(i))*PI/180),        //x1
                                 disc->getRadius(i)*sin((a - disc->getDensity(i))*PI/180),        //y1
                                 disc->getPosition(i));
                        ofVertex(disc->getRadius(i-1)*cos((a - disc->getDensity(i))*PI/180),    //x2
                                 disc->getRadius(i-1)*sin((a - disc->getDensity(i))*PI/180),    //y2
                                 disc->getPosition(i));
                        ofVertex(disc->getRadius(i)*cos(a*PI/180),                                //x3
                                 disc->getRadius(i)*sin(a*PI/180),                                //y3
                                 disc->getPosition(i));
                        ofVertex(disc->getRadius(i)*cos((a - disc->getDensity(i)/2)*PI/180),
                                 disc->getRadius(i)*sin((a - disc->getDensity(i)/2)*PI/180),
                                 disc->getPosition(i));
                        ofEndShape();
                        
                    }
                }
                
                break;
                
            {
            case 4: //rectangle
                
                bool space = false;
                
                for(int a = 0; a < 360; a++){
                    
                    if(space == false && a % disc->getDensity(i) == 0){
                        
                        ofSetPolyMode(OF_POLY_WINDING_ODD);
                        ofBeginShape();
                        ofVertex(disc->getRadius(i-1)*cos((a - disc->getDensity(i))*PI/180),
                                 disc->getRadius(i-1)*sin((a - disc->getDensity(i))*PI/180),
                                 disc->getPosition(i));
                        ofVertex(disc->getRadius(i-1)*cos((a - disc->getDensity(i)/2)*PI/180),
                                 disc->getRadius(i-1)*sin((a - disc->getDensity(i)/2)*PI/180),
                                 disc->getPosition(i));
                        ofVertex(disc->getRadius(i-1)*cos(a*PI/180),
                                 disc->getRadius(i-1)*sin(a*PI/180),
                                 disc->getPosition(i));
                        ofVertex(disc->getRadius(i)*cos(a*PI/180),
                                 disc->getRadius(i)*sin(a*PI/180),
                                 disc->getPosition(i));
                        ofVertex(disc->getRadius(i)*cos((a - disc->getDensity(i)/2)*PI/180),
                                 disc->getRadius(i)*sin((a - disc->getDensity(i)/2)*PI/180),
                                 disc->getPosition(i));
                        ofVertex(disc->getRadius(i)*cos((a - disc->getDensity(i))*PI/180),
                                 disc->getRadius(i)*sin((a - disc->getDensity(i))*PI/180),
                                 disc->getPosition(i));
                        
                        
                        ofEndShape();
                        
                        space = true;
                    }
                    else if (space == true && a % disc->getDensity(i) == 0) space = false;
                    
                }
                break;
            }
                
                
            default:
                //something something
                break;
        }
    }
}
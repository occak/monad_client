//
//  Groove.cpp
//  multiDiscTest
//
//  Created by cemcakmak on 26/02/15.
//
//

#include "Groove.h"

void Groove::setup(Disc* disc, Player* player, vector<Player *> otherPlayers){
    
	turn = true;
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
    float myHeight = ofMap(me->getLife(), 0, 100, 0, ofGetHeight()-50);
    myBar.x = ofGetWidth()/2-20;
    myBar.y = -ofGetHeight()/2+(ofGetHeight()-myHeight);
    myBar.width = 20;
    myBar.height = myHeight;
    lifeBar[0] = myBar;
    
    for(int i = 0; i < otherPlayers.size(); i++){
        ofRectangle thisBar;
        float thisHeight = ofMap(otherPlayers[i]->getLife(), 0, 100, 0, ofGetHeight()-50);
        thisBar.x = (ofGetWidth()/2)-(20*(i+2));
        thisBar.y = -ofGetHeight()/2+(ofGetHeight()-thisHeight);
        thisBar.width = 20;
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
    
    
    ofSetColor(50);
    ofFill();
    ofSetSphereResolution(20);
    ofDrawSphere(0, 0, disc->origin);
    
    for(int i = 0; i < disc->getDiscIndex(); i++){
        
        
        //rotate
        disc->setRotation(i, disc->getRotationSpeed(i));
        if(turn) ofRotate(disc->getRotation(i), 0, 0, 1);
        else ofRotate(disc->getRotation(i), 0, 0, 1);
        
        
        mesh[i].draw();
        
        //draw circles
        ofSetLineWidth(3);
        ofNoFill();
        ofSetColor(50);
        if( i != me->getDiscIndex() && disc->isMute(i) == 1) ofSetColor(ofColor::lightGrey);
        if( otherPlayers.size() > 0){
            for (int j = 0; j < otherPlayers.size(); j++) {
                if( i == otherPlayers[j]->getDiscIndex() && disc->isMute(otherPlayers[j]->getDiscIndex()) == 0) ofSetColor(otherPlayers[j]->getColor());
                else if( i == otherPlayers[j]->getDiscIndex() && disc->isMute(otherPlayers[j]->getDiscIndex()) == 1){
                    ofColor pale = otherPlayers[j]->getColor();
                    pale.setSaturation(pale.getSaturation() - 110);
                    ofSetColor(pale);
                }
            }
        }
        if( i == me->getDiscIndex() && disc->isMute(me->getDiscIndex()) == 0) ofSetColor(me->getColor());
        if( i == me->getDiscIndex() && disc->isMute(me->getDiscIndex()) == 1) {
            ofColor pale = me->getColor();
            pale.setSaturation(pale.getSaturation() - 110);
            ofSetColor(pale);
        }
        
        ofSetCircleResolution(70);
        //front
        ofCircle(0,0,disc->getPosition(i), disc->getRadius(i-1));
        ofCircle(0,0,disc->getPosition(i), disc->getRadius(i));
        //back
        ofCircle(0,0,disc->getPosition(i)-.2, disc->getRadius(i-1));
        ofCircle(0,0,disc->getPosition(i)-.2, disc->getRadius(i));
        
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
                    
                    if(fmodf(a, disc->getDensity(i)) == 0){
                        
                        double spikePoint = (disc->getRadius(i)+disc->getRadius(i-1))/2;
                        ofPoint p1, p2, p3;
                        ofPoint rp1, rp2, rp3;
                        
                        p1.set(disc->getRadius(i-1)*cos(a*PI/180) ,  //x1
                               disc->getRadius(i-1)*sin(a*PI/180),    //y1
                               disc->getPosition(i));
                        p2.set(disc->getRadius(i)*cos(a*PI/180),      //x2
                               disc->getRadius(i)*sin(a*PI/180),      //y2
                               disc->getPosition(i));
                        p3.set(spikePoint*cos(a*PI/180),
                               spikePoint*sin(a*PI/180),
                               disc->getPosition(i)+ disc->getSpikeDistance(i));
                        
                        rp1.set(p1.x, p1.y, p1.z-flipsideDist);
                        rp2.set(p2.x, p2.y, p2.z-flipsideDist);
                        rp3.set(p3.x, p3.y, p3.z-(flipsideDist+2*disc->getSpikeDistance(i)));
                        
                        
                        ofLine(p1,p2);
                        
                        ofBeginShape();
                        ofVertex(p1);
                        ofVertex(p2);
                        ofVertex(p3);
                        ofEndShape();
                        
                        //reverse
                        ofLine(rp1,rp2);   
                        ofBeginShape();
                        ofVertex(rp1);
                        ofVertex(rp2);
                        ofVertex(rp3);
                        ofEndShape();
                        
                    }
                }
                
                break;
                
            case 2: //triangle
                
                for(int a = 0; a < 360; a++){
                    
                    if(fmodf(a, disc->getDensity(i)) == 0){
                        
                        //spike locted at center of gravity
                        double spikePoint = (disc->getRadius(i-1)+(disc->getRadius(i)-disc->getRadius(i-1))/3);
                        
                        ofPoint p1, p2, p3, p4, p5;
                        p1.set(disc->getRadius(i-1)*cos((a - disc->getDensity(i))*PI/180),
                               disc->getRadius(i-1)*sin((a - disc->getDensity(i))*PI/180),
                               disc->getPosition(i));
                        p2.set(disc->getRadius(i-1)*cos((a - disc->getDensity(i)/2)*PI/180),
                               disc->getRadius(i-1)*sin((a - disc->getDensity(i)/2)*PI/180),
                               disc->getPosition(i));
                        p3.set(disc->getRadius(i-1)*cos(a*PI/180),
                               disc->getRadius(i-1)*sin(a*PI/180),
                               disc->getPosition(i));
                        p4.set(disc->getRadius(i)*cos((a - disc->getDensity(i)/2)*PI/180),
                               disc->getRadius(i)*sin((a - disc->getDensity(i)/2)*PI/180),
                               disc->getPosition(i));
                        p5.set(spikePoint*cos((a - disc->getDensity(i)/2)*PI/180),
                               spikePoint*sin((a - disc->getDensity(i)/2)*PI/180),
                               disc->getPosition(i) + disc->getSpikeDistance(i));
                        
                        ofPoint rp1, rp2, rp3, rp4, rp5;
                        rp1.set(p1.x, p1.y, p1.z-flipsideDist);
                        rp2.set(p2.x, p2.y, p2.z-flipsideDist);
                        rp3.set(p3.x, p3.y, p3.z-flipsideDist);
                        rp4.set(p4.x, p4.y, p4.z-flipsideDist);
                        rp5.set(p5.x, p5.y, p5.z-(flipsideDist+2*disc->getSpikeDistance(i)));
                        
                        
                        
                        ofSetPolyMode(OF_POLY_WINDING_ODD);
                        
                        
                        ofBeginShape(); //base
                        ofVertex(p1);
                        ofVertex(p2);
                        ofVertex(p3);
                        ofVertex(p4);
                        ofEndShape();
                        
                        ofBeginShape(); //spike
                        ofVertex(p4);
                        ofVertex(p5);
                        ofVertex(p3);
                        ofEndShape();
                        
                        ofBeginShape(); //spike
                        ofVertex(p4);
                        ofVertex(p5);
                        ofVertex(p1);
                        ofEndShape();
                        
                        //reverse
                        ofBeginShape(); //base
                        ofVertex(rp1);
                        ofVertex(rp2);
                        ofVertex(rp3);
                        ofVertex(rp4);
                        ofEndShape();
                        
                        ofBeginShape(); //spike
                        ofVertex(rp4);
                        ofVertex(rp5);
                        ofVertex(rp3);
                        ofEndShape();
                        
                        ofBeginShape(); //spike
                        ofVertex(rp4);
                        ofVertex(rp5);
                        ofVertex(rp1);
                        ofEndShape();
                    }
                }
                
                break;
            case 3: //saw
                
                for(int a = 0; a < 360; a++){
                    
                    if(fmodf(a, disc->getDensity(i)) == 0){
                        
                        
                        //spike locted at center of gravity
                        double spikePoint = (disc->getRadius(i-1)+(disc->getRadius(i)-disc->getRadius(i-1))/3);
                        
                        ofPoint p1, p2, p3, p4, p5;
                        p1.set(disc->getRadius(i)*cos((a - disc->getDensity(i))*PI/180),
                               disc->getRadius(i)*sin((a - disc->getDensity(i))*PI/180),
                               disc->getPosition(i));
                        p2.set(disc->getRadius(i)*cos((a - disc->getDensity(i)/2)*PI/180),
                               disc->getRadius(i)*sin((a - disc->getDensity(i)/2)*PI/180),
                               disc->getPosition(i));
                        p3.set(disc->getRadius(i)*cos(a*PI/180),
                               disc->getRadius(i)*sin(a*PI/180),
                               disc->getPosition(i));
                        p4.set(disc->getRadius(i-1)*cos((a - disc->getDensity(i))*PI/180),                                  disc->getRadius(i-1)*sin((a - disc->getDensity(i))*PI/180),
                               disc->getPosition(i));
                        p5.set(spikePoint*cos((a - 2*disc->getDensity(i)/3)*PI/180),
                               spikePoint*sin((a - 2*disc->getDensity(i)/3)*PI/180),
                               disc->getPosition(i) + disc->getSpikeDistance(i));
                        
                        ofPoint rp1, rp2, rp3, rp4, rp5;
                        rp1.set(p1.x, p1.y, p1.z-flipsideDist);
                        rp2.set(p2.x, p2.y, p2.z-flipsideDist);
                        rp3.set(p3.x, p3.y, p3.z-flipsideDist);
                        rp4.set(p4.x, p4.y, p4.z-flipsideDist);
                        rp5.set(p5.x, p5.y, p5.z-(flipsideDist+2*disc->getSpikeDistance(i)));
                        
                        
                        ofSetPolyMode(OF_POLY_WINDING_ODD);
                        ofBeginShape(); //base
                        ofVertex(p1);
                        ofVertex(p2);
                        ofVertex(p3);
                        ofVertex(p4);
                        ofEndShape();
                        
                        ofBeginShape(); //spike
                        ofVertex(p4);
                        ofVertex(p5);
                        ofVertex(p3);
                        ofEndShape();
                        
                        ofBeginShape(); //spike
                        ofVertex(p4);
                        ofVertex(p5);
                        ofVertex(p1);
                        ofEndShape();
                        
                        //reverse side
                        ofBeginShape(); //base
                        ofVertex(rp1);
                        ofVertex(rp2);
                        ofVertex(rp3);
                        ofVertex(rp4);
                        ofEndShape();
                        
                        ofBeginShape(); //spike
                        ofVertex(rp4);
                        ofVertex(rp5);
                        ofVertex(rp3);
                        ofEndShape();
                        
                        ofBeginShape(); //spike
                        ofVertex(rp4);
                        ofVertex(rp5);
                        ofVertex(rp1);
                        ofEndShape();
                        
                    }
                }
                
                break;
                
            {
            case 4: //rectangle
                
                bool space = false;
                
                for(int a = 0; a < 360; a++){
                    
                    if(space == false && fmodf(a, disc->getDensity(i)) == 0){
                        
                        //spike locted at center of gravity
                        double spikePoint = (disc->getRadius(i)+disc->getRadius(i-1))/2;
                        
                        ofPoint p1, p2, p3, p4, p5, p6, p7;
                        p1.set(disc->getRadius(i-1)*cos((a - disc->getDensity(i))*PI/180),
                               disc->getRadius(i-1)*sin((a - disc->getDensity(i))*PI/180),
                               disc->getPosition(i));
                        p2.set(disc->getRadius(i-1)*cos((a - disc->getDensity(i)/2)*PI/180),
                               disc->getRadius(i-1)*sin((a - disc->getDensity(i)/2)*PI/180),
                               disc->getPosition(i));
                        p3.set(disc->getRadius(i-1)*cos(a*PI/180),
                               disc->getRadius(i-1)*sin(a*PI/180),
                               disc->getPosition(i));
                        p4.set(disc->getRadius(i)*cos(a*PI/180),
                               disc->getRadius(i)*sin(a*PI/180),
                               disc->getPosition(i));
                        p5.set(disc->getRadius(i)*cos((a - disc->getDensity(i)/2)*PI/180),
                               disc->getRadius(i)*sin((a - disc->getDensity(i)/2)*PI/180),
                               disc->getPosition(i));
                        p6.set(disc->getRadius(i)*cos((a - disc->getDensity(i))*PI/180),
                               disc->getRadius(i)*sin((a - disc->getDensity(i))*PI/180),
                               disc->getPosition(i));
                        p7.set(spikePoint*cos((a - disc->getDensity(i)/2)*PI/180),
                               spikePoint*sin((a - disc->getDensity(i)/2)*PI/180),
                               disc->getPosition(i) + disc->getSpikeDistance(i));
                        
                        ofPoint rp1, rp2, rp3, rp4, rp5, rp6, rp7;
                        rp1.set(p1.x, p1.y, p1.z-flipsideDist);
                        rp2.set(p2.x, p2.y, p2.z-flipsideDist);
                        rp3.set(p3.x, p3.y, p3.z-flipsideDist);
                        rp4.set(p4.x, p4.y, p4.z-flipsideDist);
                        rp5.set(p5.x, p5.y, p5.z-flipsideDist);
                        rp6.set(p6.x, p6.y, p6.z-flipsideDist);
                        rp7.set(p7.x, p7.y, p7.z-(flipsideDist+2*disc->getSpikeDistance(i)));
                        
                        
                        
                        ofSetPolyMode(OF_POLY_WINDING_ODD);
                        
                        ofBeginShape(); //base
                        ofVertex(p1);
                        ofVertex(p2);
                        ofVertex(p3);
                        ofVertex(p4);
                        ofVertex(p5);
                        ofVertex(p6);
                        ofEndShape();
                        
                        ofBeginShape(); //spike
                        ofVertex(p6);
                        ofVertex(p7);
                        ofVertex(p4);
                        ofEndShape();
                        
                        ofBeginShape();
                        ofVertex(p6);
                        ofVertex(p7);
                        ofVertex(p1);
                        ofEndShape();
                        
                        ofBeginShape();
                        ofVertex(p4);
                        ofVertex(p7);
                        ofVertex(p3);
                        ofEndShape();
                        
                        //reverse
                        ofBeginShape(); //base
                        ofVertex(rp1);
                        ofVertex(rp2);
                        ofVertex(rp3);
                        ofVertex(rp4);
                        ofVertex(rp5);
                        ofVertex(rp6);
                        ofEndShape();
                        
                        ofBeginShape(); //spike
                        ofVertex(rp6);
                        ofVertex(rp7);
                        ofVertex(rp4);
                        ofEndShape();
                        
                        ofBeginShape();
                        ofVertex(rp6);
                        ofVertex(rp7);
                        ofVertex(rp1);
                        ofEndShape();
                        
                        ofBeginShape();
                        ofVertex(rp4);
                        ofVertex(rp7);
                        ofVertex(rp3);
                        ofEndShape();
                        
                        space = true;
                    }
                    else if (space == true && fmodf(a, disc->getDensity(i)) == 0) space = false;
                    
                }
                break;
            }
                
                
            default:
                //something something
                break;
        }
    }
}
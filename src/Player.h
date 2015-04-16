//
//  Player.h
//  varianet_server
//
//  Created by cemcakmak on 10/04/15.
//
//

#ifndef __varianet_server__Player__
#define __varianet_server__Player__

#include <stdio.h>
#include "ofMain.h"
#include "Disc.h"

class Player{
    
public:
    
    void setup();
    
    bool isConnected() const;
    void setConnection(bool connection);
    
    string getIP() const;
    void setIP(string newIP);
    
    ofColor getColor() const;
    void setColor(ofColor newColor);
    void setColorByIndex(int index);
    
    float getLife() const;
    void setLife(float newLife);
    
    int getDiscIndex() const;
    void setDiscIndex(int index);
    
    
private:
    
    string IP;
    ofColor color;
    
    float life;
    bool connected;
    
    int discIndex;
    
};


#endif /* defined(__varianet_server__Player__) */


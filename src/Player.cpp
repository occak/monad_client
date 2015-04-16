//
//  Player.cpp
//  varianet_server
//
//  Created by cemcakmak on 10/04/15.
//
//

#include "Player.h"

void Player::setup(){
    
    life = 100;
    
    
}

bool Player::isConnected() const{
    
    return connected;
    
}

void Player::setConnection(bool connection){
    
    connected = connection;
    
}

string Player::getIP() const{
    
    return IP;
    
}

void Player::setIP(string newIP) {
    
    IP = newIP;
    
}

ofColor Player::getColor() const{
    
    return color;
    
}

void Player::setColor(ofColor newColor){
    
    color.set(newColor);
}

void Player::setColorByIndex(int index){
    // 161,24,87 - purple
    // 255,153,0 - orange
    // 50,153,187 - blue
    switch (index) {
        case 1:
            color.set(161,24,87);
            break;
        case 2:
            color.set(255,153,0);
            break;
        case 3:
            color.set(50,153,187);
            break;
            
        default:
            break;
    }
    
}

float Player::getLife() const{
    
    return life;
    
}

void Player::setLife(float newLife){
    
    life = newLife;
    
}

int Player::getDiscIndex() const{
    
    return discIndex;
    
}

void Player::setDiscIndex(int index){
    
    discIndex = index;
    
}

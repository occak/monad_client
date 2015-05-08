//
//  Sound.cpp
//  groove_snd
//
//  Created by cemcakmak on 13/03/15.
//
//

#include "Sound.h"

void Sound::setup(Disc* disc){
    
    this->disc = disc;
    
    ///////////////////////
    // Tonic Synth setup
    ///////////////////////
    
    for(int i = 0; i < disc->getDiscIndex(); i++){
        
        
        // synthesis network
        
        Generator groove;
        
        ControlGenerator bpm =  synth.addParameter("bpm"+ofToString(i), 0);
        ControlGenerator metronome = ControlMetro().bpm(bpm);
        
//        float pulseRatio = ofMap(disc->getDensity(i), 1, 30, 0.05, .9);
//        ControlGenerator pulseLength = synth.addParameter("pulseLength"+ofToString(i), pulseRatio);
        ControlGenerator halfPulse = 30 / (bpm+0.001);
        ControlGenerator pulse = ControlPulse().length(halfPulse).input(metronome);
        
        float envelopeCoeff = ofMap(disc->getDensity(i), 1, 30, .1, 10);
        ControlGenerator envelope = synth.addParameter("envelopeWidth"+ofToString(i), envelopeCoeff);
        ControlGenerator attack = synth.addParameter("attack"+ofToString(i),disc->getEnvelope(i, 0));
        ControlGenerator decay = synth.addParameter("decay"+ofToString(i),disc->getEnvelope(i, 1));
        ControlGenerator sustain = synth.addParameter("sustain"+ofToString(i),disc->getEnvelope(i, 2));
        ControlGenerator release = synth.addParameter("release"+ofToString(i),disc->getEnvelope(i, 3));
        Generator amplitude = ADSR().
                                attack(attack*envelope).
                                decay(decay*envelope).
                                sustain(sustain*envelope).
                                release(release*envelope).
                                trigger(pulse);
        
        
        float pitch = ofMap(abs(disc->getNetRotationSpeed(i)), 0, 10, 50, 1500);
        ControlGenerator freq = synth.addParameter("freq"+ofToString(i), pitch);
        ControlGenerator amountFreq = synth.addParameter("amountFreq"+ofToString(i));
        ControlGenerator amountMod = synth.addParameter("amountMod"+ofToString(i),0);
        
        Generator modulation = SineWave().freq(amountFreq) * amountMod;
        Generator snd = SawtoothWave().freq(freq+modulation);
        
        float volCoeff = 1;
        if(disc->getTexture(i) == 1) volCoeff = 1.1;
        else if(disc->getTexture(i) == 2) volCoeff = .25;
        else if(disc->getTexture(i) == 3) volCoeff = .25;
        else if(disc->getTexture(i) == 4) volCoeff = .20;
        
        ControlGenerator volumeBalance = synth.addParameter("volBalance"+ofToString(i), volCoeff);
        groove = snd * amplitude * volumeBalance;
        
        float qTarget = ofMap(disc->getThickness(i), 15, 100, 10, 0);
        ControlGenerator q = synth.addParameter("q"+ofToString(i),qTarget).max(10);
        Generator filter = BPF12().input(groove).cutoff(freq).Q(q);
        
        Generator delay = StereoDelay(0, .0025*i).input(filter).feedback(.01).delayTimeLeft(0).delayTimeRight(.0025*i);
        
        master = master + delay;
    }
    
    ControlGenerator wet = synth.addParameter("wet", 0.).max(.5).min(0.);
    Generator limiter = Limiter().input(master).threshold(0.8);
    Generator reverb = Reverb().input(limiter).stereoWidth(.5).roomSize(.4).wetLevel(wet);
    ControlGenerator masterLevel = synth.addParameter("master", 1.).max(1.).min(0.);
    synth.setOutputGen(masterLevel*reverb);
    
}


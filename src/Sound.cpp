//
//  Sound.cpp
//  groove_snd
//
//  Created by cemcakmak on 13/03/15.
//
//

#include "Sound.h"

void Sound::setup(){
    
    
    ///////////////////////
    // Tonic Synth setup
    ///////////////////////
    
    for(int i = 0; i < 9; i++){
        
        // synthesis network
        
        Generator groove;
        
        ControlGenerator bpm =  synth.addParameter("bpm"+ofToString(i), 0);
        ControlGenerator metronome = ControlMetro().bpm(bpm);
        
        //        float pulseRatio = ofMap(disc->getDensity(i), 1, 30, 0.05, .9);
        //        ControlGenerator pulseLength = synth.addParameter("pulseLength"+ofToString(i), pulseRatio);
        ControlGenerator halfPulse = 30 / (bpm+0.001);
        ControlGenerator pulse = ControlPulse().length(halfPulse).input(metronome);
        
        float envelopeCoeff = ofMap(30, 1, 30, .1, 10);
        ControlGenerator envelope = synth.addParameter("envelopeWidth"+ofToString(i), envelopeCoeff);
        ControlGenerator attack = synth.addParameter("attack"+ofToString(i),0);
        ControlGenerator decay = synth.addParameter("decay"+ofToString(i),0);
        ControlGenerator sustain = synth.addParameter("sustain"+ofToString(i),0);
        ControlGenerator release = synth.addParameter("release"+ofToString(i),0);
        Generator amplitude = ADSR().
        attack(attack*envelope).
        decay(decay*envelope).
        sustain(sustain*envelope).
        release(release*envelope).
        trigger(pulse);
        
        
        float pitch = ofMap(0, 0, 10, 50, 1200);
        ControlGenerator freq = synth.addParameter("freq"+ofToString(i), pitch);
        ControlGenerator amountFreq = synth.addParameter("amountFreq"+ofToString(i));
        ControlGenerator amountMod = synth.addParameter("amountMod"+ofToString(i),0);
        
        Generator modulation = SineWave().freq(amountFreq) * amountMod;
        Generator snd = SawtoothWave().freq(freq+modulation);
        
        
        float volCoeff = 0;
        
        ControlGenerator volumeBalance = synth.addParameter("volBalance"+ofToString(i), volCoeff);
        groove = snd * amplitude * volumeBalance;
    
        float qTarget = ofMap(15., 15., 100., 3.0, 0.2);
        ControlGenerator q = synth.addParameter("q"+ofToString(i),qTarget).max(3.0).min(0.2);
        ControlGenerator qDist = synth.addParameter("qDist"+ofToString(i), 1.);
        ControlGenerator qFinal = q * qDist;
        Generator filter = BPF12().input(groove).cutoff(freq).Q(qFinal);
        
        Generator delay = StereoDelay(0, .0025*i).input(filter).feedback(.01).delayTimeLeft(0).delayTimeRight(.0025*i);
        
        float distAmount = ofMap(0., 0., 100., 1., 10.);
        ControlGenerator gainAmount = synth.addParameter("drive"+ofToString(i),distAmount).max(5.);
        Generator hardClip = Limiter().input(delay).makeupGain(gainAmount).threshold(.5);
        float clipBalance = ofMap(distAmount, 1., 10., 1., 0.5);
        hardClip = hardClip * clipBalance;
        
        master = master + hardClip;
        
    }
    
    ControlGenerator wet = synth.addParameter("wet", 0.).max(.5).min(0.);
    Generator reverb = Reverb().input(master).stereoWidth(.5).roomSize(.4).wetLevel(wet);
    Generator limiter = Limiter().input(reverb).threshold(0.99);
    ControlGenerator masterLevel = synth.addParameter("master", .95).max(.95).min(0.);
    synth.setOutputGen(masterLevel*limiter);
    
}

void Sound::newSynth(int index){
    
    // synthesis network
    
    Generator groove;
    
    ControlGenerator bpm =  synth.addParameter("bpm"+ofToString(index), 0);
    ControlGenerator metronome = ControlMetro().bpm(bpm);
    
    //        float pulseRatio = ofMap(disc->getDensity(i), 1, 30, 0.05, .9);
    //        ControlGenerator pulseLength = synth.addParameter("pulseLength"+ofToString(i), pulseRatio);
    ControlGenerator halfPulse = 30 / (bpm+0.001);
    ControlGenerator pulse = ControlPulse().length(halfPulse).input(metronome);
    
    float envelopeCoeff = ofMap(disc->getDensity(index), 1, 30, .1, 10);
    ControlGenerator envelope = synth.addParameter("envelopeWidth"+ofToString(index), envelopeCoeff);
    ControlGenerator attack = synth.addParameter("attack"+ofToString(index),disc->getEnvelope(index, 0));
    ControlGenerator decay = synth.addParameter("decay"+ofToString(index),disc->getEnvelope(index, 1));
    ControlGenerator sustain = synth.addParameter("sustain"+ofToString(index),disc->getEnvelope(index, 2));
    ControlGenerator release = synth.addParameter("release"+ofToString(index),disc->getEnvelope(index, 3));
    Generator amplitude = ADSR().
    attack(attack*envelope).
    decay(decay*envelope).
    sustain(sustain*envelope).
    release(release*envelope).
    trigger(pulse);
    
    
    float pitch = ofMap(abs(disc->getNetRotationSpeed(index)), 0, 10, 50, 1500);
    ControlGenerator freq = synth.addParameter("freq"+ofToString(index), pitch);
    ControlGenerator amountFreq = synth.addParameter("amountFreq"+ofToString(index));
    ControlGenerator amountMod = synth.addParameter("amountMod"+ofToString(index),0);
    
    Generator modulation = SineWave().freq(amountFreq) * amountMod;
    Generator snd = SawtoothWave().freq(freq+modulation);
    
    
    float volCoeff = 1;
    if(disc->getTexture(index) == 1) volCoeff = 1.1;
    else if(disc->getTexture(index) == 2) volCoeff = .7;
    else if(disc->getTexture(index) == 3) volCoeff = .5;
    else if(disc->getTexture(index) == 4) volCoeff = .1;
    
    ControlGenerator volumeBalance = synth.addParameter("volBalance"+ofToString(index), volCoeff);
    groove = snd * amplitude * volumeBalance;
    
    float qTarget = ofMap(disc->getThickness(index), 15, 100, 10, 0);
    ControlGenerator q = synth.addParameter("q"+ofToString(index),qTarget).max(10);
    Generator filter = BPF12().input(groove).cutoff(freq).Q(q);
    
    Generator delay = StereoDelay(0, .0025*index).input(filter).feedback(.01).delayTimeLeft(0).delayTimeRight(.0025*index);
    
    float distAmount = ofMap(disc->getSpikeDistance(index), 0., 100., 1., 40.);
    ControlGenerator gainAmount = synth.addParameter("drive"+ofToString(index),distAmount).max(40.);
    Generator hardClip = Limiter().input(delay).makeupGain(gainAmount).threshold(.5);
    
    master = master + hardClip;
    
}
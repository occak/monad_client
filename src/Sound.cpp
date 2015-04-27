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
        
        float pulseRatio = ofMap(disc->getDensity(i), 1, 30, 0.005, 1);
        ControlGenerator pulseLength = synth.addParameter("pulseLength"+ofToString(i), pulseRatio);
        ControlGenerator pulse = ControlPulse().length(pulseLength).input(metronome);
        
        float envelopeCoeff = ofMap(disc->getDensity(i), 1, 30, .1, 10);
        ControlGenerator envelope = synth.addParameter("envelopeWidth"+ofToString(i), envelopeCoeff);
        ControlGenerator attack = synth.addParameter("attack"+ofToString(i),disc->getEnvelope(i, 0));
        ControlGenerator decay = synth.addParameter("decay"+ofToString(i),disc->getEnvelope(i, 1));
        ControlGenerator sustain = synth.addParameter("sustain"+ofToString(i),disc->getEnvelope(i, 2));
        ControlGenerator release = synth.addParameter("release"+ofToString(i),disc->getEnvelope(i, 3));
        Generator amplitude = ADSR().
                                attack(attack*envelope).
                                decay(decay*pulseLength*envelope).
                                sustain(sustain*envelope).
                                release(release*envelope).
                                trigger(pulse);
        
        
        float pitch = ofMap(abs(disc->getNetRotationSpeed(i)), 0, 10, 50, 1500);
        ControlGenerator freq = synth.addParameter("freq"+ofToString(i), pitch);
        ControlGenerator amountFreq = synth.addParameter("amountFreq"+ofToString(i));
        ControlGenerator amountMod = synth.addParameter("amountMod"+ofToString(i),0);
//        
//        float cutoffTarget = ofMap(disc->getThickness(i), 15, 100, 0, 50);
//        ControlGenerator cutoff = synth.addParameter("frequency"+ofToString(i), cutoffTarget);
//        ControlSnapToScale scaleSnapper = ControlSnapToScale().setScale(scale).input(32 + cutoff);
//        ControlGenerator filterFreq = ControlMidiToFreq().input(scaleSnapper);
        
        Generator modulation = SineWave().freq(amountFreq) * amountMod;
        Generator snd = SawtoothWave().freq(freq+modulation);
        groove = snd * amplitude;
        
        float qTarget = ofMap(disc->getThickness(i), 15, 100, 10, 0);
        ControlGenerator q = synth.addParameter("q"+ofToString(i),qTarget).max(10);
        Generator filter = BPF12().input(groove).cutoff(freq).Q(q);
        
        Generator delay = StereoDelay(0, .0025*i).input(filter).feedback(.01).delayTimeLeft(0).delayTimeRight(.0025*i);
        
        master = master + delay;
    }
    
    ControlGenerator size = synth.addParameter("size", 0.01).max(0.5).min(0);
    ControlGenerator decay = synth.addParameter("decay", 0.01);
    Generator limiter = Limiter().input(master).threshold(0.8);
    Generator reverb = Reverb().input(limiter).stereoWidth(.5).roomSize(.5).wetLevel(size).dryLevel(.5);
    synth.setOutputGen(reverb);
    
}

//void Sound::setScale(int index, float value){
//    
//    if(scale.size()>0) scale[index] = value;
//    else scale.push_back(value);
//    
//}

#pragma once

#include "ofMain.h"
#include "ofxMaxim.h"
#include "ofxGui.h"


class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    
    
    void audioOut (float * output, int bufferSize, int nChannels);
    void audioIn(float * input, int bufferSize, int nChannels);
    int bufferSize; // This is the audio buffersize
    int sampleRate; // This is the audio sample rate
    
    
    maxiOsc myOsc[8],myOsc2[8],myOsc3[8],myOsc4, myAutoPanner,phasor[8], count, timer;
    double myStereoOutput[2];
    double waves[8];
    maxiMix myOutputs;
    //to change switch case
    int synthType;
    
    //gui controls
    ofxFloatSlider carrier,modIndex,freq,phasorFreq,startPhase,endPhase;
    ofxFloatSlider modFreq1, modFreq2,modFreq3, index, tempo, guiCuttoff, guiResonance;
    ofxFloatSlider mod2Freq1, mod2Freq2,mod2Freq3,A,D,S,R, envelopeShape, sidebands, fundamental;
    ofxPanel gui1, gui2, gui3, gui4;
    
    //some frequencies to create an arpeggio
    int myNotes[8]={130,164,196,246,261,293,329,392};
    int counter, counter2,counter3, counter4,speed=120;
    float harmonics;
    double envelope;
    maxiClock myClock;
    
    //to store the main outputs
    vector <float> lAudio;
    vector <float> rAudio;
    
    
    double ifftVal;
    ofxMaxiFFT mfft;
    
    
    //fft size
    int fftSize;
    
    maxiMFCC mfcc;
    double *mfccs;
    
    //to create a ADSR envelope
    maxiEnv myEnvelope;
    
    
    //lores filter
    maxiFilter myFilter;
    double filteredOut, cuttoff, resonance;
    
};


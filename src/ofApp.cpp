#include "ofApp.h"
//This work includes a mixture of material learnt in Audio Visual processing.
//Material used is from Goldsmiths University lectures, Maximilian examples and the Creative Programming for Audiovisual Art Kadenze Course.

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(60);
    sampleRate = 44100;
    bufferSize = 512;
    ofxMaxiSettings::setup(sampleRate, 2, bufferSize);
    lAudio.assign(bufferSize, 0.0);
    rAudio.assign(bufferSize, 0.0);
    
    //fft
    fftSize = 4096;
    //windowing setup(int _fftSize, int _windowSize, int _hopSize)
    mfft.setup(fftSize, 512, 256);
    
    //clock settings
    myClock.setTicksPerBeat(4);//This sets the number of ticks per beat
    myClock.setTempo(speed);// This sets the tempo in Beats Per Minute
    
    
    //gui1 for polyphonic synth
    gui1.setup();
    ofxGuiSetFillColor(0);
    gui1.add(index.setup("mod Index", 5, 0, 1000));
    gui1.add(modFreq1.setup("freq1", 1, 0,300));
    gui1.add(modFreq2.setup("freq2",0.001,0.001,5));
    gui1.add(tempo.setup("Tempo", 120,20,200));
    
    //gui2 for FM 2
    
    gui2.setup();
    gui2.add(carrier.setup("Carrier Frequency", 400, 10, 1500));
    gui2.add(endPhase.setup("Phasor end",489, 0,1100));
    gui2.add(phasorFreq.setup("Phasor Frequency", 0.01, 0,1000));
    gui2.add(modIndex.setup("mod Index", 5, 0, 1500));
    gui2.add(guiCuttoff.setup("Lores Cuttoff", 250, 0,2000));
    gui2.add(guiResonance.setup("Lores Resonance", 70, 0,100));
    
    
    //gui3 for Sub synth with envelope
    
    gui3.setup();
    gui3.add(mod2Freq1.setup("Main Frequency", 100, 20, 2000));
    gui3.add(mod2Freq2.setup("Freq 1", 50, 0,5000));
    gui3.add(mod2Freq3.setup("Freq 2 ",50, 0,200));
    gui3.add(A.setup("Attack", 200, 1,2000));
    gui3.add(D.setup("Decay", 200, 1,2000));
    gui3.add(S.setup("Sustain", 200, 1,2000));
    gui3.add(R.setup("Release", 200, 1,2000));
    gui3.add(envelopeShape.setup("Envelope shape", 5, 1,10));
    
    //gui4 FM with harmonic/enharmonic series
    
    gui4.setup();
    gui4.add(fundamental.setup("Fundamental Frequency", 200, 100, 1500));
    gui4.add(sidebands.setup("Sidebands", 10, 1, 5000));
    
    
    
    ofSoundStreamSetup(2,2,this, sampleRate, bufferSize, 4);
}

//--------------------------------------------------------------
void ofApp::update(){
    //here I can update the tempo in real time
    myClock.setTempo(tempo);
    
    //ADSR
    //enevelope parameters are at millis and triggered by gui sliders
    myEnvelope.setAttack(A);
    myEnvelope.setDecay(D);
    myEnvelope.setSustain(S);
    myEnvelope.setRelease(R);
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    //here we select which gui to use
    switch(synthType) {
        case 1:
            
            gui1.draw();
            
            break;
        case 2:
            gui2.draw();
            
            break;
        case 3:
            gui3.draw();
            break;
            
        case 4:
            gui4.draw();
            break;
        default:
            ofBackground(0);
            
            break;
    }
    
    ofSetColor(225);
    ofDrawBitmapString("Select 1 for polysynth with a sequence",230,30);
    ofDrawBitmapString("Select 2 for FM Synth with Lores Filter",230,40);
    ofDrawBitmapString("Select 3 for Additive Synth with ADSR",230,50);
    ofDrawBitmapString("Select 4 for FM demonstrating Harmonic and Enharmonic series",230,60);
    
    
    // draws the sound wave:
    
    ofSetColor(120);
    ofSetLineWidth(3);
    
    //here we draw a vertex to produce a representation of the audio output as a waveform. Idea taken from the Audio outputs OF examples
    ofBeginShape();
    for (unsigned int i = 0; i < lAudio.size(); i++){
        float x =  ofMap(i, 0, lAudio.size(), 0,ofGetWidth(), true);
        ofVertex(x, 450 -lAudio[i]*100);
    }
    ofEndShape(false);
    
    float horizWidth = ofGetWidth();
    float horizOffset = 100;
    float fftTop = 250;
    float mfccTop = 350;
    float chromagramTop = 450;
    
    ofSetColor(255, 0, 0,100);
    
    //draws fft output using the idea from maximilian example (Feature extractor)
    float xinc = horizWidth / fftSize * 20;
    for(int i=0; i < fftSize / 2; i++) {
        //magnitudesDB took out
        float height = mfft.magnitudes[i]*2;
        ofDrawRectangle(horizOffset + (i*xinc),ofGetHeight() - height,30, height);
    }
    
    
    
}

//--------------------------------------------------------------
void ofApp::audioOut(float * output, int bufferSize, int nChannels){
    
    for(int i = 0; i < bufferSize ; i++){
        myClock.ticker();
        
        if (myClock.tick) {
            //counter will trigger each one of the seven oscillators
            counter++;
            counter3++;
            if(counter>7){
                counter=0;
            }
            //counter for harmonic/enharmonic series
            if(counter3%20==0){
                counter4+=1;
                if(counter4>3){
                    counter4 = 0;
                }
            }
            
        }
        switch(synthType) {
            case 1:
                //fm synth with a sequence of notes and tempo control
                for (int i=0; i<8; i++) {
                    //optional wave
                    //waves[i] = myOsc[i].sinewave(myNotes[i]+(myOsc2[i].sawn(modFreq1)*phasor[i].phasor(modFreq2, 0, endPhase))*index);
                    
                    waves[i]=myOsc[i].square(myNotes[i]+myOsc2[i].sawn(modFreq1+myOsc3[i].saw(modFreq2))*index);
                    
                }
                myOutputs.stereo(waves[counter],myStereoOutput,(myAutoPanner.sinewave(1)+1)/2);
                break;
                
            case 2:
                //fm synth with filters
                waves[0] = myOsc[0].sinewave(carrier+(myOsc2[0].sawn(modIndex)*phasor[0].phasor(phasorFreq, 0, endPhase)));
                
                filteredOut = myFilter.lores(waves[0], guiCuttoff, guiResonance);
                myOutputs.stereo(filteredOut,myStereoOutput,(myAutoPanner.sinewave(1)+1)/2);
                
                break;
                
            case 3:
                //additive synth with envelope
                //I gave a saw shape to the envelope input.
                envelope=myEnvelope.adsr(myOsc[1].saw(envelopeShape),myEnvelope.trigger);
                
                counter2=timer.phasor(1, 1, 9);
                
                if (counter2==1) myEnvelope.trigger=1; //triggers envelope when phaser comes back to one
                
                else myEnvelope.trigger=0;//release the envelope
                
                waves[0]=(myOsc[0].saw(mod2Freq1)* myOsc2[0].sinewave(mod2Freq2)*myOsc3[0].sinewave(mod2Freq3))*envelope;
                
                myOutputs.stereo(waves[0],myStereoOutput,(myAutoPanner.sinewave(1)+1)/2);
                
                break;
            case 4:
                
                //in order for the modulation to be harmonic it needs to be mathematicly related similar as how the sawtooth wave is conformed e.g. fundamental+1,fundamental+2,fundamental + 3, etc.
                if(counter4==2||counter4==3){
                    //creates an harmonic series
                    harmonics = fundamental*2;
                }else{
                    //creates an unpitched sounds due to unrelated harmonics
                    harmonics = fundamental*2.2;
                }
                waves[2]= myOsc[2].sinewave(fundamental+(myOsc[3].sinewave(harmonics)*sidebands));
                myOutputs.stereo(waves[2],myStereoOutput,(myAutoPanner.sinewave(1)+1)/2);
                break;
            default:
                
                break;
        }
        
        if (mfft.process(myStereoOutput[0])) {
            
        }
        
        //output[i*nChannels]=myStereoOutput[0];
        //output[i*nChannels + 1]=myStereoOutput[1];
        lAudio[i] = output[i*nChannels    ] = myStereoOutput[0];
        rAudio[i] = output[i*nChannels + 1] = myStereoOutput[1];
    }
}
//--------------------------------------------------------------
void ofApp::audioIn(float * input, int bufferSize, int nChannels){
    
    
}
//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    //synth selection
    if (key == '1'){
        synthType = 1;
    } else if (key == '2'){
        synthType = 2;
    }
    else if (key == '2'){
        synthType = 2;
    }
    else if (key == '3'){
        synthType = 3;
    }
    else if (key == '4'){
        synthType = 4;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}


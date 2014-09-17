#include "ofApp.h"

ofImage fingerImage;
int soundIndex = 0;
bool bGen = false;
string currentState;
int freq = 100;
float rate = .9;
int soundAdd = 1;
int last = 0;
int offset = 0;

//--------------------------------------------------------------
void ofApp::setup(){
    ofSerial dum;
    scanner.setup(dum.getDeviceList()[0].getDevicePath());
    scanner.getDevice().ChangeBaudRate(115200);
    scanner.setLED(true);
    ofAddListener(scanner.gotFingerprint, this, &ofApp::onFingerprint);
    
    ofBackground(0);
    
    ofSetFrameRate(60);
    
    int bufferSize		= 512;
    int sampleRate 			= 44100;
    
    //soundStream.listDevices();
    
    //if you want to set the device id to be different than the default
    //soundStream.setDeviceID(1); 	//note some devices are input only and some are output only
    
    soundStream.setup(this, 2, 0, sampleRate, bufferSize, 4);
    currentState = "Place your finger on the device";
}

//--------------------------------------------------------------
void ofApp::update(){
    
    if ( scanner.canCaptureFingerprint() && scanner.isFingerDown() ){
        scanner.getFingerprint( false ); // pass 'false' for not raw, slightly faster
        currentState = "";
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    if (fingerImage.isAllocated()){
        if (!fingerImage.isUsingTexture()){
            fingerImage.setUseTexture(true);
            fingerImage.update();
        }
        ofPushMatrix();
        ofScale(3.0, 3.0);
        fingerImage.draw(0, 0);
        ofNoFill();
        int y = soundIndex/fingerImage.width;
        int x = soundIndex % fingerImage.width;
        ofRect(x, y, 1, fingerImage.height);
        ofFill();
        ofSetColor(255, 0, 0);
        ofRect(x, ofWrap(y + offset, 0, fingerImage.height), 1, 1);
        ofSetColor(255);
        ofPopMatrix();
    }
    if ( currentState != "") ofDrawBitmapString(currentState, ofGetWidth()/2.0, ofGetHeight()/2.0);
}

#pragma mark events

//--------------------------------------------------------------
void ofApp::onFingerprint( ofImage & img ){
    fingerImage.setUseTexture(false);
    fingerImage.setFromPixels(img.getPixelsRef());
    bGen = true;
}

float sinIndex = 0.0;

//--------------------------------------------------------------
void ofApp::audioOut(float * output, int bufferSize, int nChannels){
    if ( bGen ){
        int s = fingerImage.getPixelsRef().size();
        float index = soundIndex;
        for (int i = 0; i < bufferSize; i++){
            int ind = (int) floor(index) % s;
            int y = ofWrap(ofMap(i + offset, 0.0, bufferSize, 0, fingerImage.getHeight()) + offset,
                           0, fingerImage.height);//ind/fingerImage.width;
            int x = soundIndex; //ind % fingerImage.width;
            int c = fingerImage.getPixelsRef().getColor(x, y).r;
            sinIndex = ofMap(c, 0.0, 255, 0.0, rate);
            output[i*nChannels    ] = output[i*nChannels    ] * rate + ofMap(c,0,100,0.0,1.0) * (1.0-rate);
            output[i*nChannels + 1] = output[i*nChannels + 1] * rate + ofMap(c,0,100,0.0,1.0) * (1.0-rate);
//            index += rate;
        }
        offset ++;
        offset %= fingerImage.height;
        int t = ofGetElapsedTimeMillis();
        if ( t - last > freq ){
            last = t;
            soundIndex += soundAdd;
//            offset = 0.0;
        }
        sinIndex = 0.0;
        if ( soundIndex >= fingerImage.width ){
            soundIndex = 0;
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if ( key == '+') soundAdd++;
    if ( key == '-') soundAdd--;
    soundAdd = ofClamp(soundAdd, 0.0, 10);
}
//--------------------------------------------------------------
void ofApp::keyReleased(int key){}
//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    if ( bGen ){
//        soundIndex = x + y * fingerImage.width;
//        sinIndex = 0.0;
    }
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    freq = ofMap(x, 0, ofGetWidth(), 0, 2000);
    rate = ofMap(y, 0, ofGetHeight(), .0, 1.0);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ }

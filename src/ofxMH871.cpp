//
//  MH871.cpp
//  xmlSettingsExample
//
//  Created by dantheman on 2/9/16.
//
//

#include "ofxMH871.h"
void MH871::setup(string serialPort){
    device.setup(serialPort, 9600);
    initPlotter();
    penUp();
    timer.setFramerate(15);
}
void MH871::update(){
    if(timer.tick()){
        if(ptsCache.size() > 0){
            currentPoint = ptsCache.front();
            ptsCache.pop_front();
        }
        
        if(cache.size() > 0 && device.isInitialized()){
            sendCommand(cache.front());
            cache.pop_front();
        }else{
            if(cache.size() > 0 && !device.isInitialized()){
                cache.clear();
                ofLog(OF_LOG_ERROR)<<"cache cleared no serial device"<<endl;
            }
        }
    }
}

void MH871::draw(){
    ofPushMatrix();
    ofPushStyle();
    ofNoFill();
    ofSetColor(255, 255, 0);
    ofDrawRectangle(0, 0, drawingSize.x, drawingSize.y);
    ofSetColor(255, 0, 255);
    ofDrawCircle(currentPoint, 10);
    ofPopStyle();
    ofPopMatrix();
}

void MH871::setPrintSize(float width, float height){
    printSize.x = width;
    printSize.y = height;
}
void MH871::setPrintSize(ofVec2f size){
    setPrintSize(size.x, size.y);
}

ofVec2f MH871::getPrintSize(){
    return printSize;
}

void MH871::setDrawingSize(float width, float height){
    drawingSize.x = width;
    drawingSize.y = height;
}

void MH871::setDrawingSize(ofVec2f size){
    setDrawingSize(size.x, size.y);
}

ofVec2f MH871::getDrawingSize(){
    return drawingSize;
}


void MH871::addPoint(ofPoint pt){
    ofVec2f fooP = mapPoint(pt);
    int x = (int)fooP.x;
    int y = (int)fooP.y;
    ptsCache.push_back(pt);
    cache.push_back("PA"+ofToString(x)+","+ofToString(y)+";");
}
void MH871::addPoint(ofVec2f pt){
    ofVec2f fooP = mapPoint(pt);
    int x = (int)fooP.x;
    int y = (int)fooP.y;
    ptsCache.push_back(pt);
    cache.push_back("PA"+ofToString(x)+","+ofToString(y)+";");
}
void MH871::plotPolyline(ofPolyline line){
    // alteration by db 23/1/2020
    vector<ofPoint> verts;
    ofVec3f new3DPoint;
    ofVec2f new2DPoint;
    for (int i = 0; i<line.size(); i++){
        new3DPoint = line[i];
        new2DPoint.set(new3DPoint.x, new3DPoint.y);
        verts.push_back( new2DPoint);
    }
   
    for(float i = 0; i < verts.size(); i++){
        if(i == 0){
            startPlot(verts[i]);
        }else{
            addPoint(verts[i]);
        }
    }
    cache.push_back("PU;");
}
void MH871::plotPolylines(vector<ofPolyline> lines){
    for(int j = 0; j < lines.size(); j++){
        plotPolyline(lines[j]);
    }
}
void MH871::endPlot(){
    cache.push_back("PU;");
    cache.push_back("PA0,0;");
    ptsCache.push_back(ofVec2f(0, 0));
    
}
void MH871::startPlot(ofPoint pt){
    ofVec2f fooP = mapPoint(pt);
    int x = (int)fooP.x;
    int y = (int)fooP.y;
    ptsCache.push_back(pt);
    cache.push_back("PA"+ofToString(x)+","+ofToString(y)+";");
    cache.push_back("PD;");
}
void MH871::startPlot(ofVec2f pt){
    ofVec2f fooP = mapPoint(pt);
    int x = (int)fooP.x;
    int y = (int)fooP.y;
    ptsCache.push_back(pt);
    cache.push_back("PA"+ofToString(x)+","+ofToString(y)+";");
    cache.push_back("PD;");
}

void MH871::initPlotter(){
    sendCommand("IN;");
    endPlot();
}

void MH871::penDown(){
    sendCommand("PD;");
}

void MH871::penUp(){
    sendCommand("PU;");
}

ofVec2f MH871::mapPoint(ofPoint pt){
    return ofVec2f(ofMap(pt.x, 0, drawingSize.x, printSize.x, 0, true), ofMap(pt.y, 0, drawingSize.y, 0, printSize.y, true));
}

ofVec2f MH871::mapPoint(ofVec2f pt){
    return ofVec2f(ofMap(pt.x, 0, drawingSize.x, printSize.x, 0, true), ofMap(pt.y, 0, drawingSize.y, printSize.y, 0, true));
}

void MH871::sendCommand(string command){
    ofBuffer buf;
    buf.set(command);
    device.writeBytes(buf);
}

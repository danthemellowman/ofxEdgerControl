//
//  DownloaderTask.h
//  edgertronicControl
//
//  Created by Dan Moore on 8/18/14.
//
//

#pragma once
#include "ofMain.h"

class DownloaderTask{
public:
    DownloaderTask();
    ~DownloaderTask();
    void setup();
    void triggerDownload();
    void update(ofEventArgs& args);
    void urlResponse(ofHttpResponse & response);
    void exit();
    
	ofEvent<string> downloadFinished;
private:
    string downloadURL;
    string lastFileName;
    deque<string> downloadFiles;
    float progress;
};
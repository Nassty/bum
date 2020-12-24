#pragma once

#include "Parsing.h"
#include "ofMain.h"
#include "ofxGLEditor.h"

class ofApp : public ofBaseApp, public ofxGLEditorListener {
 public:
  void setup();
  void update();
  void draw();

  void keyPressed(int key);
  void windowResized(int w, int h);

  /// ofxGLEditor events
  void saveFileEvent(int &whichEditor);
  void openFileEvent(int &whichEditor);
  void executeScriptEvent(int &whichEditor);
  void evalReplEvent(const string &text);

  ofxGLEditor editor;
  string code;

  ofPixels pixs;
  int it = 0;

  BlockStatement c;
  TokenMap ctx;
};

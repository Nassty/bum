#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
  ofBackground(0);
  ctx["r"] = 0;
  ctx["g"] = 0;
  ctx["b"] = 0;
  ctx["x"] = 0;
  ctx["y"] = 0;
  ctx["t"] = 0;

  pixs.allocate(20, 20, OF_IMAGE_COLOR);

  // set a custom Repl banner & prompt (do this before setup())
  ofxEditor::loadFont("arcade.ttf", 24);

  // setup editor with event listener
  editor.setup(this);

  // make some room for the bottom editor info text
  editor.resize(ofGetWidth(), ofGetHeight() - ofxEditor::getCharHeight());

  // open & load a file into the current editor (1)
  editor.openFile("hello.txt", 1);

  editor.getSettings().setCursorColor(ofColor::red);  // current pos cursor
  editor.getSettings().setAlpha(0.5);  // main text, cursor, & highlight alpha
  editor.getSettings().setTextColor(ofColor::red);

  // other settings
  editor.setLineWrapping(true);
  editor.setLineNumbers(true);
  editor.setAutoFocus(false);
  code = editor.getText(1);
  c.compile(code.c_str());
}

//--------------------------------------------------------------
void ofApp::update() {
  it++;
  int x = 0;
  ctx["t"] = ofGetElapsedTimef();
  for (auto line : pixs.getLines()) {
    int y = 0;
    for (auto pix : line.getPixels()) {
      ctx["x"] = x;
      ctx["y"] = y;
      try {
        c.exec(ctx);
      } catch (const std::exception &) {
      }
      pix[0] = abs(int(255.f * ctx["r"].asDouble()));
      pix[1] = abs(int(255.f * ctx["g"].asDouble()));
      pix[2] = abs(int(255.f * ctx["b"].asDouble()));
      y++;
    }
    x++;
  }
}

//--------------------------------------------------------------
void ofApp::draw() {
  ofImage img;
  img.setFromPixels(pixs);
  img.draw(0, 0, ofGetWidth(), ofGetHeight());

  editor.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
  bool modifier = ofGetKeyPressed(
      ofxEditor::getSuperAsModifier() ? OF_KEY_SUPER : OF_KEY_CONTROL);
  if (modifier) {
    switch (key) {
      case 'f':
      case 6:
        ofToggleFullscreen();
        return;
      case 'l':
      case 12:
        editor.setLineWrapping(!editor.getLineWrapping());
        return;
      case 'n':
      case 14:
        editor.setLineNumbers(!editor.getLineNumbers());
        return;
      case 'k':
      case 26:
        editor.setAutoFocus(!editor.getAutoFocus());
        return;
    }
  }

  // see ofxGLEditor.h for key commands
  editor.keyPressed(key);
}

void ofApp::windowResized(int w, int h) {
  // make some room for the bottom editor info text
  editor.resize(w, h - ofxEditor::getCharHeight());
}

//--------------------------------------------------------------
void ofApp::openFileEvent(int &whichEditor) {
  // received an editor open via CTRL/Super + o

  ofLogNotice() << "received open event for editor " << whichEditor
                << " with filename " << editor.getEditorFilename(whichEditor);
}

//--------------------------------------------------------------
void ofApp::saveFileEvent(int &whichEditor) {
  // received an editor save via CTRL/Super + s or CTRL/Super + d

  ofLogNotice() << "received save event for editor " << whichEditor
                << " with filename " << editor.getEditorFilename(whichEditor);
}

//--------------------------------------------------------------
void ofApp::executeScriptEvent(int &whichEditor) {
  // received on editor CTRL/Super + e

  // get the text buffer with:
  string txt = editor.getText(whichEditor);
  std::for_each(txt.begin(), txt.end(), [](char &c) { c = ::tolower(c); });
  code = txt;
  c.compile(txt.c_str());
  ofLogNotice() << "received execute script event for editor " << whichEditor;
  ofLogNotice() << txt.c_str();
}

//--------------------------------------------------------------
void ofApp::evalReplEvent(const string &text) {
	editor.evalReplReturn(to_string(ctx["r"].asDouble())); // print this, then prompt
	editor.evalReplReturn(to_string(ctx["g"].asDouble())); // print this, then prompt
	editor.evalReplReturn(to_string(ctx["b"].asDouble())); // print this, then prompt
}

#include "cinder/app/AppBasic.h"
#include "cinder/Cinder.h"
#include "cinder/Utilities.h"
#include <list>

#include "V8Script.h"

#include "ExampleObjectWrap.h"
#include "TestObject.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class BasicApp : public AppBasic {
  public:
	void setup();
	void shutdown();
	void update();

	void mouseDrag( MouseEvent event );
	void keyDown( KeyEvent event );
	void draw();

	V8ScriptRef mScript;
	ScriptFunction mUpdateScriptFunc;

	map<string, string> mOptions;
    map<string, string> mOutput;
};

void BasicApp::setup()
{
	string fileName = "script.js";
	fs::path filePath = getAssetPath("") / fs::path(fileName);

	console() << "Script file: " << filePath << endl;

	mOptions["test_option"] = "option value";

	WrappersList expose;
	expose.push_back( &ExampleObjectWrap::Init );
	expose.push_back( &TestObject::Init );

	mScript = V8Script::create();
	mScript->setup(filePath, &mOptions, &mOutput, &expose);
	mScript->getFunction(&mUpdateScriptFunc, "update");


}

void BasicApp::shutdown()
{
	mUpdateScriptFunc.Reset();
}

void BasicApp::update()
{
	mOptions["elapsedSeconds"] = toString( (int)getElapsedSeconds() );

	mScript->call(&mUpdateScriptFunc); // call update function from script
}



void BasicApp::mouseDrag( MouseEvent event )
{
	
}

void BasicApp::keyDown( KeyEvent event )
{
	
}

void BasicApp::draw()
{
	gl::clear( Color( 0.1f, 0.1f, 0.15f ) );

	
}



// This line tells Cinder to actually create the application
CINDER_APP_BASIC( BasicApp, RendererGl(0) )
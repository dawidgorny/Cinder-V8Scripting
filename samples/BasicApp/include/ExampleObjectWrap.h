#pragma once

#include "ObjectWrap.h"

#include "ExampleObject.h"

class ExampleObjectWrap : public ObjectWrap {
public:
	static void Init(v8::Handle<v8::Object> exports);

private:
	//explicit ExampleObjectWrap();
	//~ExampleObjectWrap();

	static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
	static v8::Persistent<v8::Function> constructor;
	
	/*
	 * ----
	 */
	
	static void setValue(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void getValue(const v8::FunctionCallbackInfo<v8::Value>& args);
};

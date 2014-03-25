#pragma once

#include "v8.h"
#include "v8utils.h"

#include <memory>
#include <boost/filesystem/path.hpp>

using namespace std;

typedef void (*WrapFunc)(v8::Handle<v8::Object>);
typedef std::vector< WrapFunc > WrappersList;
typedef v8::Persistent<v8::Function> ScriptFunction;

class V8Script;
typedef std::shared_ptr<V8Script> V8ScriptRef;

class V8Script {
public:
	static V8ScriptRef create() { return std::make_shared<V8Script>(); }
	V8Script() {
		mV8Initialized = false;
		v8::V8::InitializeICU();
	}
	~V8Script() {
		context_.Reset();
		
		isolate_ = NULL;
		v8::V8::Dispose();
	}


	void setup(boost::filesystem::path scriptFile, 
						map<string, string>* opts,
                        map<string, string>* output,
						WrappersList* exposeList = NULL);
	void getFunction(ScriptFunction* functionObj, std::string functionName);
	void call(ScriptFunction* functionObj);

	void bind(float* var, std::string name);

	v8::Handle<v8::Object> getGlobal();



	v8::Handle<v8::Object> WrapMap(std::map<std::string, std::string>* obj);
	static std::map<std::string, std::string>* UnwrapMap(v8::Handle<v8::Object> obj);

	static void MapGet(v8::Local<v8::String> name,
                                    const v8::PropertyCallbackInfo<v8::Value>& info);
	static void MapSet(v8::Local<v8::String> name,
                                    v8::Local<v8::Value> value_obj,
                                    const v8::PropertyCallbackInfo<v8::Value>& info);
	v8::Handle<v8::ObjectTemplate> MakeMapTemplate(v8::Isolate* isolate);

	bool InstallMaps(map<string, string>* opts,
                                         map<string, string>* output);

	static v8::Persistent<v8::ObjectTemplate> map_template_;

private:
	

	v8::Isolate*					isolate_;
	v8::Persistent<v8::Context>		context_;

	bool mV8Initialized;

	

};

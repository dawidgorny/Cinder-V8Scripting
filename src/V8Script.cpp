#include "V8Script.h"

void V8Script::setup(boost::filesystem::path scriptFile, 
						map<string, string>* opts,
                        map<string, string>* output,
						WrappersList* exposeList)
{
	isolate_ = v8::Isolate::GetCurrent();
	v8::HandleScope handle_scope(isolate_);

	std::string fileName = scriptFile.filename().string();

	v8::Handle<v8::String> script_source;
	v8::Handle<v8::Value> script_name;

	// Use argument as a name of file to load.
	

	script_source = v8::utils::ReadFile(isolate_, scriptFile.string().c_str());
	script_name = v8::String::NewFromUtf8(isolate_, fileName.c_str());


	// Create a template for the global object.
	v8::Handle<v8::ObjectTemplate> global = v8::ObjectTemplate::New();

	// Bind the global 'print' function to the C++ Print callback.
	global->Set(v8::String::NewFromUtf8(isolate_, "print"),
		v8::FunctionTemplate::New(v8::utils::Print));


	// Create a new execution environment containing the built-in
	// functions
	v8::Handle<v8::Context> context = v8::Context::New(isolate_, NULL, global);
	context_.Reset(isolate_, context);

	// Enter the newly created execution environment.
	v8::Context::Scope context_scope(context);

	// Make the options mapping available within the context
	if (!InstallMaps(opts, output))
		return;

	// Exposing from the list
	if(exposeList != NULL) 
	{
		for(WrappersList::const_iterator it = exposeList->cbegin(); it != exposeList->cend(); ++it) 
		{
			WrapFunc func = *it;
			func( context->Global() );
		}
	}


	bool report_exceptions = true;

	v8::Handle<v8::Script> script;
	{
		// Compile script in try/catch context.
		v8::TryCatch try_catch;
		script = v8::Script::Compile(script_source, script_name);
		if (script.IsEmpty()) {
			// Print errors that happened during compilation.
			if (report_exceptions)
				v8::utils::ReportException(isolate_, &try_catch);
			return;
		}
	}

	{
		v8::TryCatch try_catch;

		script->Run();
		if (try_catch.HasCaught()) {
			if (report_exceptions)
				v8::utils::ReportException(isolate_, &try_catch);
			return;
		}
	}


	mV8Initialized = true;
}

void V8Script::getFunction(ScriptFunction* functionObj, std::string functionName)
{
	if(!mV8Initialized)
		return;

	v8::HandleScope handle_scope( isolate_ );
	v8::Local<v8::Context> context = v8::Local<v8::Context>::New(isolate_, context_);
	v8::Context::Scope context_scope(context);

	v8::Handle<v8::String> fun_name =
		v8::String::NewFromUtf8(isolate_, functionName.c_str());
	v8::Handle<v8::Value> func_val = context->Global()->Get(fun_name);
	
	if (!func_val->IsFunction()) {
		ci::app::console() << "Error: Script does not declare '"<<functionName<<"' global function.\n";
		return;
	}
	
	// It is a function; cast it to a Function
	v8::Handle<v8::Function> func = v8::Handle<v8::Function>::Cast(func_val);
	functionObj->Reset(isolate_, func);
}

void V8Script::call(ScriptFunction* functionObj)
{
	if(!mV8Initialized)
		return;
	
	v8::HandleScope handle_scope( isolate_ );
	v8::Local<v8::Context> context = v8::Local<v8::Context>::New(isolate_, context_);
	v8::Context::Scope context_scope(context);

	// ---

	v8::Handle<v8::String> input_line = v8::String::New("test");

    const int argc = 1;
    v8::Handle<v8::Value> argv[argc] = { input_line };

	v8::Handle<v8::Value> result;
	{
		v8::TryCatch try_catch;

		v8::Local<v8::Function> process = v8::Local<v8::Function>::New(isolate_, *functionObj);
		result = process->Call(isolate_->GetCurrentContext()->Global(), argc, argv);
		if (try_catch.HasCaught()) {
			v8::utils::ReportException(isolate_, &try_catch);
			return;
		}
	}
    /*v8::String::Utf8Value str(result);
    const char* cstr = ToCString(str);
	console() << cstr << endl;*/
}

void V8Script::bind(float* var, std::string name)
{
	if(!mV8Initialized)
		return;
	
	v8::HandleScope handle_scope( isolate_ );
	v8::Local<v8::Context> context = v8::Local<v8::Context>::New(isolate_, context_);
	v8::Context::Scope context_scope(context);

	// ---
	
	// Set the options object as a property on the global object.
	//context->Global()->Set(v8::String::NewFromUtf8(isolate_, name.c_str()), v);
}

v8::Handle<v8::Object> V8Script::getGlobal()
{
	v8::HandleScope handle_scope( isolate_ );
	v8::Local<v8::Context> context = v8::Local<v8::Context>::New(isolate_, context_);

	return handle_scope.Close( context->Global() );
}





bool V8Script::InstallMaps(map<string, string>* opts,
                                         map<string, string>* output) {
  v8::HandleScope handle_scope(isolate_);

  // Wrap the map object in a JavaScript wrapper
  v8::Handle<v8::Object> opts_obj = WrapMap(opts);

  v8::Local<v8::Context> context =
      v8::Local<v8::Context>::New(isolate_, context_);

  // Set the options object as a property on the global object.
  context->Global()->Set(v8::String::NewFromUtf8(isolate_, "options"),
                         opts_obj);

  v8::Handle<v8::Object> output_obj = WrapMap(output);
  context->Global()->Set(v8::String::NewFromUtf8(isolate_, "output"),
                         output_obj);

  return true;
}


v8::Persistent<v8::ObjectTemplate> V8Script::map_template_;

// -----------------------------------
// --- A c c e s s i n g   M a p s ---
// -----------------------------------


// Utility function that wraps a C++ http request object in a
// JavaScript object.
v8::Handle<v8::Object> V8Script::WrapMap(map<string, string>* obj) {
  // Handle scope for temporary handles.
  v8::HandleScope handle_scope(isolate_);

  // Fetch the template for creating JavaScript map wrappers.
  // It only has to be created once, which we do on demand.
  if (map_template_.IsEmpty()) {
    v8::Handle<v8::ObjectTemplate> raw_template = MakeMapTemplate(isolate_);
    map_template_.Reset(isolate_, raw_template);
  }
  v8::Handle<v8::ObjectTemplate> templ =
      v8::Local<v8::ObjectTemplate>::New(isolate_, map_template_);

  // Create an empty map wrapper.
  v8::Handle<v8::Object> result = templ->NewInstance();

  // Wrap the raw C++ pointer in an External so it can be referenced
  // from within JavaScript.
  v8::Handle<v8::External> map_ptr = v8::External::New(isolate_, obj);

  // Store the map pointer in the JavaScript wrapper.
  result->SetInternalField(0, map_ptr);

  // Return the result through the current handle scope.  Since each
  // of these handles will go away when the handle scope is deleted
  // we need to call Close to let one, the result, escape into the
  // outer handle scope.
  return handle_scope.Close(result);
}


// Utility function that extracts the C++ map pointer from a wrapper
// object.
std::map<std::string, std::string>* V8Script::UnwrapMap(v8::Handle<v8::Object> obj) {
  v8::Handle<v8::External> field = v8::Handle<v8::External>::Cast(obj->GetInternalField(0));
  void* ptr = field->Value();
  return static_cast<std::map<std::string, std::string>*>(ptr);
}

void V8Script::MapGet(v8::Local<v8::String> name,
                                    const v8::PropertyCallbackInfo<v8::Value>& info) {
  // Fetch the map wrapped by this object.
  std::map<std::string, std::string>* obj = UnwrapMap(info.Holder());

  // Convert the JavaScript string to a std::string.
  string key = v8::utils::ObjectToString(name);

  // Look up the value if it exists using the standard STL ideom.
  map<string, string>::iterator iter = obj->find(key);

  // If the key is not present return an empty handle as signal
  if (iter == obj->end()) return;

  // Otherwise fetch the value and wrap it in a JavaScript string
  const string& value = (*iter).second;
  info.GetReturnValue().Set(v8::String::NewFromUtf8(
      info.GetIsolate(), value.c_str(), v8::String::kNormalString,
      static_cast<int>(value.length())));
}


void V8Script::MapSet(v8::Local<v8::String> name,
                                    v8::Local<v8::Value> value_obj,
                                    const v8::PropertyCallbackInfo<v8::Value>& info) {
  // Fetch the map wrapped by this object.
  map<string, string>* obj = UnwrapMap(info.Holder());

  // Convert the key and value to std::strings.
  string key = v8::utils::ObjectToString(name);
  string value = v8::utils::ObjectToString(value_obj);

  // Update the map.
  (*obj)[key] = value;

  // Return the value; any non-empty handle will work.
  info.GetReturnValue().Set(value_obj);
}


v8::Handle<v8::ObjectTemplate> V8Script::MakeMapTemplate(
    v8::Isolate* isolate) {
  v8::HandleScope handle_scope(isolate);

  v8::Handle<v8::ObjectTemplate> result = v8::ObjectTemplate::New();
  result->SetInternalFieldCount(1);
  result->SetNamedPropertyHandler(V8Script::MapGet, V8Script::MapSet);

  // Again, return the result through the current handle scope.
  return handle_scope.Close(result);
}
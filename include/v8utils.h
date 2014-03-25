#pragma once

#include "v8.h"

#include "cinder/app/App.h"


namespace v8 { namespace utils {

	// Extracts a C string from a V8 Utf8Value.
	const char* ToCString(const v8::String::Utf8Value& value);


	// Reads a file into a v8 string.
	v8::Handle<v8::String> ReadFile(v8::Isolate* isolate, const char* name);


	void ReportException(v8::Isolate* isolate, v8::TryCatch* try_catch);


	// The callback that is invoked by v8 whenever the JavaScript 'print'
	// function is called.  Prints its arguments on stdout separated by
	// spaces and ending with a newline.
	void Print(const v8::FunctionCallbackInfo<v8::Value>& args);

	// Convert a JavaScript string to a std::string.  To not bother too
	// much with string encodings we just use ascii.
	std::string ObjectToString(v8::Local<v8::Value> value);




	

// Used to be a macro, hence the uppercase name.
#define V8_DEFINE_CONSTANT(target, constant)                                \
  do {                                                                        \
    v8::Isolate* isolate = v8::Isolate::GetCurrent();                         \
    v8::Local<v8::String> constant_name =                                     \
        v8::String::NewFromUtf8(isolate, #constant);                          \
    v8::Local<v8::Number> constant_value =                                    \
        v8::Number::New(isolate, static_cast<double>(constant));              \
    v8::PropertyAttribute constant_attributes =                               \
        static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete);    \
    (target)->Set(constant_name, constant_value, constant_attributes);        \
  }                                                                           \
  while (0)

// Used to be a macro, hence the uppercase name.
template <typename TypeName>
inline void V8_SET_METHOD(const TypeName& recv,
                            const char* name,
                            v8::FunctionCallback callback) {
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  v8::HandleScope handle_scope(isolate);
  v8::Local<v8::FunctionTemplate> t = v8::FunctionTemplate::New(isolate,
                                                                callback);
  v8::Local<v8::Function> fn = t->GetFunction();
  v8::Local<v8::String> fn_name = v8::String::NewFromUtf8(isolate, name);
  fn->SetName(fn_name);
  recv->Set(fn_name, fn);
}
#define V8_SET_METHOD v8::utils::V8_SET_METHOD

// Used to be a macro, hence the uppercase name.
// Not a template because it only makes sense for FunctionTemplates.
inline void V8_SET_PROTOTYPE_METHOD(v8::Handle<v8::FunctionTemplate> recv,
                                      const char* name,
                                      v8::FunctionCallback callback) {
  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  v8::HandleScope handle_scope(isolate);
  v8::Local<v8::FunctionTemplate> t = v8::FunctionTemplate::New(isolate,
                                                                callback);
  recv->PrototypeTemplate()->Set(v8::String::NewFromUtf8(isolate, name),
                                 t->GetFunction());
}
#define V8_SET_PROTOTYPE_METHOD v8::utils::V8_SET_PROTOTYPE_METHOD


} }

/*
inline const char* v8::utils::ToCString(const v8::String::Utf8Value& value);
inline void v8::utils::ReportException(v8::Isolate* isolate, v8::TryCatch* handler);
inline v8::Handle<v8::String> v8::utils::ReadFile(v8::Isolate* isolate, const char* name);
inline void v8::utils::Print(const v8::FunctionCallbackInfo<v8::Value>& args);

*/




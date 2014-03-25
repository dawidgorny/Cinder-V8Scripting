#include "ExampleObjectWrap.h"

using namespace v8;

Persistent<Function> ExampleObjectWrap::constructor;

void ExampleObjectWrap::Init(Handle<Object> exports) {
	Isolate* isolate = Isolate::GetCurrent();
	HandleScope scope(isolate);

	// Prepare constructor template
	Local<FunctionTemplate> tpl = FunctionTemplate::New(Isolate::GetCurrent(), ExampleObjectWrap::New);
	tpl->InstanceTemplate()->SetInternalFieldCount(1);
	tpl->SetClassName(String::NewFromUtf8(Isolate::GetCurrent(), "ExampleObject"));

	// Prototype
	V8_SET_PROTOTYPE_METHOD(tpl, "setValue", setValue);
	V8_SET_PROTOTYPE_METHOD(tpl, "getValue", getValue);

	constructor.Reset(Isolate::GetCurrent(), tpl->GetFunction());
	exports->Set(String::NewFromUtf8(Isolate::GetCurrent(), "ExampleObject"), tpl->GetFunction());
}

void ExampleObjectWrap::New(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  if (args.IsConstructCall()) {
    // Invoked as constructor: `new ExampleObject(...)`
    //double value = args[0]->IsUndefined() ? 0 : args[0]->NumberValue();
    
	ExampleObject* obj = new ExampleObject();

	args.This()->SetAlignedPointerInInternalField(0, obj);
    args.GetReturnValue().Set(args.This());
  } else {
    // Invoked as plain function `ExampleObject(...)`, turn into construct call.
    const int argc = 1;
    Local<Value> argv[argc] = { args[0] };
    Local<Function> cons = Local<Function>::New(isolate, constructor);
    args.GetReturnValue().Set(cons->NewInstance(argc, argv));
  }
}


/*
 * ----
 */

void ExampleObjectWrap::setValue(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  if( args.Length() != 1 ) {
	  printf("Method expects 1 argument");
	  return;
  }

  void* obj_ptr = args.This()->GetAlignedPointerFromInternalField(0);
  ExampleObject* obj = static_cast<ExampleObject*>( obj_ptr );
  //ExampleObject* obj = ObjectWrap::Unwrap<ExampleObject>(args.This());
  obj->setValue( args[0]->NumberValue() );

  args.GetReturnValue().Set( Null() );
}

void ExampleObjectWrap::getValue(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  void* obj_ptr = args.This()->GetAlignedPointerFromInternalField(0);
  ExampleObject* obj = static_cast<ExampleObject*>( obj_ptr );
  //ExampleObject* obj = ObjectWrap::Unwrap<ExampleObject>(args.This());

  args.GetReturnValue().Set(Number::New(isolate, obj->getValue()));
}

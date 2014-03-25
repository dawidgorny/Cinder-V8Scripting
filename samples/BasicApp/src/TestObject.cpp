#include "TestObject.h"

using namespace v8;

Persistent<Function> TestObject::constructor;

TestObject::TestObject(double value) : value_(value) {
}

TestObject::~TestObject() {
}

void TestObject::Init(Handle<Object> exports) {

	//v8::HandleScope scope(isolate);

	// Prepare constructor template
	Local<FunctionTemplate> tpl = FunctionTemplate::New(Isolate::GetCurrent(), TestObject::New);
	tpl->InstanceTemplate()->SetInternalFieldCount(1);
	tpl->SetClassName(String::NewFromUtf8(Isolate::GetCurrent(), "TestObject"));

	// Prototype
	V8_SET_PROTOTYPE_METHOD(tpl, "plusOne", PlusOne);

	constructor.Reset(Isolate::GetCurrent(), tpl->GetFunction());
	exports->Set(String::NewFromUtf8(Isolate::GetCurrent(), "TestObject"), tpl->GetFunction());
}

void TestObject::New(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  if (args.IsConstructCall()) {
    // Invoked as constructor: `new TestObject(...)`
    double value = args[0]->IsUndefined() ? 0 : args[0]->NumberValue();
    TestObject* obj = new TestObject(value);
    obj->Wrap(args.This());
    args.GetReturnValue().Set(args.This());
  } else {
    // Invoked as plain function `TestObject(...)`, turn into construct call.
    const int argc = 1;
    Local<Value> argv[argc] = { args[0] };
    Local<Function> cons = Local<Function>::New(isolate, constructor);
    args.GetReturnValue().Set(cons->NewInstance(argc, argv));
  }
}

void TestObject::PlusOne(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  TestObject* obj = ObjectWrap::Unwrap<TestObject>(args.This());
  obj->value_ += 1;

  args.GetReturnValue().Set(Number::New(isolate, obj->value_));
}

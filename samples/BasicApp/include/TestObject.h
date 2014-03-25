#pragma once

#include "ObjectWrap.h"

class TestObject : public ObjectWrap {
 public:
	 static void Init(v8::Handle<v8::Object> exports);

 private:
  explicit TestObject(double value = 0);
  ~TestObject();

  static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void PlusOne(const v8::FunctionCallbackInfo<v8::Value>& args);
  static v8::Persistent<v8::Function> constructor;
  double value_;
};

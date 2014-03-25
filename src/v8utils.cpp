#include "v8utils.h"


namespace v8 { namespace utils {

	// Extracts a C string from a V8 Utf8Value.
	const char* ToCString(const v8::String::Utf8Value& value) {
	  return *value ? *value : "<string conversion failed>";
	}


	// Reads a file into a v8 string.
	v8::Handle<v8::String> ReadFile(v8::Isolate* isolate, const char* name) { 

	  FILE* file = fopen(name, "rb");
	  if (file == NULL) return v8::Handle<v8::String>();

	  fseek(file, 0, SEEK_END);
	  int size = ftell(file);
	  rewind(file);

	  char* chars = new char[size + 1];
	  chars[size] = '\0';
	  for (int i = 0; i < size;) {
		int read = static_cast<int>(fread(&chars[i], 1, size - i, file));
		i += read;
	  }
	  fclose(file);
	  v8::Handle<v8::String> result =
		  v8::String::NewFromUtf8(isolate, chars, v8::String::kNormalString, size);
	  delete[] chars;
	  return result;
	}


	void ReportException(v8::Isolate* isolate, v8::TryCatch* try_catch) {
	  v8::HandleScope handle_scope(isolate);
	  v8::String::Utf8Value exception(try_catch->Exception());
	  const char* exception_string = ToCString(exception);
	  v8::Handle<v8::Message> message = try_catch->Message();
	  if (message.IsEmpty()) {
		// V8 didn't provide any extra information about this error; just
		// print the exception.
		printf("%s\n", exception_string);
		ci::app::console() << exception_string << std::endl;
	  } else {
		// Print (filename):(line number): (message).
		v8::String::Utf8Value filename(message->GetScriptResourceName());
		const char* filename_string = ToCString(filename);
		int linenum = message->GetLineNumber();
		printf("%s:%i: %s\n", filename_string, linenum, exception_string);
		ci::app::console() << filename_string << ":"<< linenum << ": " << exception_string << std::endl;
		// Print line of source code.
		v8::String::Utf8Value sourceline(message->GetSourceLine());
		const char* sourceline_string = ToCString(sourceline);
		printf("%s\n", sourceline_string);
		ci::app::console() << sourceline_string << std::endl;
		// Print wavy underline (GetUnderline is deprecated).
		int start = message->GetStartColumn();
		for (int i = 0; i < start; i++) {
		  printf(" ");
		  ci::app::console() << " ";
		}
		int end = message->GetEndColumn();
		for (int i = start; i < end; i++) {
		  printf("^");
		  ci::app::console() << "^";
		}
		printf("\n");
		ci::app::console() << std::endl;
	  }
	}


	// The callback that is invoked by v8 whenever the JavaScript 'print'
	// function is called.  Prints its arguments on stdout separated by
	// spaces and ending with a newline.
	void Print(const v8::FunctionCallbackInfo<v8::Value>& args) {
	  bool first = true;
	  for (int i = 0; i < args.Length(); i++) {
		v8::HandleScope handle_scope(args.GetIsolate());
		if (first) {
		  first = false;
		} else {
		  printf(" ");
		  ci::app::console() << " ";
		}
		v8::String::Utf8Value str(args[i]);
		const char* cstr = ToCString(str);
		printf("%s", cstr);
		ci::app::console() << cstr;
	  }
	  printf("\n");
	  ci::app::console() << std::endl;
	  fflush(stdout);
	}


	// Convert a JavaScript string to a std::string.  To not bother too
	// much with string encodings we just use ascii.
	std::string ObjectToString(v8::Local<v8::Value> value) {
		v8::String::Utf8Value utf8_value(value);
		return std::string(*utf8_value);
	}

} }
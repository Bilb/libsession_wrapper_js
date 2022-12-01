#pragma once

#include <node.h>
#include <node_object_wrap.h>

#include "base_config.hpp"

#include "session/config/user_profile.hpp"

using v8::Context;
using v8::Exception;
using v8::Function;
using v8::FunctionCallbackInfo;
using v8::FunctionTemplate;
using v8::Isolate;
using v8::Local;
using v8::Number;
using v8::Object;
using v8::ObjectTemplate;
using v8::String;
using v8::Undefined;
using v8::Value;

class UserConfigWrapper : public ConfigBaseWrapper
{
public:
  static void Init(v8::Local<v8::Object> exports)
  {
    Isolate *isolate = exports->GetIsolate();
    Local<Context> context = isolate->GetCurrentContext();

    Local<ObjectTemplate> addon_data_tpl = ObjectTemplate::New(isolate);
    addon_data_tpl->SetInternalFieldCount(1); // 1 field for the UserConfigWrapper::New()
    Local<Object> addon_data =
        addon_data_tpl->NewInstance(context).ToLocalChecked();

    // Prepare constructor template
    Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New, addon_data);
    tpl->SetClassName(String::NewFromUtf8(isolate, "UserConfigWrapper").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Prototype
    NODE_SET_PROTOTYPE_METHOD(tpl, "getName", GetName);
    NODE_SET_PROTOTYPE_METHOD(tpl, "setName", SetName);
    SESSION_LINK_BASE_CONFIG

    Local<Function> constructor = tpl->GetFunction(context).ToLocalChecked();
    addon_data->SetInternalField(0, constructor);
    exports->Set(context, String::NewFromUtf8(isolate, "UserConfigWrapper").ToLocalChecked(),
                 constructor)
        .FromJust();
  }

private:
  explicit UserConfigWrapper()
  {
    initWithConfig(new session::config::UserProfile());
  }
  ~UserConfigWrapper() {}

  static void New(const v8::FunctionCallbackInfo<v8::Value> &args)
  {

    Isolate *isolate = args.GetIsolate();

    if (args.IsConstructCall())
    {
      // Invoked as constructor: `new UserConfigWrapper(...)`
      UserConfigWrapper *obj = new UserConfigWrapper();
      obj->Wrap(args.This());
      args.GetReturnValue().Set(args.This());
      return;
    }
    else
    {
      // Invoked as plain function `MyObject(...)`, turn into construct call throw
      isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate,
                              "Only syntax with new is supported")
              .ToLocalChecked()));
      return;
    }
  }

  static void GetName(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    Isolate *isolate = args.GetIsolate();

    UserConfigWrapper *obj = ObjectWrap::Unwrap<UserConfigWrapper>(args.Holder());
    auto asUserProfile = static_cast<session::config::UserProfile *>(obj->config);
    auto name = asUserProfile->get_name();
    if (name == nullptr)
    {
      args.GetReturnValue().Set(Null(isolate));
    }
    else
    {
      args.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, name->c_str()).ToLocalChecked());
    }
  }

  static void SetName(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    v8::Isolate *isolate = args.GetIsolate();

    if (args.Length() != 1)
    {
      isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate,
                              "Wrong number of arguments")
              .ToLocalChecked()));
      return;
    }

    if (!args[0]->IsString() && !args[0]->IsNull())
    {

      isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate,
                              "Wrong arguments")
              .ToLocalChecked()));
      return;
    }

    v8::String::Utf8Value str(isolate, args[0]);
    std::string cppStr(*str);

    UserConfigWrapper *obj = ObjectWrap::Unwrap<UserConfigWrapper>(args.Holder());
    auto asUserProfile = static_cast<session::config::UserProfile *>(obj->config);

    asUserProfile->set_name(cppStr);
  }
};

void InitAll(Local<Object> exports)
{
  UserConfigWrapper::Init(exports);
}

NODE_MODULE(session_util_wrapper, InitAll)

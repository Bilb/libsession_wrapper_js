#pragma once

#include <node.h>
#include <node_object_wrap.h>
#include "session/config/base.hpp"

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
using v8::Value;

#define SESSION_LINK_BASE_CONFIG                          \
  NODE_SET_PROTOTYPE_METHOD(tpl, "needsDump", NeedsDump); \
  NODE_SET_PROTOTYPE_METHOD(tpl, "needsPush", NeedsPush);

class ConfigBaseWrapper : public node::ObjectWrap
{
public:
  static void Init(v8::Local<v8::Object> exports)
  {

    Isolate *isolate = exports->GetIsolate();
    Local<Context> context = isolate->GetCurrentContext();

    Local<ObjectTemplate> addon_data_tpl = ObjectTemplate::New(isolate);
    addon_data_tpl->SetInternalFieldCount(1); // 1 field for the ConfigBaseWrapper::New()
    Local<Object> addon_data =
        addon_data_tpl->NewInstance(context).ToLocalChecked();

    // Prepare constructor template
    Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New, addon_data);
    tpl->SetClassName(String::NewFromUtf8(isolate, "ConfigBaseWrapper").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    Local<Function> constructor = tpl->GetFunction(context).ToLocalChecked();
    addon_data->SetInternalField(0, constructor);
    exports->Set(context, String::NewFromUtf8(isolate, "UserConfigWrapper").ToLocalChecked(),
                 constructor)
        .FromJust();
  }

protected:
  session::config::ConfigBase *config;

  void initWithConfig(session::config::ConfigBase *config)
  {

    this->config = config;
  }

  ~ConfigBaseWrapper()
  {
    // FIXME
    //  if (config)
    //  {
    //    config_free(config)
    //    config = NULL;
    //  }
  }
  ConfigBaseWrapper()
  {
    config = nullptr;
  }
  static void NeedsDump(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    ConfigBaseWrapper *obj = ObjectWrap::Unwrap<ConfigBaseWrapper>(args.Holder());
    args.GetReturnValue().Set(obj->config->needs_dump());
    return;
  }

  static void NeedsPush(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    ConfigBaseWrapper *obj = ObjectWrap::Unwrap<ConfigBaseWrapper>(args.Holder());
    args.GetReturnValue().Set(obj->config->needs_push());
  }

private:
  static void New(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    v8::Isolate *isolate = args.GetIsolate();

    if (args.IsConstructCall())
    {
      if (args.Length() != 0)
      {
        isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate,
                                "Wrong number of arguments")
                .ToLocalChecked()));
        return;
      }
      ConfigBaseWrapper *obj = new ConfigBaseWrapper();
      obj->Wrap(args.This());
      args.GetReturnValue().Set(args.This());
    }
    else
    {
      isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate,
                              "You need to call the constructor with the `new` syntax")
              .ToLocalChecked()));
      return;
    }
  }
};

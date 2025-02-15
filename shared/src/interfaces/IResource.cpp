#include "IResource.h"

void js::IResource::RequireBindingNamespaceWrapper(js::FunctionContext& ctx)
{
    if(!ctx.CheckArgCount(1)) return;

    std::string bindingName;
    if(!ctx.GetArg(0, bindingName)) return;

    Binding& binding = Binding::Get(bindingName);
    if(!ctx.Check(binding.IsValid(), "Invalid binding name")) return;
    IResource* resource = ctx.GetResource();
    if(!resource) return;

    v8::Local<v8::Module> bindingModule = binding.GetCompiledModule(resource);
    if(bindingModule->GetStatus() != v8::Module::Status::kEvaluated) resource->InitializeBinding(&binding);
    ctx.Return(bindingModule->GetModuleNamespace());
}

void js::IResource::InitializeBinding(js::Binding* binding)
{
    if(binding->GetName().ends_with("bootstrap.js")) return;  // Skip bootstrap bindings, those are handled separately

    v8::Local<v8::Module> module = binding->GetCompiledModule(this);
    if(module.IsEmpty())
    {
        Logger::Error("INTERNAL ERROR: Failed to compile binding module", binding->GetName());
        return;
    }
    if(module->GetStatus() == v8::Module::Status::kEvaluated) return;

    if(module->GetStatus() == v8::Module::Status::kEvaluating)
    {
        Logger::Error("INTERNAL ERROR: Binding module", binding->GetName(), "is already evaluating; circular dependency?");
        return;
    }

    module->Evaluate(GetContext());
    if(module->GetStatus() != v8::Module::Status::kEvaluated)
    {
        Logger::Error("INTERNAL ERROR: Failed to evaluate binding module", binding->GetName());
        v8::Local<v8::Value> exception = module->GetException();
        if(!exception.IsEmpty()) Logger::Error("INTERNAL ERROR:", *v8::String::Utf8Value(isolate, exception));
    }
}

void js::IResource::InitializeBindings(Binding::Scope scope, Module& altModule)
{
    std::vector<Binding*> bindings = Binding::GetBindingsForScope(scope);
    v8::Local<v8::Context> ctx = GetContext();

    {
        TemporaryGlobalExtension altExtension(ctx, "__alt", altModule.GetNamespace(this));
        TemporaryGlobalExtension cppBindingsExtension(ctx, "__cppBindings", Module::Get("cppBindings").GetNamespace(this));
        TemporaryGlobalExtension requireBindingExtension(ctx, "requireBinding", RequireBindingNamespaceWrapper);

        for(Binding* binding : bindings) InitializeBinding(binding);
    }
}

extern js::Class baseObjectClass;
bool js::IResource::IsBaseObject(v8::Local<v8::Value> val)
{
    return val->IsObject() && val.As<v8::Object>()->InstanceOf(GetContext(), baseObjectClass.GetTemplate(isolate).Get()->GetFunction(GetContext()).ToLocalChecked()).ToChecked();
}

extern js::Class bufferClass;
bool js::IResource::IsBuffer(v8::Local<v8::Value> val)
{
    return val->IsObject() && val.As<v8::Object>()->InstanceOf(GetContext(), bufferClass.GetTemplate(isolate).Get()->GetFunction(GetContext()).ToLocalChecked()).ToChecked();
}

void js::IResource::AddOwnedBuffer(Buffer* buffer, v8::Local<v8::Object> obj)
{
    Persistent<v8::Object> persistent(GetIsolate(), obj);
    persistent.SetWeak(buffer, js::WeakHandleCallback<js::Buffer>, v8::WeakCallbackType::kParameter);
    ownedBuffers.insert({ buffer, persistent });
}

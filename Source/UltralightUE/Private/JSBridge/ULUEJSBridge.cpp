/*
 *   Copyright (c) 2023 Mikael Aboagye & Ultralight Inc.
 *   All rights reserved.

 *   Permission is hereby granted, free of charge, to any person obtaining a copy
 *   of this software and associated documentation files (the "Software"), to deal
 *   in the Software without restriction, including without limitation the rights
 *   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *   copies of the Software, and to permit persons to whom the Software is
 *   furnished to do so, subject to the following conditions:

 *   The above copyright notice and this permission notice shall be included in all
 *   copies or substantial portions of the Software.

 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *   SOFTWARE.
 */

#include "JSBridge/ULUEJSBridge.h"
#include "ULUELogging.h"

#include <JavaScriptCore/JavaScript.h>

// ---------------------------------------------------------------------------
// Registered callback storage
// ---------------------------------------------------------------------------

static TMap<FString, FULUEJSBridge::FJSCallbackFunction>& GetRegisteredCallbacks()
{
	static TMap<FString, FULUEJSBridge::FJSCallbackFunction> Callbacks;
	return Callbacks;
}

// ---------------------------------------------------------------------------
// String conversion helpers
// ---------------------------------------------------------------------------

JSStringRef FULUEJSBridge::FStringToJSString(const FString& Str)
{
	FTCHARToUTF8 Converter(*Str);
	return JSStringCreateWithUTF8CString(Converter.Get());
}

FString FULUEJSBridge::JSValueToFString(JSContextRef Ctx, JSValueRef Value)
{
	if (!Ctx || !Value)
	{
		return FString();
	}

	JSValueRef Exception = nullptr;
	JSStringRef JSStr = JSValueToStringCopy(Ctx, Value, &Exception);
	if (!JSStr || Exception)
	{
		if (JSStr)
		{
			JSStringRelease(JSStr);
		}
		return FString();
	}

	size_t MaxSize = JSStringGetMaximumUTF8CStringSize(JSStr);
	TArray<char> Buffer;
	Buffer.SetNumUninitialized(static_cast<int32>(MaxSize));

	JSStringGetUTF8CString(JSStr, Buffer.GetData(), MaxSize);
	JSStringRelease(JSStr);

	return FString(UTF8_TO_TCHAR(Buffer.GetData()));
}

// ---------------------------------------------------------------------------
// Script evaluation
// ---------------------------------------------------------------------------

FString FULUEJSBridge::EvaluateScript(JSContextRef Ctx, const FString& Script)
{
	if (!Ctx)
	{
		UE_LOG(LogUltralightUE, Warning, TEXT("EvaluateScript: Invalid JSContext."));
		return FString();
	}

	JSStringRef JsScript = FStringToJSString(Script);
	JSValueRef Exception = nullptr;

	JSValueRef Result = JSEvaluateScript(Ctx, JsScript, nullptr, nullptr, 0, &Exception);
	JSStringRelease(JsScript);

	if (Exception)
	{
		FString ExStr = JSValueToFString(Ctx, Exception);
		UE_LOG(LogUltralightUE, Error, TEXT("EvaluateScript: JS Exception: %s"), *ExStr);
		return ExStr;
	}

	if (!Result)
	{
		return FString();
	}

	return JSValueToFString(Ctx, Result);
}

// ---------------------------------------------------------------------------
// Native callback trampoline
// ---------------------------------------------------------------------------

static JSValueRef NativeCallbackTrampoline(
	JSContextRef Ctx,
	JSObjectRef Function,
	JSObjectRef ThisObject,
	size_t ArgumentCount,
	const JSValueRef Arguments[],
	JSValueRef* Exception)
{
	// Retrieve the function name from private data
	void* PrivateData = JSObjectGetPrivate(Function);
	if (!PrivateData)
	{
		return JSValueMakeUndefined(Ctx);
	}

	FString* FuncName = static_cast<FString*>(PrivateData);
	FULUEJSBridge::FJSCallbackFunction* Callback = GetRegisteredCallbacks().Find(*FuncName);
	if (!Callback)
	{
		UE_LOG(LogUltralightUE, Warning, TEXT("NativeCallbackTrampoline: No callback registered for '%s'."), **FuncName);
		return JSValueMakeUndefined(Ctx);
	}

	// Convert arguments to FString array
	TArray<FString> Args;
	Args.Reserve(static_cast<int32>(ArgumentCount));
	for (size_t i = 0; i < ArgumentCount; ++i)
	{
		Args.Add(FULUEJSBridge::JSValueToFString(Ctx, Arguments[i]));
	}

	// Call the registered C++ function
	FString ResultStr = (*Callback)(Args);

	// Convert result back to JSValue
	JSStringRef JsResult = FULUEJSBridge::FStringToJSString(ResultStr);
	JSValueRef RetVal = JSValueMakeString(Ctx, JsResult);
	JSStringRelease(JsResult);

	return RetVal;
}

// ---------------------------------------------------------------------------
// Callback release (clean up stored FString when the JS object is GC'd)
// ---------------------------------------------------------------------------

static void NativeCallbackFinalize(JSObjectRef Object)
{
	void* PrivateData = JSObjectGetPrivate(Object);
	if (PrivateData)
	{
		FString* FuncName = static_cast<FString*>(PrivateData);
		delete FuncName;
	}
}

// ---------------------------------------------------------------------------
// Bind global function
// ---------------------------------------------------------------------------

void FULUEJSBridge::BindGlobalFunction(JSContextRef Ctx, const FString& FunctionName, FJSCallbackFunction Callback)
{
	if (!Ctx)
	{
		UE_LOG(LogUltralightUE, Warning, TEXT("BindGlobalFunction: Invalid JSContext."));
		return;
	}

	// Store the callback
	GetRegisteredCallbacks().Add(FunctionName, MoveTemp(Callback));

	// Create a JSClass with our trampoline and release callback
	JSClassDefinition ClassDef = kJSClassDefinitionEmpty;
	ClassDef.className = "ULUENativeFunction";
	ClassDef.callAsFunction = NativeCallbackTrampoline;
	ClassDef.finalize = NativeCallbackFinalize;

	JSClassRef FuncClass = JSClassCreate(&ClassDef);

	// Store the function name as private data (will be freed in finalize)
	FString* NameCopy = new FString(FunctionName);

	JSObjectRef FuncObj = JSObjectMake(Ctx, FuncClass, NameCopy);
	JSClassRelease(FuncClass);

	// Set on the global object
	JSStringRef JsName = FStringToJSString(FunctionName);
	JSObjectRef GlobalObj = JSContextGetGlobalObject(Ctx);
	JSObjectSetProperty(Ctx, GlobalObj, JsName, FuncObj, kJSPropertyAttributeNone, nullptr);
	JSStringRelease(JsName);

	UE_LOG(LogUltralightUE, Log, TEXT("BindGlobalFunction: Bound '%s' to global JS scope."), *FunctionName);
}

// ---------------------------------------------------------------------------
// Call JS function
// ---------------------------------------------------------------------------

FString FULUEJSBridge::CallJSFunction(JSContextRef Ctx, const FString& FunctionName, const TArray<FString>& Args)
{
	if (!Ctx)
	{
		UE_LOG(LogUltralightUE, Warning, TEXT("CallJSFunction: Invalid JSContext."));
		return FString();
	}

	JSObjectRef GlobalObj = JSContextGetGlobalObject(Ctx);

	// Get the function from the global object
	JSStringRef JsName = FStringToJSString(FunctionName);
	JSValueRef FuncVal = JSObjectGetProperty(Ctx, GlobalObj, JsName, nullptr);
	JSStringRelease(JsName);

	if (!FuncVal || !JSValueIsObject(Ctx, FuncVal))
	{
		UE_LOG(LogUltralightUE, Warning, TEXT("CallJSFunction: '%s' is not a function."), *FunctionName);
		return FString();
	}

	JSObjectRef FuncObj = JSValueToObject(Ctx, FuncVal, nullptr);
	if (!FuncObj || !JSObjectIsFunction(Ctx, FuncObj))
	{
		UE_LOG(LogUltralightUE, Warning, TEXT("CallJSFunction: '%s' is not callable."), *FunctionName);
		return FString();
	}

	// Build arguments array
	TArray<JSValueRef> JsArgs;
	JsArgs.Reserve(Args.Num());
	TArray<JSStringRef> JsArgStrings; // keep alive until after the call
	JsArgStrings.Reserve(Args.Num());

	for (const FString& Arg : Args)
	{
		JSStringRef JsStr = FStringToJSString(Arg);
		JsArgStrings.Add(JsStr);
		JsArgs.Add(JSValueMakeString(Ctx, JsStr));
	}

	JSValueRef Exception = nullptr;
	JSValueRef Result = JSObjectCallAsFunction(
		Ctx, FuncObj, GlobalObj,
		static_cast<size_t>(JsArgs.Num()), JsArgs.GetData(),
		&Exception);

	// Release argument strings
	for (JSStringRef JsStr : JsArgStrings)
	{
		JSStringRelease(JsStr);
	}

	if (Exception)
	{
		FString ExStr = JSValueToFString(Ctx, Exception);
		UE_LOG(LogUltralightUE, Error, TEXT("CallJSFunction '%s': Exception: %s"), *FunctionName, *ExStr);
		return ExStr;
	}

	if (!Result)
	{
		return FString();
	}

	return JSValueToFString(Ctx, Result);
}

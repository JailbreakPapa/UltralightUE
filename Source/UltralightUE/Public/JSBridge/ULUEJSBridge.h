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

#pragma once

#include "CoreMinimal.h"

// Forward declarations matching JavaScriptCore opaque types.
// We redeclare them here so consumers of this header do not need JSC headers.
typedef const struct OpaqueJSContext* JSContextRef;
typedef const struct OpaqueJSValue* JSValueRef;
typedef struct OpaqueJSValue* JSObjectRef;
typedef struct OpaqueJSString* JSStringRef;

/**
 * FULUEJSBridge
 *
 * Static utility class for interacting with JavaScriptCore via the C API.
 * Provides helpers for evaluating scripts, binding C++ callbacks as global
 * JavaScript functions, and type conversions between UE and JS types.
 */
class ULTRALIGHTUE_API FULUEJSBridge
{
public:
	/** Callback type for C++ functions callable from JavaScript. */
	typedef TFunction<FString(const TArray<FString>&)> FJSCallbackFunction;

	/** Evaluate a JavaScript expression and return the result as an FString. */
	static FString EvaluateScript(JSContextRef Ctx, const FString& Script);

	/**
	 * Bind a named C++ callback as a global JavaScript function.
	 * The callback receives all arguments as strings and returns a string result.
	 */
	static void BindGlobalFunction(JSContextRef Ctx, const FString& FunctionName, FJSCallbackFunction Callback);

	/**
	 * Call a named JavaScript function on the global object with string arguments.
	 * Returns the result as an FString.
	 */
	static FString CallJSFunction(JSContextRef Ctx, const FString& FunctionName, const TArray<FString>& Args);

	/** Convert a JSValueRef to an FString. */
	static FString JSValueToFString(JSContextRef Ctx, JSValueRef Value);

	/** Convert an FString to a JSStringRef. Caller must release via JSStringRelease(). */
	static JSStringRef FStringToJSString(const FString& Str);
};

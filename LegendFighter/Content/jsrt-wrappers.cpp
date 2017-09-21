// Copyright 2015 Paul Vick
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "pch.h"
#include "jsrt-wrappers.h"

namespace jsrt
{
    const std::wstring typed_array_type<char, false>::type_name = L"Int8";
    const std::wstring typed_array_type<unsigned char, false>::type_name = L"Uint8";
    const std::wstring typed_array_type<short, false>::type_name = L"Int16";
    const std::wstring typed_array_type<unsigned short, false>::type_name = L"Uint16";
    const std::wstring typed_array_type<int, false>::type_name = L"Int32";
    const std::wstring typed_array_type<unsigned int, false>::type_name = L"Uint32";
    const std::wstring typed_array_type<float, false>::type_name = L"Float32";
    const std::wstring typed_array_type<double, false>::type_name = L"Float64";

    void runtime::dispose()
    {
        // TODO: Throws an access violation in this case, which shouldn't happen
        if (!is_valid())
        {
            throw invalid_argument_exception();
        }
        runtime::translate_error_code(JsDisposeRuntime(_handle));
        _handle = JS_INVALID_RUNTIME_HANDLE;
    }

    size_t runtime::memory_usage() const
    {
        // TODO: Throws an access violation in this case, which shouldn't happen
        if (!is_valid())
        {
            throw invalid_argument_exception();
        }

        size_t memoryUsage;
        runtime::translate_error_code(JsGetRuntimeMemoryUsage(_handle, &memoryUsage));
        return memoryUsage;
    }

    size_t runtime::memory_limit() const
    {
        // TODO: Throws an access violation in this case, which shouldn't happen
        if (!is_valid())
        {
            throw invalid_argument_exception();
        }

        size_t memoryLimit;
        runtime::translate_error_code(JsGetRuntimeMemoryLimit(_handle, &memoryLimit));
        return memoryLimit;
    }

    void runtime::set_memory_limit(size_t memory_limit) const
    {
        // TODO: Throws an access violation in this case, which shouldn't happen
        if (!is_valid())
        {
            throw invalid_argument_exception();
        }

        runtime::translate_error_code(JsSetRuntimeMemoryLimit(_handle, memory_limit));
    }

    void runtime::collect_garbage() const
    {
        // TODO: Throws an access violation in this case, which shouldn't happen
        if (!is_valid())
        {
            throw invalid_argument_exception();
        }

        runtime::translate_error_code(JsCollectGarbage(_handle));
    }

    void runtime::set_memory_allocation_callback(void *callbackState, JsMemoryAllocationCallback allocationCallback) const
    {
        // TODO: Throws an access violation in this case, which shouldn't happen
        if (!is_valid())
        {
            throw invalid_argument_exception();
        }

        runtime::translate_error_code(JsSetRuntimeMemoryAllocationCallback(_handle, callbackState, allocationCallback));
    }

    void runtime::set_before_collect_callback(void *callbackState, JsBeforeCollectCallback beforeCollectCallback) const
    {
        // TODO: Throws an access violation in this case, which shouldn't happen
        if (!is_valid())
        {
            throw invalid_argument_exception();
        }

        runtime::translate_error_code(JsSetRuntimeBeforeCollectCallback(_handle, callbackState, beforeCollectCallback));
    }

    void runtime::disable_execution() const
    {
        // TODO: Throws an access violation in this case, which shouldn't happen
        if (!is_valid())
        {
            throw invalid_argument_exception();
        }

        runtime::translate_error_code(JsDisableRuntimeExecution(_handle));
    }

    void runtime::enable_execution() const
    {
        // TODO: Throws an access violation in this case, which shouldn't happen
        if (!is_valid())
        {
            throw invalid_argument_exception();
        }

        runtime::translate_error_code(JsEnableRuntimeExecution(_handle));
    }

    bool runtime::is_execution_disabled() const
    {
        // TODO: Throws an access violation in this case, which shouldn't happen
        if (!is_valid())
        {
            throw invalid_argument_exception();
        }

        bool value;
        runtime::translate_error_code(JsIsRuntimeExecutionDisabled(_handle, &value));
        return value;
    }

    void runtime::translate_error_code(JsErrorCode errorCode)
    {
        switch (errorCode)
        {
        case JsNoError:
            return;
        case JsErrorInvalidArgument:
            throw invalid_argument_exception();
        case JsErrorNullArgument:
            throw null_argument_exception();
        case JsErrorNoCurrentContext:
            throw no_current_context_exception();
        case JsErrorInExceptionState:
            throw in_exception_state_exception();
        case JsErrorNotImplemented:
            throw not_implemented_exception();
        case JsErrorWrongThread:
            throw wrong_thread_exception();
        case JsErrorRuntimeInUse:
            throw runtime_in_use_exception();
        case JsErrorBadSerializedScript:
            throw bad_serialized_script_exception();
        case JsErrorInDisabledState:
            throw in_disabled_state_exception();
        case JsErrorCannotDisableExecution:
            throw cannot_disable_execution_exception();
        case JsErrorHeapEnumInProgress:
            throw heap_enum_in_progress_exception();
        case JsErrorArgumentNotObject:
            throw argument_not_object_exception();
        case JsErrorInProfileCallback:
            throw in_profile_callback_exception();
        case JsErrorInThreadServiceCallback:
            throw in_thread_service_callback_exception();
        case JsErrorCannotSerializeDebugScript:
            throw cannot_serialize_debug_script_exception();
        case JsErrorAlreadyDebuggingContext:
            throw already_debugging_context_exception();
        case JsErrorAlreadyProfilingContext:
            throw already_profiling_context_exception();
        case JsErrorIdleNotEnabled:
            throw idle_not_enabled_exception();
        case JsCannotSetProjectionEnqueueCallback:
            throw cannot_set_projection_enqueue_callback_exception();
        case JsErrorCannotStartProjection:
            throw cannot_start_projection_exception();
        case JsErrorInObjectBeforeCollectCallback:
            throw in_object_before_collect_callback_exception();
        case JsErrorObjectNotInspectable:
            throw object_not_inspectable_exception();
        case JsErrorPropertyNotSymbol:
            throw property_not_symbol_exception();
        case JsErrorPropertyNotString:
            throw property_not_string_exception();
        case JsErrorOutOfMemory:
            throw out_of_memory_exception();
        case JsErrorScriptException:
            {
                JsValueRef exception;

                if (JsGetAndClearException(&exception) != JsNoError)
                {
                    // Something has gone very wrong.
                    throw new fatal_exception();
                }

                throw script_exception(value(exception));
            }
        case JsErrorScriptCompile:
            {
                JsValueRef exception;

                if (JsGetAndClearException(&exception) != JsNoError)
                {
                    // Something has gone very wrong.
                    throw new fatal_exception();
                }

                throw script_compile_exception(compile_error(exception));
            }
        case JsErrorScriptTerminated:
            throw script_terminated_exception();
        case JsErrorScriptEvalDisabled:
            throw script_eval_disabled_exception();
        case JsErrorFatal:
            throw fatal_exception();
        default:
            throw fatal_exception();
        }
    }

    context runtime::create_context() const
    {
        // TODO: Throws an access violation in this case, which shouldn't happen
        if (!is_valid())
        {
            throw invalid_argument_exception();
        }

        JsContextRef newContext;

        runtime::translate_error_code(JsCreateContext(_handle, &newContext));

        return context(newContext);
    }

    void reference::set_before_collect_callback(void *callback_state, JsObjectBeforeCollectCallback beforeCollectCallback) const
    {
        runtime::translate_error_code(JsSetObjectBeforeCollectCallback(_ref, callback_state, beforeCollectCallback));
    }

    void context::start_debugging()
    {
        runtime::translate_error_code(JsStartDebugging());
    }
#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
    IActiveScriptProfilerHeapEnum *context::enumerate_heap()
    {
        IActiveScriptProfilerHeapEnum *enumerator;
        runtime::translate_error_code(JsEnumerateHeap(&enumerator));
        return enumerator;
    }

    bool context::is_enumerating_heap()
    {
        bool value;
        runtime::translate_error_code(JsIsEnumeratingHeap(&value));
        return value;
    }
#endif
    void context::set_exception(value object)
    {
        runtime::translate_error_code(JsSetException(object.handle()));
    }

    value context::get_and_clear_exception()
    {
        JsValueRef exception;
        runtime::translate_error_code(JsGetAndClearException(&exception));
        return value(exception);
    }

    function_base context::parse(std::wstring script, JsSourceContext sourceContext, std::wstring sourceUrl)
    {
        JsValueRef result = nullptr;

        runtime::translate_error_code(JsParseScript(script.c_str(), sourceContext, sourceUrl.c_str(), &result));

        return function_base(result);
    }

    value context::evaluate(std::wstring script, JsSourceContext sourceContext, std::wstring sourceUrl)
    {
        JsValueRef result = nullptr;

        runtime::translate_error_code(JsRunScript(script.c_str(), sourceContext, sourceUrl.c_str(), &result));

        return value(result);
    }

    function_base context::parse_serialized(std::wstring script, unsigned char *buffer, JsSourceContext sourceContext, std::wstring sourceUrl)
    {
        JsValueRef result = nullptr;

        runtime::translate_error_code(JsParseSerializedScript(script.c_str(), buffer, sourceContext, sourceUrl.c_str(), &result));

        return function_base(result);
    }

    value context::evaluate_serialized(std::wstring script, unsigned char *buffer, JsSourceContext sourceContext, std::wstring sourceUrl)
    {
        JsValueRef result = nullptr;

        runtime::translate_error_code(JsRunSerializedScript(script.c_str(), buffer, sourceContext, sourceUrl.c_str(), &result));

        return value(result);
    }

    void CALLBACK context::promise_thunk(JsValueRef task, void *callbackState)
    {
        (*static_cast<std::function<void(jsrt::function<void>)> *>(callbackState))(jsrt::function<void>(task));
    }

    void CALLBACK context::uwp_thunk(JsProjectionCallback jsCallback, JsProjectionCallbackContext jsContext, void *callbackState)
    {
        (*static_cast<std::function<void(std::function<void()>)> *>(callbackState))(std::bind(jsCallback, jsContext));
    }

    value context::undefined()
    {
        JsValueRef undefinedValue;
        runtime::translate_error_code(JsGetUndefinedValue(&undefinedValue));
        return value(undefinedValue);
    }

    value context::null()
    {
        JsValueRef nullValue;
        runtime::translate_error_code(JsGetNullValue(&nullValue));
        return object(nullValue);
    }

    object context::global()
    {
        JsValueRef globalObject;
        runtime::translate_error_code(JsGetGlobalObject(&globalObject));
        return object(globalObject);
    }

    symbol property_id::symbol() const
    {
        JsValueRef result;
        runtime::translate_error_code(JsGetSymbolFromPropertyId(_ref, &result));
        return jsrt::symbol(result);
    }

    property_id property_id::create(jsrt::symbol symbol)
    {
        JsPropertyIdRef propertyId;
        runtime::translate_error_code(JsGetPropertyIdFromSymbol(symbol.handle(), &propertyId));
        return property_id(propertyId);
    }

	boolean boolean::convert(value value)
    {
        JsValueRef booleanValue;
        runtime::translate_error_code(JsConvertValueToBoolean(value.handle(), &booleanValue));
        return boolean(booleanValue);
    }

    number number::convert(value value)
    {
        JsValueRef numberValue;
        runtime::translate_error_code(JsConvertValueToNumber(value.handle(), &numberValue));
        return number(numberValue);
    }

    string string::convert(value value)
    {
        JsValueRef stringValue;
        runtime::translate_error_code(JsConvertValueToString(value.handle(), &stringValue));
        return string(stringValue);
    }

    std::vector<symbol> object::get_own_property_symbols() const
    {
        std::vector<symbol> symbolsVector;

        JsValueRef symbols;
        runtime::translate_error_code(JsGetOwnPropertySymbols(handle(), &symbols));

        auto symbolsArray = array<symbol>(symbols);

        for (int index = 0; index < symbolsArray.size(); index++)
        {
            symbolsVector.push_back(symbolsArray[index]);
        }

        return symbolsVector;
    }

	std::vector<std::wstring> object::get_own_property_names() const
	{
        std::vector<std::wstring> namesVector;

        JsValueRef names;
        runtime::translate_error_code(JsGetOwnPropertyNames(handle(), &names));

        auto namesArray = array<std::wstring>(names);

        for (int index = 0; index < namesArray.size(); index++)
        {
            namesVector.push_back(namesArray[index]);
        }

        return namesVector;
    }
}
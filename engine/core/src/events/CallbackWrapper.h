#pragma once

template <typename FuncSignature>
struct CallbackWrapper;

template <typename TRet, typename... TArgs>
struct CallbackWrapper<TRet(TArgs...)> {
    static inline std::function<TRet(TArgs...)> callback;

    static TRet call(TArgs... args) { return callback(std::forward<TArgs>(args)...); }
};
//template <typename TRet, typename... TArgs>
//std::function<TRet(TArgs...)> CallbackWrapper<TRet(TArgs...)>::callback;
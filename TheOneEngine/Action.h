#pragma once

class Callable {};

template<class... Params>
class Action
{
private:
    using ActionPtr = void (Callable::*)(Params...);
    ActionPtr action;
    Callable* obj;
    void* func;

public:
    Action() : obj(nullptr), func(nullptr) {}

    Action(void* _func) : obj(nullptr), func(_func) {}

    template<class T>
    Action(void (T::* _action)(Params...), void* _obj)
    {
        action = reinterpret_cast<ActionPtr>(_action);
        obj = static_cast<Callable*>(_obj);
    }

    void Execute(Params... params)
    {
        if (obj) {
            (obj->*action)(params...);
        }
        else {
            auto f = reinterpret_cast<void(*)(Params...)>(func);
            f(params...);
        }
    }

    void operator()(Params... params) {
        Execute(params...);
    }

    bool operator==(const Action<>& other) {
        return obj == other.obj && GetActionPtr() == other.GetActionPtr();
    }

    void* GetObj() const { return obj; }
    void* GetActionPtr() const { return obj ? (void*&)action : func; }
};
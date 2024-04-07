#pragma once

#include <functional>


/**
 * @brief A placeholder class for callable objects.
 */
class Callable {};

/**
 * @brief Represents an action that can be executed.
 * @tparam _params Parameter types for the action.
 */
template<class... _params>
class Action
{
private:
    using MemberFunctionPtr = void (Callable::*)(_params...);

    MemberFunctionPtr ActionPtr;    ///< Pointer to a member function.
    Callable* ObjectPtr;            ///< Pointer to the object associated with the member function.
    std::function<void(_params...)> FreeFunction; ///< Function pointer for free functions.

public:
    /**
     * @brief Default constructor.
     */
    Action() : ObjectPtr(nullptr), FreeFunction(nullptr) {}

    /**
     * @brief Constructor for initializing with a free function.
     * @param func Pointer to the free function.
     */
    Action(void* func) : FreeFunction(reinterpret_cast<void(*)(_params...)>(func)) {}

    /**
     * @brief Constructor for initializing with a member function and object.
     * @tparam T Type of the object.
     * @param action Pointer to the member function.
     * @param obj Pointer to the object.
     */
    template<class T>
    Action(void (T::* action)(_params...), void* obj)
    {
        ActionPtr = reinterpret_cast<MemberFunctionPtr>(action);
        ObjectPtr = static_cast<Callable*>(obj);
    }

    /**
     * @brief Executes the action.
     * @param params Parameters for the action.
     */
    void Execute(_params... params)
    {
        if (ObjectPtr && ActionPtr) {
            (ObjectPtr->*ActionPtr)(params...);
        }
        else if (FreeFunction) {
            FreeFunction(params...);
        }
    }

    /**
     * @brief Overloaded function call operator to execute the action.
     * @param params Parameters for the action.
     */
    void operator()(_params... params) {
        Execute(params...);
    }

    /**
     * @brief Checks if this action is equal to another action.
     * @param other Another action to compare.
     * @return True if equal, false otherwise.
     */
    bool operator==(const Action<_params...>& other) const {
        return ObjectPtr == other.ObjectPtr && GetActionPtr() == other.GetActionPtr();
    }

    /**
     * @brief Gets the object pointer associated with this action.
     * @return Pointer to the object.
     */
    Callable* GetObjectPtr() const { return ObjectPtr; }

    /**
     * @brief Gets the function pointer associated with this action.
     * @return Function pointer.
     */
    std::function<void(_params...)> GetActionPtr() const
    {
        if (ObjectPtr && ActionPtr)
            return std::bind(ActionPtr, ObjectPtr, std::placeholders::_1);

        return FreeFunction;
    }
};
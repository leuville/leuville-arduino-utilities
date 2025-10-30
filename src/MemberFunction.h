/*
 * Module: functor
 *
 * Function: Functor pattern
 *
 * Copyright and license: See accompanying LICENSE file
 *
 * Author: Laurent Nel
 */

#pragma once

namespace leuville {
namespace simple_template_library {

/*
 * Member function wrapper
 *
 * T = target type
 * R = return type
 * Args = parameters types
 */
template <typename T, typename R, typename ... Args>
class MemberFunction {

	using MemberFuncPtr = R(T::*)(Args...);	// callback type (member function pointer)

protected:

	T * 			_target;	// target object
	MemberFuncPtr	_func;		// member function to call on target with operator()

public:

	MemberFunction(T * target = nullptr, MemberFuncPtr func = nullptr)
		: _target(target), _func(func) {
	}

	virtual R operator()(Args ... args) {
		return ((*_target).*_func)(args...);
	}
};

}
}
//
//  tracking_ptr.h
//  tracking_ptr
//

#ifndef __tracking_ptr__tracking_ptr_h__
#define __tracking_ptr__tracking_ptr_h__

#include <iostream>
#include <functional>

#include "tracking_ptr_iface.h"

template <typename T>
class tracking_ptr : public tracking_ptr_iface {
public:
	tracking_ptr(T & ref, std::function<void()> on_invalidation) : ref_(&ref), on_invalidation_(on_invalidation) {
		std::cout << "tracking_ptr: ctor" << std::endl;
		// register to tracker
		ref_->track(*this);
	}
	
	// TODO:
	//  - on copy: register the copy
	//  - on move: unregister the original, register the new
	
	~tracking_ptr() {
		std::cout << "tracking_ptr: dtor" << std::endl;
		// unregister from tracker
		if (ref_) {
			ref_->free(*this);
		}
	}
	
	T * get() {
		std::cout << "tracking_ptr: get" << std::endl;
		return ref_;
	}
	
	T * operator -> () {
		std::cout << "tracking_ptr: ->" << std::endl;
		if (ref_) {
			return ref_;
		} else {
			throw std::runtime_error("invalid reference");
		}
	}
	
	virtual void origin_died() override {
		std::cout << "tracking_ptr: origin_died" << std::endl;
		on_invalidation_();
		ref_ = nullptr;
	}
	
protected:
	T * ref_;
	std::function<void()> on_invalidation_;
};

#endif

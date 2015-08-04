//
//  sa_ref.h
//  selfaware_reference
//
//  Created by a on 04/08/15.
//  Copyright (c) 2015 a. All rights reserved.
//

#ifndef __selfaware_reference__sa_ref__
#define __selfaware_reference__sa_ref__

#include <iostream>
#include <functional>

#include "tracking_ptr_iface.h"

template <typename T>
class tracking_ptr : public tracking_ptr_iface {
public:
	tracking_ptr(T & ref, std::function<void()> on_invalidation) : ref_(&ref), on_invalidation_(on_invalidation) {
		std::cout << "sar: ctor" << std::endl;
		// register to tracker
		ref_->track(*this);
	}
	
	// TODO:
	//  - on copy: register the copy
	//  - on move: unregister the original, register the new
	
	~tracking_ptr() {
		std::cout << "sar: dtor" << std::endl;
		// unregister from tracker
		if (ref_) {
			ref_->free(*this);
		}
	}
	
	/*T & get() {
		std::cout << "get" << std::endl;
		return ref_;
	}*/
	
	T * operator -> () {
		std::cout << "sar: ->" << std::endl;
		if (ref_) {
			return ref_;
		} else {
			throw std::runtime_error("invalid reference");
		}
	}
	
	virtual void origin_died() override {
		std::cout << "sar: origin_died" << std::endl;
		on_invalidation_();
		ref_ = nullptr;
	}
	
protected:
	T * ref_;
	std::function<void()> on_invalidation_;
};

#endif /* defined(__selfaware_reference__sa_ref__) */

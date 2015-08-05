//
//  tracking_ptr.h
//  tracking_ptr
//

#ifndef __tracking_ptr__tracking_ptr_h__
#define __tracking_ptr__tracking_ptr_h__

#include <iostream>
#include <functional>

#include "tracking_ptr_iface.h"
#include "tracking_ptr_logging.h"

template <typename T>
class tracking_ptr : public tracking_ptr_iface {
public:
	//! Default ("empty/nullptr") constructor
	tracking_ptr() : ref_(nullptr), on_invalidation_(nullptr) {
	}
	
	//! Constructor for valid tracking_ptr creation
	tracking_ptr(T & ref, std::function<void()> on_invalidation) : ref_(&ref), on_invalidation_(on_invalidation) {
		TRACKING_PTR_LOG("tracking_ptr: ctor")
		// register to tracker
		ref_->track(*this);
	}
	
	//! Copy constructor
	tracking_ptr(const tracking_ptr & other) : ref_(other.ref_), on_invalidation_(other.on_invalidation_) {
		TRACKING_PTR_LOG("tracking_ptr: copy ctor (" << &other << ") -> " << this)
		// on copy: register the copied pointer
		if (ref_) {
			ref_->track(*this);
		} else {
			TRACKING_PTR_LOG("tracking_ptr: copy constructed a nullptr" << this)
		}
	}
	
	//! Copy assignment
	tracking_ptr & operator = (const tracking_ptr & other) {
		if (&other == this) {
			return *this;
		}
		tracking_ptr tmp(other);
		swap_with(tmp);
		return *this;
	}
	
	//! Move constructor
	tracking_ptr(tracking_ptr && other) : ref_(std::move(other.ref_)), on_invalidation_(std::move(other.on_invalidation_)) {
		TRACKING_PTR_LOG("tracking_ptr: move ctor (" << &other << ") -> " << this)
		// on move: unregister the original pointer, register the new pointer
		ref_->free(other);
		ref_->track(*this);
		
		// destroy the original
		other.ref_ = nullptr;
		other.on_invalidation_ = nullptr;
	}
	
	//! Move assignment
	tracking_ptr & operator = (tracking_ptr && other) {
		if (&other == this) {
			return *this;
		}
		swap_with(other);
		return *this;
	}
	
	//! Destructor
	~tracking_ptr() {
		TRACKING_PTR_LOG("tracking_ptr: dtor")
		// unregister from tracker
		if (ref_) {
			ref_->free(*this);
		}
	}
	
	T * get() {
		TRACKING_PTR_LOG("tracking_ptr: get")
		return ref_;
	}
	
	T * operator -> () {
		TRACKING_PTR_LOG("tracking_ptr: ->")
		if (ref_) {
			return ref_;
		} else {
			throw std::runtime_error("invalid reference");
		}
	}
	
	// TODO: reset()
	
	// TODO: on_invalidation setter
	
	virtual void origin_died() override {
		TRACKING_PTR_LOG("tracking_ptr: origin_died")
		on_invalidation_();
		ref_ = nullptr;
	}
	
protected:
	T * ref_;
	std::function<void()> on_invalidation_;
	// TODO: mutex for thread safety? (and in tracker too?)
	
private:
	void swap_with(tracking_ptr & other) {
		// swap
		std::swap(ref_, other.ref_);
		std::swap(on_invalidation_, other.on_invalidation_);
		
		// "re-register" with correct tracking_ptr addresses
		if (ref_) {
			ref_->free(other);
			ref_->track(*this);
		}
		if (other.ref_) {
			other.ref_->free(*this);
			other.ref_->track(other);
		}
	}
};

#endif

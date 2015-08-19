//
//  tracking_ptr.h
//  tracking_ptr
//  Copyright (c) 2015, Martin Mi, released under BSD 3-Clause License (see LICENSE file for details)
//

#ifndef __tracking_ptr__tracking_ptr_h__
#define __tracking_ptr__tracking_ptr_h__

#include <iostream>
#include <functional>

#include "tracking_ptr_iface.h"
#include "tracking_ptr_logging.h"
#include "tracked.h"

template <typename T>
class tracking_ptr : public tracking_ptr_iface {
public:
	//! Default ("empty/nullptr") constructor
	tracking_ptr() : ref_(nullptr), on_invalidation_(nullptr) {
		TRACKING_PTR_LOG("tracking_ptr " << this << ": null ctor")
	}
	
	//! Constructor for valid tracking_ptr creation
	tracking_ptr(T & ref, std::function<void(tracking_ptr &)> on_invalidation = nullptr) : ref_(&ref), on_invalidation_(on_invalidation) {
		TRACKING_PTR_LOG("tracking_ptr " << this << ": ctor(" << &ref_ << ")")
		static_assert(std::is_convertible<T *, tracked *>::value, "Type T in tracking_ptr<T> must be (publicly) derived from 'tracked' aka 'ptr_tracker'");
		// register to tracker
		ref_->track(*this);
	}
	
	//! Copy constructor
	tracking_ptr(const tracking_ptr & other) : ref_(other.ref_), on_invalidation_(other.on_invalidation_) {
		TRACKING_PTR_LOG("tracking_ptr " << this << ": copy ctor (" << &other << ")")
		// on copy: register the copied pointer
		if (ref_) {
			ref_->track(*this);
		} else {
			TRACKING_PTR_LOG("tracking_ptr: copy constructed a nullptr" << this)
		}
	}
	
	//! Copy assignment
	tracking_ptr & operator = (const tracking_ptr & other) {
		TRACKING_PTR_LOG("tracking_ptr " << this << ": copy assignment = " << &other)
		
		if (&other == this) {
			return *this;
		}
		tracking_ptr tmp(other);
		swap_with(tmp);
		return *this;
	}
	
	//! Move constructor
	tracking_ptr(tracking_ptr && other) : ref_(std::move(other.ref_)), on_invalidation_(std::move(other.on_invalidation_)) {
		TRACKING_PTR_LOG("tracking_ptr " << this << ": move ctor (" << &other << ")")
		// on move: unregister the original pointer, register the new pointer
		ref_->free(other);
		ref_->track(*this);
		
		// destroy the original
		other.ref_ = nullptr;
		other.on_invalidation_ = nullptr;
	}
	
	//! Move assignment
	tracking_ptr & operator = (tracking_ptr && other) {
		TRACKING_PTR_LOG("tracking_ptr " << this << ": move assignment = " << &other)

		if (&other == this) {
			return *this;
		}
		swap_with(other);
		return *this;
	}
	
	//! Destructor
	~tracking_ptr() {
		TRACKING_PTR_LOG("tracking_ptr " << this << ": ~tracking_ptr")
		// unregister from tracker
		if (ref_) {
			ref_->free(*this);
		}
	}
	
	T * get() {
		TRACKING_PTR_LOG("tracking_ptr " << this << ": get")
		return static_cast<T*>(ref_);
	}
	
	T & operator * () const {
		TRACKING_PTR_LOG("tracking_ptr " << this << ": *")
		return static_cast<T &>(*ref_);
	}
	
	T * operator -> () {
		TRACKING_PTR_LOG("tracking_ptr " << this << ": ->")
		if (ref_) {
			return static_cast<T*>(ref_);
		} else {
			throw std::runtime_error("invalid reference");
		}
	}
	
	bool operator < (const tracking_ptr & other) const {
		TRACKING_PTR_LOG("tracking_ptr " << this << ": <")
		return ref_ < other.ref_;
	}
	
	// TODO: reset()
	
	// TODO: on_invalidation setter
	
	virtual void origin_died() override {
		TRACKING_PTR_LOG("tracking_ptr " << this << ": origin_died")
		if (on_invalidation_) {
			// Invalidate the pointer
			ref_ = nullptr;
			// Call the invalidation handler
			// (move it to a local variable (to stack) to avoid invalid access to it if it is deleted by on_invalidation_)
			auto on_invalidation_local = std::move(on_invalidation_);
			on_invalidation_local(*this);
		} else {
			TRACKING_PTR_LOG("tracking_ptr " << this << ": origin_died: on_invalidation_ is null")
		}
	}
	
protected:
	tracked * ref_;
	std::function<void(tracking_ptr &)> on_invalidation_;
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

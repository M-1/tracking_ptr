//
//  tracked.h
//  tracking_ptr
//  Copyright (c) 2015, Martin Mi, released under BSD 3-Clause License (see LICENSE file for details)
//

#ifndef __tracking_ptr__tracked_h__
#define __tracking_ptr__tracked_h__

#include <iostream>
#include <set>

#include "tracking_ptr_iface.h"
#include "tracking_ptr_logging.h"

class ptr_tracker {
public:
	ptr_tracker() : active_references_() {}
	
	//! Copying the tracked object does not make sense: Would referencing objects have to hold multiple references? Should they be informed about destruction of just one, or all copies?
	ptr_tracker(const ptr_tracker & dummy) = delete;
	
	//! TODO: handle moving instead of disabling it: update all tracking_ptrs pointing to this
	ptr_tracker(ptr_tracker && dummy) = delete;
	
	//! Deleted copy assignment
	ptr_tracker & operator = (const ptr_tracker & dummy) = delete;
	
	//! TODO: handle moving
	ptr_tracker & operator = (ptr_tracker && dummy) = delete;
	
	void track(tracking_ptr_iface & r) {
		TRACKING_PTR_LOG("tracker: track " << &r)
		active_references_.emplace(&r);
	}
	
	void free(tracking_ptr_iface & r) {
		TRACKING_PTR_LOG("tracker: free " << &r)
		active_references_.erase(&r);
	}
	
	~ptr_tracker() {
		for (tracking_ptr_iface * r : active_references_) {
			// TODO: wrap in try-catch block?
			r->origin_died();
		}
	}
	
protected:
	std::set<tracking_ptr_iface *> active_references_;
};

//! Syntactic sugar (for use in "class some_class : public tracked {...}")
typedef ptr_tracker tracked;

#endif

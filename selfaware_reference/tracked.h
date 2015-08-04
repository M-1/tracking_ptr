//
//  ref_tracker.h
//  selfaware_reference
//
//  Created by a on 04/08/15.
//  Copyright (c) 2015 a. All rights reserved.
//

#ifndef __selfaware_reference__ref_tracker__
#define __selfaware_reference__ref_tracker__

#include <iostream>
#include <set>

#include "tracking_ptr_iface.h"

class ptr_tracker {
public:
	void track(tracking_ptr_iface & r){
		std::cout << "tracker: track " << &r << std::endl;
		active_references_.emplace(&r);
	}
	
	void free(tracking_ptr_iface & r){
		std::cout << "tracker: free " << &r << std::endl;
		active_references_.erase(&r);
	}
	
	~ptr_tracker() {
		for (tracking_ptr_iface * r : active_references_) {
			r->origin_died();
		}
	}
	
protected:
	std::set<tracking_ptr_iface *> active_references_;
};

//! Syntactic sugar (for use in "class some_class : public tracked {...}")
typedef ptr_tracker tracked;

#endif /* defined(__selfaware_reference__ref_tracker__) */

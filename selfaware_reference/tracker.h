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

#include "ref_interface.h"

class tracker {
public:
	void track(ref & r) {
		std::cout << "tracker: track " << &r << std::endl;
		active_references_.emplace(&r);
	}
	
	void free(ref & r) {
		std::cout << "tracker: free " << &r << std::endl;
		active_references_.erase(&r);
	}
	
	~tracker() {
		for (ref * r : active_references_) {
			r->origin_died();
		}
	}
	
protected:
	std::set<ref *> active_references_;
};

#endif /* defined(__selfaware_reference__ref_tracker__) */

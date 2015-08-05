//
//  tracked.h
//  tracking_ptr
//

#ifndef __tracking_ptr__tracked_h__
#define __tracking_ptr__tracked_h__

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
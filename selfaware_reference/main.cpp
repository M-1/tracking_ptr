//
//  main.cpp
//  selfaware_reference
//
//  Created by a on 04/08/15.
//  Copyright (c) 2015 a. All rights reserved.
//

#include <iostream>
#include <memory>

#include "sa_ref.h"
#include "tracker.h"

class C : public tracker {
	int i_;
	
public:
	C(int i) : i_(i) {
		std::cout << "C: ctor(" << i_ << ")" << std::endl;
	}
	
	~C() {
		std::cout << "C: dtor" << std::endl;
	}
	
	void print() {
		std::cout << i_ << std::endl;
	}
};

int main(int argc, const char * argv[])
{
	// Create two instances of class C (their lifetime is managed by unique_ptr), name them i1 and i2
	std::unique_ptr<C> i1(new C(1));
	std::unique_ptr<C> i2(new C(2));

	// Create reference to i1, pass it "invalidation function" to be called when the instance gets destructed
	sar<C> r1_1(*i1, [](){std::cout << "! r1_1: i1 invalidated" << std::endl;});
	
	// Use the reference to work with the referenced instance
	r1_1->print();
	
	// Create another refernece to i1
	sar<C> r1_2(*i1, [](){std::cout << "! r1_2: i1 invalidated" << std::endl;});
	
	// Create reference to i2
	sar<C> r2(*i2, [](){std::cout << "! r2: i2 invalidated" << std::endl;});
	
	// Work with the references
	r1_1->print();
	r1_2->print();
	r2->print();
	
	// Let's kill i1
	std::cout << "killing i1" << std::endl;
	i1.reset();
	
	// i1 is now dead, touching references to it throws
	try {
		r1_1->print();
	} catch (const std::exception & e) {
		std::cout << e.what() << std::endl;
	}
	try {
		r1_2->print();
	} catch (const std::exception & e) {
		std::cout << e.what() << std::endl;
	}
	
	// But i2 is still alive, we may still use reference(s) to it
	r2->print();
	
	std::cout << "(end of scope)" << std::endl;
	
	// Local variables will be destructed in the reverse order of construction:
	// ~r2, ~r1_2, ~r1_1, ~i2, ~i1
	
	// Because r2 gets destructed before i2, no invalidation is triggered, only the reference gets freed from its tracker (the object it points to)
	
	// Destruction of r1_2 (and r1_1) skips the references being freed since the tracker does not exist anymore (i1 is already dead)
	
	// Destruction of i2 does not trigger invalidation of references, because all references to it have been freed
	
	// Destruction of i1 does nothing since the unique_ptr is already null
	
	return 0;
}


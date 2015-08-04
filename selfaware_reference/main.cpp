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
	C(int i) : i_(i) {}
	
	void print() {
		std::cout << i_ << std::endl;
	}
};

int main(int argc, const char * argv[])
{
	/*C bla(123);
	
	auto r = sar<C>(bla, [](){std::cout << "inv" << std::endl;});
	
	r->print();
	
	bla.~C();
	*/
	
	
	std::unique_ptr<C> i1(new C(1));
	std::unique_ptr<C> i2(new C(2));

	sar<C> r1_1(*i1, [](){std::cout << "! r1_1: i1 invalidated" << std::endl;});
	sar<C> r1_2(*i1, [](){std::cout << "! r1_2: i1 invalidated" << std::endl;});
	
	sar<C> r2_1(*i2, [](){std::cout << "! r2_1: i2 invalidated" << std::endl;});
	sar<C> r2_2(*i2, [](){std::cout << "! r2_2: i2 invalidated" << std::endl;});
	
	std::cout << "killing i1" << std::endl;

	r1_1->print();
	
	
	std::cout << "killing i1" << std::endl;
	
	i1.reset();
	
	// i1 is now dead, touching references to it would throw
	try {
		r1_2->print();
	} catch (const std::exception & e) {
		std::cout << e.what() << std::endl;
	}
	
	// i2 is still alive, we may use references to it
	
	r2_1->print();

	
	std::cout << "(end of scope)" << std::endl;

	return 0;
}


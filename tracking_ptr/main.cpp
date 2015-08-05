//
//  main.cpp
//  tracking_ptr
//

#include <iostream>
#include <memory>

//#define TRACKING_PTR_ENABLE_LOGGING 1
#include "tracking_ptr.h"
#include "tracked.h"

//! An example class that can have tracking_ptr(s) pointed at it
class C : public tracked {
	int i_;
	
public:
	explicit C(int i) : i_(i) {
		std::cout << "C: ctor(" << i_ << ")" << std::endl;
	}
	
	~C() {
		std::cout << "C: dtor" << std::endl;
	}
	
	void print() {
		std::cout << i_ << std::endl;
	}
};

//! Basic usage (simple creation of tracking pointers and destruction of them vs destruction of their targets)
void basic_example() {
	// Create two instances of class C (their lifetime is managed by unique_ptr), name them i1 and i2
	std::unique_ptr<C> i1(new C(1));
	std::unique_ptr<C> i2(new C(2));

	// Create tracking pointer to i1, pass it "invalidation function" to be called when the instance gets destructed
	tracking_ptr<C> r1_1(*i1, [](){std::cout << "! r1_1: i1 invalidated" << std::endl;});
	
	// Use the pointer to work with the pointed-to instance
	r1_1->print();
	
	// Create another tracking pointer to i1 (the count of tracking pointers is not limited)
	tracking_ptr<C> r1_2(*i1, [](){std::cout << "! r1_2: i1 invalidated" << std::endl;});
	
	// Create a tracking pointer to i2
	tracking_ptr<C> r2(*i2, [](){std::cout << "! r2: i2 invalidated" << std::endl;});
	
	// Work with the pointers "directly"...
	r1_1->print();
	r1_2->print();
	r2->print();
	
	// ... get the raw pointer and work with it instead
	{
		C * cp = r2.get();
		cp->print();
	}
	
	// Let's kill i1
	// While it gets destroyed, it invalidates all tracking pointers pointing at it
	std::cout << "killing i1" << std::endl;
	i1.reset();
	
	// i1 is now dead, touching invalidated tracking pointers throws
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
	
	// But i2 is still alive, we may still use the tracking pointer to it
	r2->print();
	
	std::cout << "(end of scope)" << std::endl;
	
	// Local variables will be destructed in the reverse order of construction:
	// ~r2, ~r1_2, ~r1_1, ~i2, ~i1
	
	// Because r2 gets destructed before i2, no invalidation is triggered, only the tracking_ptr gets freed from its tracker (the object it points at)
	
	// Destruction of r1_2 (and r1_1) skips the pointers being freed since the tracker does not exist anymore (i1 is already dead)
	
	// Destruction of i2 does not trigger invalidation of tracking pointer r2, because it has been freed
	
	// Destruction of i1 does nothing since the unique_ptr is already null
}

//! Copying and moving of tracking pointers
void copy_move_example() {
	// Create an instance of class C, name it "i1"
	std::unique_ptr<C> i1(new C(1));

	// Create tracking pointer to i1, pass it "invalidation function" to be called when the instance gets destructed
	tracking_ptr<C> r1_1(*i1, [](){std::cout << "! i1 invalidated" << std::endl;});
	
	// Copy the tracking pointer (the new copy gets tracked automatically)
	std::cout << "copy" << std::endl;
	tracking_ptr<C> r1_2(r1_1);
	
	// Both the original and the copy are valid and point at i1
	r1_1->print();
	r1_2->print();
	
	// Move the pointer (the original pointer gets "untracked" and the new pointer gets tracked automatically)
	std::cout << "move" << std::endl;
	tracking_ptr<C> r1_3(std::move(r1_1));
	
	// Usage of move source after moving is invalid (throws)
	try {
		r1_1->print();
	} catch (const std::exception & e) {
		std::cout << e.what() << std::endl;
	}
	
	// But the move target is valid instead
	r1_3->print();
	
	// We can also use move-assignment to move the pointer back to r1_1
	std::cout << "move assign" << std::endl;
	r1_1 = std::move(r1_3);
	
	// Or copy-assignment it to copy it to r1_3
	std::cout << "copy assign" << std::endl;
	r1_3 = r1_1;
	
	// When we kill the tracking instance, all three pointers will get invalidated
	i1.reset();
}

int main(int argc, const char * argv[]) {
	std::cout << "--- basic example ---" << std::endl;
	basic_example();
	
	std::cout << "--- copy and move example ---" << std::endl;
	copy_move_example();
	
	return 0;
}

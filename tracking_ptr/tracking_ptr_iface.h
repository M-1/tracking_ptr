//
//  tracking_ptr_iface.h
//  tracking_ptr
//

#ifndef __tracking_ptr__tracking_ptr_iface_h__
#define __tracking_ptr__tracking_ptr_iface_h__

#include <iostream>

class tracking_ptr_iface {
public:
	virtual void origin_died() = 0;
	virtual ~tracking_ptr_iface() {}
};

#endif

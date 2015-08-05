//
//  tracking_ptr_iface.h
//  tracking_ptr
//  Copyright (c) 2015, Martin Mi, released under BSD 3-Clause License (see LICENSE file for details)
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

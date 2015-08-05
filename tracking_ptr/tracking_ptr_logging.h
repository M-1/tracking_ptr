//
//  tracking_ptr_logging.h
//  tracking_ptr
//

#ifndef __tracking_ptr__tracking_ptr_logging_h__
#define __tracking_ptr__tracking_ptr_logging_h__

#ifdef TRACKING_PTR_ENABLE_LOGGING
// Log to std::clog
#define TRACKING_PTR_LOG(x) std::clog << x << std::endl;
#else
// Do not log
#define TRACKING_PTR_LOG(x)
#endif

#endif

#ifndef _NAT_UTILS_H_
#define _NAT_UTILS_H_

#include "./NatDataDef.h"

extern void LogNatData(const NatData &data);

extern void LogIpPort(const sockaddr_in &addr);

extern int ParseNatData(const NatData &data);

#endif

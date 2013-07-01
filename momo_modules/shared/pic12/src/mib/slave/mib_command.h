#ifndef __mib_command_h__
#define __mib_command_h__

#include "protocol.h"

enum
{
	kMIBCreateParameters = 1,
	kMIBExecuteCommand = 2
};

#define extract_param_type(params, n) ((params & (1<<(n+3))) >> (n+3))
#define extract_param_count(params) (params & 0b111)

//Callback Type
typedef void* (*mib_callback)(MIBParamList *);

unsigned char			find_handler(unsigned char feature, unsigned char cmd);
mib_callback			get_handler(unsigned char handler_index);
MIBParamList *			build_params(unsigned char handler_index);

#endif
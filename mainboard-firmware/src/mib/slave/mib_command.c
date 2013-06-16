//mib_command.c 

#include "mib_command.h"
#include "bus_slave.h"
#include <string.h>

#define kNumFeatures 1
#define kNumCommands 2
const unsigned char features[kNumFeatures] = {2};
const unsigned char commands[kNumFeatures+1] = {0,kNumCommands};
const mib_callback  handlers[kNumCommands] = {echo_buffer, test_command};

extern volatile unsigned char 	mib_buffer[kBusMaxMessageSize];

mib_callback find_handler(unsigned char feature, unsigned char cmd)
{
	unsigned int i, num_cmds;
	int found_feat = -1;

	for (i=0; i<kNumFeatures; ++i)
	{
		if (features[i] == feature)
		{
			found_feat = i;
			break;
		}
	}

	if (found_feat == -1)
		return kInvalidMIBHandler;

	num_cmds = commands[found_feat+1] - commands[found_feat];

	if (cmd >= num_cmds)
		return kInvalidMIBHandler;

	return handlers[commands[found_feat] + cmd];
}

void* test_command(int state, void *param)
{
	if (state == kMIBCreateParameters)
	{
		volatile MIBParamList *list = bus_allocate_param_list(2);
		list->params[0] = (MIBParameterHeader*)bus_allocate_int_param();
		list->params[1] = (MIBParameterHeader*)bus_allocate_int_param();

		return (void*)list;
	}
	else
	{
		_RA1 = !_RA1;
		
		MIBIntParameter *retval;

		bus_free_all();

		retval = (MIBIntParameter*)bus_allocate_int_param();

		bus_init_int_param(retval, 6);
		bus_slave_setreturn(kNoMIBError, (MIBParameterHeader*)retval);
	}

	return 0;
}

void* echo_buffer(int state, void *param)
{
	if (state == kMIBCreateParameters)
	{
		volatile MIBParamList *list = bus_allocate_param_list(1);
		list->params[0] = (MIBParameterHeader*)bus_allocate_buffer_param(20);

		return (void*)list;
	}
	else
	{	
		MIBParamList *list = (MIBParamList*)param;
		MIBBufferParameter *buf = (MIBBufferParameter *)list->params[0];
		bus_free_all();

		memmove((void*)mib_buffer, buf, 2);
		memmove((void*)mib_buffer+2, buf->data, buf->header.len);

		bus_slave_setreturn(kNoMIBError, (MIBParameterHeader*)mib_buffer);
	}

	return 0;
}
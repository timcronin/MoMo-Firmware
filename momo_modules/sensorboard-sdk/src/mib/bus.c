//bus.c

#include "bus_master.h"
#include "bus_slave.h"
//#include "task_manager.h"
#include <string.h>

volatile MIBState 		mib_state;
volatile unsigned char 	mib_buffer[kBusMaxMessageSize];
unsigned int 			mib_firstfree;

void bus_init()
{
	mib_firstfree = 0;
	i2c_enable(kControllerPICAddress);
}

int bus_send(unsigned char address, unsigned char *buffer, unsigned char len, unsigned char flags)
{
	if (len > kBusMaxMessageSize)
		return -1;

	//Fill in message contents
	mib_state.bus_msg.address = address;
	mib_state.bus_msg.data_ptr = buffer;
	mib_state.bus_msg.last_data = buffer + len;
	mib_state.bus_msg.flags = flags;

	return i2c_send_message();
}

int bus_receive(unsigned char address, volatile unsigned char *buffer, unsigned char len, unsigned char flags)
{
	mib_state.bus_msg.address = address;
	mib_state.bus_msg.data_ptr = buffer;
	
	mib_state.bus_msg.last_data = buffer;
	mib_state.bus_msg.len = len;

	mib_state.bus_msg.flags = flags;

	return i2c_receive_message();
}

volatile unsigned char *bus_allocate_space(unsigned int len)
{
	volatile unsigned char *allocated; 

	if ((kBusMaxMessageSize - mib_firstfree) < len)
		return (unsigned char *)0;

	allocated = &mib_buffer[mib_firstfree];
	
	mib_firstfree += len;

	return allocated;
}

void bus_free_all()
{
	mib_firstfree = 0;
}

void bus_init_int_param(MIBIntParameter *param, int value)
{
	param->header.type = kMIBInt16Type;
	param->header.len = 2;
	param->value = value;
}

void bus_init_buffer_param(MIBBufferParameter *param, void *data, unsigned char len)
{
	param->header.type = kMIBBufferType;
	param->header.len = len;
	param->data = data;
}

MIBParameterHeader *bus_allocate_return_buffer(unsigned char **out_buffer)
{
	unsigned char len = kBusMaxMessageSize - mib_firstfree;
	MIBParameterHeader *ret = (MIBParameterHeader *)bus_allocate_space(len);

	ret->type = kMIBBufferType;
	ret->len = len - sizeof(MIBParameterHeader);

	if (out_buffer != NULL)
		*out_buffer = ((unsigned char*)ret) + sizeof(MIBParameterHeader);

	return ret;
}

volatile MIBIntParameter *bus_allocate_int_param()
{
	volatile MIBIntParameter *param = (volatile MIBIntParameter *)bus_allocate_space(sizeof(MIBIntParameter));

	if (param == 0)
		return 0;

	param->header.type = kMIBInt16Type;
	param->header.len = sizeof(MIBIntParameter) - sizeof(MIBParameterHeader);

	return param;
}

volatile MIBBufferParameter *bus_allocate_buffer_param(unsigned int len)
{
	volatile MIBBufferParameter *param;

	//If len == 0, allocate all remaining space
	if (len == 0)
		len = kBusMaxMessageSize - mib_firstfree - sizeof(MIBBufferParameter);

	param = (volatile MIBBufferParameter *)bus_allocate_space(sizeof(MIBBufferParameter)+len);

	if (param == 0)
		return 0;

	param->header.type = kMIBBufferType;
	param->header.len = len;
	param->data = ((unsigned char*)param) + sizeof(MIBBufferParameter);

	return param;
}

volatile MIBParamList *bus_allocate_param_list(unsigned int num)
{
	volatile MIBParamList *list = (volatile MIBParamList*)bus_allocate_space(sizeof(MIBParamList) + sizeof(MIBParameterHeader*)*num);

	if (list == 0)
		return 0;

	list->count = num;
	list->curr = 0;

	return list;
}

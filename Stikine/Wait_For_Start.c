/*
 * Wait_For_Start.c
 *
 *  Created on: Nov 23, 2015
 *      Author: cgoss
 */

#include <stdint.h>
#include "States.h"
#include "Radio.h"
#include "pindefs.h"

Machine_State Wait_For_Start()
{
	struct Listen_Struct message_status;
	uint8_t Got_Ok = False;
	while(!Got_Ok)
	{
		message_status = LBT_Listen(65535); // Listen for 2 seconds
		if(message_status.Status == Message_Recieved)
		{
			if(message_status.payload[0] == 0xFF)
			{
				Got_Ok = True;
			}
		}
	}

	return Localizing;
}

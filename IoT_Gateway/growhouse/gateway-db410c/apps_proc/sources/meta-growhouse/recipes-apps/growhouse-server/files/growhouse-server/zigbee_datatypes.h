/*
 * zigbee_datatypes.h: Zigbee attributes data types.
 * Copyright (c) 2018
 *
 */
#ifndef __ZIGBEE_DATATYPES_H__
#define __ZIGBEE_DATATYPES_H__

#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>

typedef struct ZCLDataTypes {
	bool isSigned;
	int numBytes;
	char dataFormat;
}ZCLDATATYPE;

ZCLDATATYPE ZCLDataTypesArray[] = {

	{ 0, 0 },	//ZCL_NO_DATA_ATTRIBUTE_TYPE
	{ }, { }, { }, { }, { }, { }, { },
	{ false , 1 , 'f'},	// 8 ZCL_DATA8_ATTRIBUTE_TYPE
	{ false , 2 , 'f'},	// 9 ZCL_DATA16_ATTRIBUTE_TYPE
	{ false , 3 , 'f'},
	{ false , 4 , 'f'},
	{ false , 5 , 'f'},
	{ false , 6 , 'f'},
	{ false , 7 , 'f'},
	{ false , 8 , 'f'},
	{ false , 1 , 'b'},	// 16 ZCL_BOOLEAN_ATTRIBUTE_TYPE
	{ }, { }, { }, { }, { }, { }, { },
	{ false , 1},	// 24 ZCL_BITMAP8_ATTRIBUTE_TYPE
	{ false , 2},
	{ false , 3},
	{ false , 4},
	{ false , 5},
	{ false , 6},
	{ false , 7},
	{ false , 8},
	{ false , 1 , 'f'},	// 32 ZCL_INT8U_ATTRIBUTE_TYPE
	{ false , 2 , 'f'},
	{ false , 3 , 'i'},
	{ false , 4 , 'i'},
	{ false , 5 , 'i'},
	{ false , 6 , 'i'},
	{ false , 7 , 'i'},
	{ false , 8 , 'i'},	// 39 ZCL_INT64U_ATTRIBUTE_TYPE
	{ true , 1 , 'f' },	//40 ZCL_INT8S_ATTRIBUTE_TYPE
	{ true , 2 , 'f' },
	{ true , 3 , 'f' },
	{ true , 4 , 'i' },
	{ true , 5 , 'i' },
	{ true , 6 , 'i' },
	{ true , 7 , 'i' },
	{ true , 8 , 'i' },
	{ false , 1},	// 48 ZCL_ENUM8_ATTRIBUTE_TYPE
	{ false , 2},
	{ }, { }, { }, { }, { }, { }, { },
	{ false, 4, 'f'},
	{ }, { }, { }, { }, { }, { }, { }, { }, { }, { }, { }, { }, { },
	{ }, { }, { }, { }, { }, { }, { }, { }, { }, { }, { }, { }, { }, { },
	{ }, { }, { }, { }, { }, { }, { }, { }, { }, { }, { }, { }, { }, { },
	{ }, { }, { }, { }, { }, { }, { }, { }, { }, { }, { }, { }, { }, { },
	{ }, { }, { }, { }, { }, { }, { }, { }, { }, { }, { }, { }, { }, { },
	{ }, { }, { }, { }, { }, { }, { }, { }, { }, { }, { }, { }, { }, { },
	{ }, { }, { }, { }, { }, { }, { }, { }, { }, { }, { }, { }, { }, { },
	{ }, { }, { }, { }, { }, { }, { }, { }, { }, { }, { }, { }, { }, { },
	{ }, { }, { }, { }, { }, { }, { }, { }, { }, { }, { }, { }, { }, { },
	{ }, { }, { }, { }, { }, { }, { }, { }, { }, { }, { }, { }, { }, { },
	{ }, { }, { }, { }, { }, { }, { }, { }, { }, { }, { }, { }, { }, { },
	{ }, { }, { }, { }, { }, { }, { }, { }, { }, { }, { }, { }, { }, { },
	{ }, { }, { }, { }, { }, { }, { },
	{ false , 2},
	{ false , 2},
	{ }, { },
	{ }, { }, { }, { }, { }, { }, { }, { }, { }, { }, { }, { }, { }, { },
	{ }, { }, { }
};

#endif // __ZIGBEE_DATATYPES_H__
/*
 * End zigbee_datatypes.h
 */

#pragma once
#ifndef _COLLISION_LAYER_H
#define _COLLISION_LAYER_H
enum eColLayer : unsigned
{
	LAYER_NONE = 0x00u,
	LAYER_1 = 0x01u << 0,
	LAYER_2 = 0x01u << 1,
	LAYER_3 = 0x01u << 2,
	LAYER_4 = 0x01u << 3,
	LAYER_5 = 0x01u << 4,
	LAYER_6 = 0x01u << 5,
	LAYER_7 = 0x01u << 6,
	LAYER_8 = 0x01u << 7,
	LAYER_9 = 0x01u << 8,
	LAYER_10 = 0x01u << 9,
	LAYER_11 = 0x01u << 10,
	LAYER_12 = 0x01u << 11,
	LAYER_13 = 0x01u << 12,
	LAYER_14 = 0x01u << 13,
	LAYER_15 = 0x01u << 14,
	LAYER_16 = 0x01u << 15,
	LAYER_17 = 0x01u << 16,
	LAYER_18 = 0x01u << 17,
	LAYER_19 = 0x01u << 18,
	LAYER_20 = 0x01u << 19,
	LAYER_21 = 0x01u << 20,
	LAYER_22 = 0x01u << 21,
	LAYER_23 = 0x01u << 22,
	LAYER_24 = 0x01u << 23,
	LAYER_25 = 0x01u << 24,
	LAYER_26 = 0x01u << 25,
	LAYER_27 = 0x01u << 26,
	LAYER_28 = 0x01u << 27,
	LAYER_29 = 0x01u << 28,
	LAYER_30 = 0x01u << 29,
	LAYER_31 = 0x01u << 30,
	LAYER_32 = 0x01u << 31,
	LAYER_GLOBAL = 0xFFFFFFFFu,
};

#endif


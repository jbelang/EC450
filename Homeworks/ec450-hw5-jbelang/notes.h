/*
 * notes.h
 * The timing and note sequence for Joy to the World and Sandstorm are included in here,
 * as well as the definitions of each note's frequency, and the number of ticks needed to
 * count to a quarter note (defined as a half second, for a normal BPM of 120)
 *  Created on: Mar 21, 2015
 *  Author: Jesse Belanger
 */

#ifndef NOTES_H_
#define NOTES_H_

#define C6_60 0x0777
#define D_62 0x06A6
#define E_64 0x05ED
#define F_65 0x0597
#define G_67 0x04FE
#define A_69 0x0470
#define B_71 0x03F4
#define C7_72  0x03BB

#define quarter 61

const int joyLength = 73;
const double joyTiming[73] = {1, 0.75, 0.25, 1.5, 0.5, 1, 1, 1.5, 0.5,
		1.5, 0.0625, 0.5, 1.5, 0.0625, 0.5, 1, 0.0625, 1, 0.0625, 0.5, 0.5, 0.5, 0.5, 0.0625, 0.75, 0.25, 0.5, 0.5, 0.0625,
		0.5, 0.5, 0.5, 0.5, 0.0625, 0.75, 0.25, 0.5, 0.0625, 0.5, 0.0625, 0.5, 0.0625, 0.5, 0.0625, 0.5, 0.0625, 0.25, 0.25, 1.5, 0.25, 0.25, 0.5, 0.0625, 0.5, 0.0625, 0.5, 0.0625, 0.25, 0.25,
		1.5, 0.25, 0.25, 0.5, 1, 0.5, 0.75, 0.25, 0.5, 0.5, 1, 1, 2, 0};

const int joyNotes[73] = {C7_72, B_71, A_69, G_67, F_65, E_64, D_62, C6_60, G_67,
		A_69, 0, A_69, B_71, 0, B_71, C7_72, 0, C7_72, 0, C7_72, B_71, A_69, G_67, 0, G_67, F_65, E_64, C7_72, 0,
		C7_72, B_71, A_69, G_67, 0, G_67, F_65, E_64, 0, E_64, 0, E_64, 0, E_64, 0, E_64, 0, E_64, F_65, G_67, F_65, E_64, D_62, 0, D_62, 0, D_62, 0, D_62, E_64,
		F_65, E_64, D_62, E_64, C7_72, A_69, G_67, F_65, E_64, F_65, E_64, D_62, C6_60, 0};

const int sandstormLength = 232;

const double sandstormTiming[232] = {0.75, 0.0625, 0.25, 0.0625, 0.5, 0.0625, 0.5, 0.0625, 0.75, 0.0625, 0.25, 0.0625, 0.5, 0.0625, 0.5, 0.0625,
		0.75, 0.0625, 0.25, 0.0625, 0.5, 0.0625, 0.5, 0.0625, 0.75, 0.0625, 0.25, 0.0625, 0.5, 0.0625, 0.5, 0.0625,
		0.75, 0.0625, 0.25, 0.0625, 0.5, 0.0625, 0.5, 0.0625, 0.75, 0.0625, 0.25, 0.0625, 0.5, 0.0625, 0.5, 0.0625,
		0.75, 0.0625, 0.25, 0.0625, 0.5, 0.0625, 0.5, 0.0625, 0.75, 0.0625, 0.25, 0.0625, 0.5, 0.0625, 0.5, 0.0625,
		0.75, 0.0625, 0.25, 0.0625, 0.5, 0.0625, 0.5, 0.0625, 0.75, 0.0625, 0.25, 0.0625, 0.5, 0.0625, 0.5, 0.0625,
		0.75, 0.0625, 0.25, 0.0625, 0.5, 0.0625, 0.5, 0.0625, 0.75, 0.0625, 0.25, 0.0625, 0.5, 0.0625, 0.5, 0.0625,
		0.75, 0.0625, 0.25, 0.0625, 0.5, 0.0625, 0.5, 0.0625, 0.75, 0.0625, 0.25, 0.0625, 0.5, 0.0625, 0.5, 0.0625,
		0.75, 0.0625, 0.25, 0.0625, 0.5, 0.0625, 0.5, 0.0625, 0.75, 0.0625, 0.25, 0.0625, 0.5, 0.0625, 0.5, 0.0625,
		0.25, 0.0625, 0.25, 0.0625, 0.25, 0.0625, 0.25, 0.0625, 0.25, 0.0625, 0.25, 0.0625, 0.5, 0.0625, 0.75, 0.0625, 0.25, 0.0625, 0.5, 0.0625, 0.5, 0.0625,
		0.75, 0.0625, 0.25, 0.0625, 0.5, 0.0625, 0.5, 0.0625, 0.75, 0.0625, 0.25, 0.0625, 0.5, 0.0625, 0.5, 0.0625,
		0.25, 0.0625, 0.25, 0.0625, 0.25, 0.0625, 0.25, 0.0625, 0.25, 0.0625, 0.25, 0.0625, 0.5, 0.0625, 0.75, 0.0625, 0.25, 0.0625, 0.5, 0.0625, 0.5, 0.0625,
		0.75, 0.0625, 0.25, 0.0625, 0.5, 0.0625, 0.5, 0.0625, 0.75, 0.0625, 0.25, 0.0625, 0.5, 0.0625, 0.5, 0.0625,
		0.25, 0.0625, 0.25, 0.0625, 0.25, 0.0625, 0.25, 0.0625, 0.25, 0.0625, 0.25, 0.0625, 0.5, 0.0625, 0.25, 0.0625, 0.25, 0.0625, 0.25, 0.0625, 0.25, 0.0625, 0.25, 0.0625, 0.25, 0.0625, 0.5, 0.0625};

const int sandstormNotes[232] = {G_67, 0, G_67, 0, G_67, 0, G_67, 0, G_67, 0, G_67, 0, G_67, 0, C6_60, 0,
		C6_60, 0, C6_60, 0, C6_60, 0, B_71, 0, B_71, 0, B_71, 0, B_71, 0, F_65, 0,
		G_67, 0, G_67, 0, G_67, 0, G_67, 0, G_67, 0, G_67, 0, G_67, 0, C6_60, 0,
		G_67, 0, G_67, 0, G_67, 0, G_67, 0, G_67, 0, G_67, 0, G_67, 0, C6_60, 0,
		G_67, 0, G_67, 0, G_67, 0, G_67, 0, G_67, 0, G_67, 0, G_67, 0, C6_60, 0,
		C6_60, 0, C6_60, 0, C6_60, 0, B_71, 0, B_71, 0, B_71, 0, B_71, 0, F_65, 0,
		G_67, 0, G_67, 0, G_67, 0, G_67, 0, G_67, 0, G_67, 0, G_67, 0, C6_60, 0,
		G_67, 0, G_67, 0, G_67, 0, G_67, 0, G_67, 0, G_67, 0, G_67, 0, C6_60, 0,
		B_71, 0, B_71, 0, B_71, 0, B_71, 0, B_71, 0, 0, 0, 0, 0, G_67, 0, G_67, 0, G_67, 0, C6_60, 0,
		C6_60, 0, C6_60, 0, C6_60, 0, B_71, 0, B_71, 0, B_71, 0, B_71, 0, F_65, 0,
		B_71, 0, B_71, 0, B_71, 0, B_71, 0, B_71, 0, 0, 0, 0, 0, G_67, 0, G_67, 0, G_67, 0, C6_60, 0,
		G_67, 0, G_67, 0, G_67, 0, G_67, 0, G_67, 0, G_67, 0, G_67, 0, C6_60, 0,
		B_71, 0, B_71, 0, B_71, 0, B_71, 0, B_71, 0, 0, 0, 0, 0, B_71, 0, B_71, 0, B_71, 0, B_71, 0, B_71, 0, 0, 0, 0, 0};

#endif /* NOTES_H_ */

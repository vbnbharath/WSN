/*
 * States.h
 *
 *  Created on: Nov 23, 2015
 *      Author: cgoss
 */

/**
 * \file States.h
 * \brief The contains the enumeration of possible states for the main control loop.
 */

#ifndef STATES_H_
#define STATES_H_

typedef enum {
	Waiting_For_Start, ///< Startup state, waiting for the signal to begin localization
	Localizing,        ///< Localizing state performs periodic pings so that the localization beacon can hear them
	Clustering,        ///< Begins the time clustering process
	CH_TDMA_Assignment,///< Cluster head state in which it assignes TDMA slots to child nodes
	CH_Sensing,        ///< Cluster head state, normal mode of operation
	TDMA_Assignment,   ///< Child node state, waiting for TDMA assignment from cluster head
	Sensing            ///< Normal operation mode for child nodes.
} Machine_State;

#endif /* STATES_H_ */

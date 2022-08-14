/*
 * Panels.h
 *
 *      Author: Eric Mintz
 *
 * Different panel geometries
 */

#ifndef PANELS_H_
#define PANELS_H_

#include "Panel.h"

class Panels {
public:

	/**
	 * Returns a Panel implementation for a panel whose LED string zigzags
	 * up and down.  See
	 */
	static const Panel* alternatingEightByThirtyTwo();

private:
	Panels(){};

};

#endif /* PANELS_H_ */

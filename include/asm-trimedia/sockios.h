/* 
 * Copyright (C) 2009 Gulessoft , Inc. 
 * Written by Guo Hongruan (guo.hongruan@gulessoft.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef _ASM_TRIMEDIA_SOCKIOS_H
#define _ASM_TRIMEDIA_SOCKIOS_H

/* Socket-level I/O control calls. */
#define FIOSETOWN 	0x8901
#define SIOCSPGRP	0x8902
#define FIOGETOWN	0x8903
#define SIOCGPGRP	0x8904
#define SIOCATMARK	0x8905
#define SIOCGSTAMP	0x8906		/* Get stamp */

#endif /* _ASM_TRIMEDIA_SOCKIOS_H */

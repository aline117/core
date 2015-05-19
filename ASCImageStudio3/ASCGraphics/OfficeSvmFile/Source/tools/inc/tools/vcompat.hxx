/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: vcompat.hxx,v $
 * $Revision: 1.3 $
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/
#ifndef _VCOMPAT_HXX
#define _VCOMPAT_HXX
#ifdef AVS
#include "tools/toolsdllapi.h"
#include <tools/solar.h>
#endif
#include "OpenOfficeBasic.h"

namespace SVMCore{
// -----------
// - Defines -
// -----------

#define COMPAT_FORMAT( char1, char2, char3, char4 ) \
	((UINT32)((((UINT32)(char)(char1)))|				\
	(((UINT32)(char)(char2))<<8UL)|					\
	(((UINT32)(char)(char3))<<16UL)|					\
	((UINT32)(char)(char4))<<24UL))

// --------------
// - ImplCompat -
// --------------

class SvStream;

class VersionCompat
{
    SvStream*		mpRWStm;
    UINT32			mnCompatPos;
    UINT32			mnTotalSize;
    UINT16			mnStmMode;
    UINT16			mnVersion;

					VersionCompat() {}
					VersionCompat( const VersionCompat& ) {}
    VersionCompat&	operator=( const VersionCompat& ) { return *this; }
    BOOL			operator==( const VersionCompat& ) { return FALSE; }

public:

					VersionCompat( SvStream& rStm, USHORT nStreamMode, UINT16 nVersion = 1 );
					~VersionCompat();

    UINT16			GetVersion() const { return mnVersion; }
};
}//SVMCore
#endif // _VCOMPAT_HXX

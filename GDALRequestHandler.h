
// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of nc_handler, a data handler for the OPeNDAP data
// server. 

// This file is part of the GDAL OPeNDAP Adapter

// Copyright (c) 2004 OPeNDAP, Inc.
// Author: Frank Warmerdam <warmerdam@pobox.com>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

 
// CDFRequestHandler.h

#ifndef I_GDALRequestHandler_H
#define I_GDALRequestHandler_H 1

#include <BESRequestHandler.h>

class GDALRequestHandler : public BESRequestHandler {
private:
    static bool		_show_shared_dims ;
    static bool		_show_shared_dims_set ;

    static bool     _ignore_unknown_types ;
    static bool     _ignore_unknown_types_set ;

public:
	GDALRequestHandler( const string &name ) ;
    virtual	~GDALRequestHandler( void ) ;

    static bool	gdal_build_das( BESDataHandlerInterface &dhi ) ;
    static bool	gdal_build_dds( BESDataHandlerInterface &dhi ) ;
    static bool	gdal_build_data( BESDataHandlerInterface &dhi ) ;
    static bool	gdal_build_help( BESDataHandlerInterface &dhi ) ;
    static bool	gdal_build_version( BESDataHandlerInterface &dhi ) ;

    static bool get_show_shared_dims() { return _show_shared_dims; }
    static bool get_ignore_unknown_types() { return _ignore_unknown_types; }

};

#endif


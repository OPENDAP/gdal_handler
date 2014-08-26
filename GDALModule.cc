// GDALModule.cc

// This file is part of bes, A C++ back-end server implementation framework
// for the OPeNDAP Data Access Protocol.

// This file is part of the GDAL OPeNDAP Adapter

// Copyright (c) 2004 OPeNDAP, Igdal.
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
// Foundation, Igdal., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
// You can contact OPeNDAP, Igdal. at PO Box 112, Saunderstown, RI. 02874-0112.

#include "config.h"

#include <iostream>

using std::endl;

#include "GDALModule.h"

#include <BESRequestHandlerList.h>
#include "GDALRequestHandler.h"
#include <BESDapService.h>
#include <BESContainerStorageList.h>
#include <BESContainerStorageCatalog.h>
#include <BESCatalogDirectory.h>
#include <BESCatalogList.h>
#include <BESDebug.h>

#define GDAL_CATALOG "catalog"

void GDALModule::initialize(const string & modname)
{
    BESDEBUG("gdal", "Initializing GDAL module " << modname << endl);
    BESDEBUG("gdal", "    adding " << modname << " request handler" << endl);

    BESRequestHandler *handler = new GDALRequestHandler(modname);
    BESRequestHandlerList::TheList()->add_handler(modname, handler);

    BESDEBUG("gdal", modname << " handles dap services" << endl);

    BESDapService::handle_dap_service(modname);

    BESDEBUG("gdal", "    adding " << GDAL_CATALOG << " catalog" << endl);

    if (!BESCatalogList::TheCatalogList()->ref_catalog(GDAL_CATALOG)) {
        BESCatalogList::TheCatalogList()-> add_catalog(new BESCatalogDirectory(GDAL_CATALOG));
    }
    else {
        BESDEBUG("gdal", "    catalog already exists, skipping" << endl);
    }

    BESDEBUG("gdal", "    adding catalog container storage " << GDAL_CATALOG << endl);

    if (!BESContainerStorageList::TheList()->ref_persistence(GDAL_CATALOG)) {
        BESContainerStorageCatalog *csc = new BESContainerStorageCatalog(GDAL_CATALOG);
        BESContainerStorageList::TheList()->add_persistence(csc);
    }
    else {
        BESDEBUG("gdal", "    storage already exists, skipping" << endl);
    }

    BESDEBUG("gdal", "    adding gdal debug context" << endl);
    BESDebug::Register("gdal");

    BESDEBUG("gdal", "Done Initializing GDAL module " << modname << endl);
}

void GDALModule::terminate(const string & modname)
{
    BESDEBUG("gdal", "Cleaning GDAL module " << modname << endl);

    BESDEBUG("gdal", "    removing GDAL Handler" << modname << endl);
    BESRequestHandler *rh = BESRequestHandlerList::TheList()->remove_handler(modname);
    if (rh)
        delete rh;

    BESDEBUG("gdal", "    removing catalog container storage" << GDAL_CATALOG << endl);
    BESContainerStorageList::TheList()->deref_persistence(GDAL_CATALOG);

    BESDEBUG("gdal", "    removing " << GDAL_CATALOG << " catalog" << endl);
    BESCatalogList::TheCatalogList()->deref_catalog(GDAL_CATALOG);

    BESDEBUG("gdal", "Done Cleaning GDAL module " << modname << endl);
}

/** @brief dumps information about this object
 *
 * Displays the pointer value of this instance
 *
 * @param strm C++ i/o stream to dump the information to
 */
void GDALModule::dump(ostream & strm) const
{
    strm << BESIndent::LMarg << "GDALModule::dump - (" << (void *) this << ")" << endl;
}

extern "C" BESAbstractModule * maker()
{
    return new GDALModule;
}


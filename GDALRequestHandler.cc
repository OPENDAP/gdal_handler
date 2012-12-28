// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of gdal_handler, a data handler for the OPeNDAP data
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


// GDALRequestHandler.cc

#include "config.h"

#include <string>
#include <sstream>

#include "GDAL_DDS.h"
#include "GDALRequestHandler.h"

#include <BESResponseHandler.h>
#include <BESResponseNames.h>
#include <BESDapNames.h>
#include <BESDASResponse.h>
#include <BESDDSResponse.h>
#include <BESDataDDSResponse.h>
#include <BESVersionInfo.h>
#include <BESDapError.h>
#include <BESInternalFatalError.h>
#include <BESDataNames.h>
#include <TheBESKeys.h>
#include <BESUtil.h>

#include <BESServiceRegistry.h>
// #include <BESUtil.h>
#include <BESContextManager.h>

#include <InternalErr.h>
#include <Ancillary.h>
#include <mime_util.h>
#include <debug.h>

#define GDAL_NAME "gdal"

using namespace libdap;

extern void gdal_read_dataset_attributes(DAS & das, const string & filename);
extern GDALDatasetH gdal_read_dataset_variables(DDS *dds, const string & filename);

GDALRequestHandler::GDALRequestHandler(const string &name) :
    BESRequestHandler(name)
{
    add_handler(DAS_RESPONSE, GDALRequestHandler::gdal_build_das);
    add_handler(DDS_RESPONSE, GDALRequestHandler::gdal_build_dds);
    add_handler(DATA_RESPONSE, GDALRequestHandler::gdal_build_data);
    add_handler(HELP_RESPONSE, GDALRequestHandler::gdal_build_help);
    add_handler(VERS_RESPONSE, GDALRequestHandler::gdal_build_version);

#if 0
    // See comments in the header about these... jhrg
    if (GDALRequestHandler::_show_shared_dims_set == false) {
        bool key_found = false, context_found = false;
        // string key = "GDAL.ShowSharedDimensions";
        string doset;
        TheBESKeys::TheKeys()->get_value("GDAL.ShowSharedDimensions", doset, key_found);
        // TODO Fix this so it works
        string context_value = BESContextManager::TheManager()->get_context("xdap_accept", context_found);
        //cerr << "context value: " << context_value << endl;
        //cerr << "Testing values..." << endl;
        if (key_found) {
            //cerr << " Key found" << endl;
            doset = BESUtil::lowercase(doset);
            if (doset == "true" || doset == "yes") {
                GDALRequestHandler::_show_shared_dims = true;
            }
        }
        else if (context_found) {
            //cerr << "context found" << endl;
            if (version_ge(context_value, 3.2))
                GDALRequestHandler::_show_shared_dims = false;
            else
                GDALRequestHandler::_show_shared_dims = true;
        }
        else {
            //cerr << "Set default value" << endl;
            GDALRequestHandler::_show_shared_dims = true;
        }

        GDALRequestHandler::_show_shared_dims_set = true;
    }

    if (GDALRequestHandler::_ignore_unknown_types_set == false) {
        bool key_found = false;
        string doset;
        TheBESKeys::TheKeys()->get_value("GDAL.IgnoreUnknownTypes", doset, key_found);
        if (key_found) {
            doset = BESUtil::lowercase(doset);
            if (doset == "true" || doset == "yes")
                GDALRequestHandler::_ignore_unknown_types = true;
            else
                GDALRequestHandler::_ignore_unknown_types = false;
        }
        else {
            // if the key is not found, set the default value
            GDALRequestHandler::_ignore_unknown_types = false;
        }

        GDALRequestHandler::_ignore_unknown_types_set = true;
    }
#endif
}

GDALRequestHandler::~GDALRequestHandler()
{
}

/** If there appears to be a pathname in the error message, remove all of the
 * text except for the filename. There is a util function in libdap
 * for this, but it only knows how to chop off leading characters.
 * In this case the pathname is embedded in an error message that
 * should be preserved.
 */
static string sanitize(string msg)
{
    string::size_type first = msg.find("/");
    string::size_type last = msg.find_last_of("/");
    if (first == string::npos || last == string::npos)
        return msg;
    else
        return msg.erase(first, last-first+1);
}

bool GDALRequestHandler::gdal_build_das(BESDataHandlerInterface & dhi)
{
    BESResponseObject *response = dhi.response_handler->get_response_object();
    BESDASResponse *bdas = dynamic_cast<BESDASResponse *> (response);
    if (!bdas)
        throw BESInternalError("cast error", __FILE__, __LINE__);
    try {
        bdas->set_container(dhi.container->get_symbolic_name());
        DAS *das = bdas->get_das();
        string accessed = dhi.container->access();
        gdal_read_dataset_attributes(*das, accessed);
        Ancillary::read_ancillary_das(*das, accessed);
        bdas->clear_container();
    }
    catch (BESError &e) {
        throw;
    }
    catch (InternalErr & e) {
        throw BESDapError(sanitize(e.get_error_message()), true, e.get_error_code(), __FILE__, __LINE__);
    }
    catch (Error & e) {
        throw BESDapError(sanitize(e.get_error_message()), false, e.get_error_code(), __FILE__, __LINE__);
    }
    catch (...) {
        throw BESInternalFatalError("unknown exception caught building DAS", __FILE__, __LINE__);
    }

    return true;
}

bool GDALRequestHandler::gdal_build_dds(BESDataHandlerInterface & dhi)
{
    DBG(cerr << "In GDALRequestHandler::gdal_build_dds" << endl);

    BESResponseObject *response = dhi.response_handler->get_response_object();
    BESDDSResponse *bdds = dynamic_cast<BESDDSResponse *> (response);
    if (!bdds)
        throw BESInternalError("cast error", __FILE__, __LINE__);

    try {
        bdds->set_container(dhi.container->get_symbolic_name());
        DDS *dds = bdds->get_dds();

        string filename = dhi.container->access();
        dds->filename(filename);
        dds->set_dataset_name(name_path(filename));

        // Here the handler does not need the open dataset handle, so
        // it closes it right away.
        GDALDatasetH hDS = gdal_read_dataset_variables(dds, filename);
        GDALClose(hDS);

        DAS *das = new DAS;
        BESDASResponse bdas(das);
        bdas.set_container(dhi.container->get_symbolic_name());
        gdal_read_dataset_attributes(*das, filename);
        Ancillary::read_ancillary_das(*das, filename);

        dds->transfer_attributes(das);
        bdds->set_constraint(dhi);

        bdds->clear_container();
    }
    catch (BESError &e) {
        throw;
    }
    catch (InternalErr & e) {
        throw BESDapError(sanitize(e.get_error_message()), true, e.get_error_code(), __FILE__, __LINE__);
    }
    catch (Error & e) {
        throw BESDapError(sanitize(e.get_error_message()), false, e.get_error_code(), __FILE__, __LINE__);
    }
    catch (...) {
        throw BESInternalFatalError("unknown exception caught building DDS", __FILE__, __LINE__);
    }

    return true;
}

bool GDALRequestHandler::gdal_build_data(BESDataHandlerInterface & dhi)
{
    DBG(cerr << "In GDALRequestHandler::gdal_build_data" << endl);

    BESResponseObject *response = dhi.response_handler->get_response_object();
    BESDataDDSResponse *bdds = dynamic_cast<BESDataDDSResponse *> (response);
    if (!bdds)
        throw BESInternalError("cast error", __FILE__, __LINE__);

    try {
        bdds->set_container(dhi.container->get_symbolic_name());

        // This copies the vanilla DDS into a new GDALDDS object, where the
        // handler can store extra information about the dataset.
        GDALDDS *gdds = new GDALDDS(bdds->get_dds());

        // Now delete the old DDS object
        delete bdds->get_dds();

        // Now make the BESDataDDSResponse object use our new object. When it
        // deletes the GDALDDS, the GDAL library will be used to close the
        // dataset.
        bdds->set_dds(gdds);

        string filename = dhi.container->access();
        gdds->filename(filename);
        gdds->set_dataset_name(filename.substr(filename.find_last_of('/') + 1));

        // Save the dataset handle so that it can be closed later
        // when the BES is done with the DDS (which is really a GDALDDS,
        // spawn of DataDDS...)
        GDALDatasetH hDS = gdal_read_dataset_variables(gdds, filename);
        gdds->setGDALDataset(hDS);

        DAS *das = new DAS;
        BESDASResponse bdas(das);
        bdas.set_container(dhi.container->get_symbolic_name());
        gdal_read_dataset_attributes(*das, filename);
        Ancillary::read_ancillary_das(*das, filename);

        gdds->transfer_attributes(das);
#ifdef DEBUG_DEBUG
        cerr << "About to print vars info..." << endl;
        DDS::Vars_iter i = gdds->var_begin();
        while (i != gdds->var_end()) {
            BaseType *b = *i++;
            cerr << b->name() << " is a " << b->type_name() << "(" << typeid(*b).name() << ")" << endl;
        }
#endif
        bdds->set_constraint(dhi);

        bdds->clear_container();
    }
    catch (BESError &e) {
        throw;
    }
    catch (InternalErr & e) {
        throw BESDapError(sanitize(e.get_error_message()), true, e.get_error_code(), __FILE__, __LINE__);
    }
    catch (Error & e) {
        throw BESDapError(sanitize(e.get_error_message()), false, e.get_error_code(), __FILE__, __LINE__);
    }
    catch (...) {
        throw BESInternalFatalError("unknown exception caught building DAS", __FILE__, __LINE__);
    }

    return true;
}

bool GDALRequestHandler::gdal_build_help(BESDataHandlerInterface & dhi)
{
    BESResponseObject *response = dhi.response_handler->get_response_object();
    BESInfo *info = dynamic_cast<BESInfo *> (response);
    if (!info)
        throw BESInternalError("cast error", __FILE__, __LINE__);

    map < string, string > attrs;
    attrs["name"] = PACKAGE_NAME;
    attrs["version"] = PACKAGE_VERSION;
    list < string > services;
    BESServiceRegistry::TheRegistry()->services_handled(GDAL_NAME, services);
    if (services.size() > 0) {
        string handles = BESUtil::implode(services, ',');
        attrs["handles"] = handles;
    }
    info->begin_tag("module", &attrs);
    info->end_tag("module");

    return true;
}

bool GDALRequestHandler::gdal_build_version(BESDataHandlerInterface & dhi)
{
    BESResponseObject *response = dhi.response_handler->get_response_object();
    BESVersionInfo *info = dynamic_cast<BESVersionInfo *> (response);
    if (!info)
        throw BESInternalError("cast error", __FILE__, __LINE__);

    info->add_module(PACKAGE_NAME, PACKAGE_VERSION);

    return true;
}

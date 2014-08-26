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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

// GDALRequestHandler.cc

#include "config.h"

#include <string>
#include <sstream>

#include <DMR.h>
#include <mime_util.h>
#include <D4BaseTypeFactory.h>
#include <InternalErr.h>
#include <Ancillary.h>
#include <debug.h>

#include <BESResponseHandler.h>
#include <BESResponseNames.h>
#include <BESDapNames.h>
#include <BESDASResponse.h>
#include <BESDDSResponse.h>
#include <BESDataDDSResponse.h>
#include <BESDMRResponse.h>

#include <BESVersionInfo.h>
#include <BESDapError.h>
#include <BESInternalFatalError.h>
#include <BESDataNames.h>
#include <BESServiceRegistry.h>
#include <BESUtil.h>
#include <BESContextManager.h>
#include <BESDebug.h>
#include <TheBESKeys.h>

#include "GDAL_DDS.h"
#include "GDAL_DMR.h"
#include "GDALRequestHandler.h"

#define GDAL_NAME "gdal"

using namespace libdap;

extern void gdal_read_dataset_attributes(DAS &das, const string & filename);
extern GDALDatasetH gdal_read_dataset_variables(DDS *dds, const string & filename);


GDALRequestHandler::GDALRequestHandler(const string &name) :
    BESRequestHandler(name)
{
    add_handler(DAS_RESPONSE, GDALRequestHandler::gdal_build_das);
    add_handler(DDS_RESPONSE, GDALRequestHandler::gdal_build_dds);
    add_handler(DATA_RESPONSE, GDALRequestHandler::gdal_build_data);

    add_handler(DMR_RESPONSE, GDALRequestHandler::gdal_build_dmr);
    add_handler(DAP4DATA_RESPONSE, GDALRequestHandler::gdal_build_dmr);

    add_handler(HELP_RESPONSE, GDALRequestHandler::gdal_build_help);
    add_handler(VERS_RESPONSE, GDALRequestHandler::gdal_build_version);
}

GDALRequestHandler::~GDALRequestHandler()
{
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
        throw BESDapError(e.get_error_message(), true, e.get_error_code(), __FILE__, __LINE__);
    }
    catch (Error & e) {
        throw BESDapError(e.get_error_message(), false, e.get_error_code(), __FILE__, __LINE__);
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
        dds->set_dataset_name(filename.substr(filename.find_last_of('/') + 1));

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
        throw BESDapError(e.get_error_message(), true, e.get_error_code(), __FILE__, __LINE__);
    }
    catch (Error & e) {
        throw BESDapError(e.get_error_message(), false, e.get_error_code(), __FILE__, __LINE__);
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
        throw BESDapError(e.get_error_message(), true, e.get_error_code(), __FILE__, __LINE__);
    }
    catch (Error & e) {
        throw BESDapError(e.get_error_message(), false, e.get_error_code(), __FILE__, __LINE__);
    }
    catch (...) {
        throw BESInternalFatalError("unknown exception caught building DAS", __FILE__, __LINE__);
    }

    return true;
}

// FIXME
#include <D4Group.h>

bool GDALRequestHandler::gdal_build_dmr(BESDataHandlerInterface &dhi)
{
	// Because this code does not yet know how to build a DMR directly, use
	// the DMR ctor that builds a DMR using a 'full DDS' (a DDS with attributes).
	// First step, build the 'full DDS'
	string data_path = dhi.container->access();

	BaseTypeFactory factory;
	DDS dds(&factory, name_path(data_path), "3.2");
	dds.filename(data_path);

	GDALDatasetH hDS = 0;	// Set in the following block but needed later.

	try {
		hDS = gdal_read_dataset_variables(&dds, data_path);

		DAS das;
		gdal_read_dataset_attributes(das, data_path);
		Ancillary::read_ancillary_das(das, data_path);
		dds.transfer_attributes(&das);
	}
	catch (InternalErr &e) {
		throw BESDapError(e.get_error_message(), true, e.get_error_code(), __FILE__, __LINE__);
	}
	catch (Error &e) {
		throw BESDapError(e.get_error_message(), false, e.get_error_code(), __FILE__, __LINE__);
	}
	catch (...) {
		throw BESDapError("Caught unknown error building GDAL DMR response", true, unknown_error, __FILE__, __LINE__);
	}

	// Extract the DMR Response object - this holds the DMR used by the
	// other parts of the framework.
	BESResponseObject *response = dhi.response_handler->get_response_object();
	BESDMRResponse &bes_dmr = dynamic_cast<BESDMRResponse &>(*response);

	// In this handler we use a different pattern since the handler specializes the DDS/DMR.
	// First, build the DMR adding the open handle to the GDAL dataset, then free the DMR
	// the BES built and add this one. The GDALDMR object will close the open dataset when
	// the BES runs the DMR's destructor.

	DMR *dmr = bes_dmr.get_dmr();
	dmr->set_factory(new D4BaseTypeFactory);
	dmr->build_using_dds(dds);

	BESDEBUG("gdal", "BES DMR D4Group pointer: " << dmr->root() /*->get_parent()*/ << endl);

	GDALDMR *gdal_dmr = new GDALDMR(dmr);
	gdal_dmr->setGDALDataset(hDS);

	BESDEBUG("gdal", "gdal DMR D4Group pointer: " << gdal_dmr->root() /*->get_parent()*/ << endl);

	delete dmr;
	bes_dmr.set_dmr(gdal_dmr);

	XMLWriter xml;
	BESDEBUG("gdal", "About to print gdal DMR..." << endl);
	gdal_dmr->print_dap4(xml);
	BESDEBUG("gdal", "gdal DMR: " << xml.get_doc() << endl);

	// Instead of fiddling with the internal storage of the DHI object,
	// (by setting dhi.data[DAP4_CONSTRAINT], etc., directly) use these
	// methods to set the constraints. But, why? Ans: from Patrick is that
	// in the 'container' mode of BES each container can have a different
	// CE.
	bes_dmr.set_dap4_constraint(dhi);
	bes_dmr.set_dap4_function(dhi);

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

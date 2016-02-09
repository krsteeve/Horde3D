// *************************************************************************************************
//
// Horde3D
//   Next-Generation Graphics Engine
// --------------------------------------
// Copyright (C) 2006-2011 Nicolas Schulz
//
// This software is distributed under the terms of the Eclipse Public License v1.0.
// A copy of the license may be obtained at: http://www.eclipse.org/legal/epl-v10.html
//
// *************************************************************************************************

#ifndef _daeLibCameras_H_
#define _daeLibCameras_H_

#include "utXML.h"
#include <string>
#include <vector>
#include "daeLibVisualScenes.h"

using namespace Horde3D;


struct DaeLibCameras
{
	std::vector< DaeCamera * > cameras;
	std::string                id;
	std::string                name;

	~DaeLibCameras()
	{
		for (unsigned int i = 0; i < cameras.size(); ++i) delete cameras[i];
	}


	DaeCamera *findCamera(const std::string &id)
	{
		if( id == "" ) return 0x0;
		
		for (unsigned int i = 0; i < cameras.size(); ++i)
		{
			if (cameras[i]->id == id) return cameras[i];
		}

		return 0x0;
	}

	
	bool parse( const XMLNode &rootNode )
	{
		XMLNode node1 = rootNode.getFirstChild( "library_cameras" );
		if( node1.isEmpty() ) return true;

		XMLNode node2 = node1.getFirstChild( "camera" );
		while( !node2.isEmpty() )
		{
			DaeCamera *camera = new DaeCamera();
			if (camera->parse(node2)) cameras.push_back(camera);
			else delete camera;

			node2 = node2.getNextSibling( "camera" );
		}
		
		return true;
	}

};
#endif

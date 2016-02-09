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

#ifndef _daeLibLights_H_
#define _daeLibLights_H_

#include "utXML.h"
#include <string>
#include <vector>
#include "daeLibVisualScenes.h"

using namespace Horde3D;


struct DaeLibLights
{
	std::vector< DaeLight * > lights;
	std::string               id;
	std::string               name;

	~DaeLibLights()
	{
		for (unsigned int i = 0; i < lights.size(); ++i) delete lights[i];
	}


	DaeLight *findLight(const std::string &id)
	{
		if( id == "" ) return 0x0;
		
		for (unsigned int i = 0; i < lights.size(); ++i)
		{
			if (lights[i]->id == id) return lights[i];
		}

		return 0x0;
	}

	
	bool parse( const XMLNode &rootNode )
	{
		XMLNode node1 = rootNode.getFirstChild( "library_lights" );
		if( node1.isEmpty() ) return true;

		XMLNode node2 = node1.getFirstChild( "light" );
		while( !node2.isEmpty() )
		{
			DaeLight *light = new DaeLight();
			if( light->parse( node2 ) ) lights.push_back( light );
			else delete light;

			node2 = node2.getNextSibling( "light" );
		}
		
		return true;
	}

};
#endif

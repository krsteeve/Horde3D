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

#ifndef _converter_H_
#define _converter_H_

#include "daeMain.h"
#include "utMath.h"
#include <string.h> // memset

using namespace Horde3D;


struct Joint;

struct Vertex
{
	Vec3f  storedPos, pos;
	Vec3f  storedNormal, normal, tangent, bitangent;
	Vec3f  texCoords[4];
	Joint  *joints[4];
	float  weights[4];	
	int    daePosIndex;


	Vertex()
	{
		joints[0] = 0x0; joints[1] = 0x0; joints[2] = 0x0; joints[3] = 0x0;
		weights[0] = 1; weights[1] = 0; weights[2] = 0; weights[3] = 0;
	}
};


struct TriGroup
{
	unsigned int  first, count;
	unsigned int  vertRStart, vertREnd;
	std::string   matName;

	unsigned int                 numPosIndices;
	std::vector< unsigned int >  *posIndexToVertices;

	TriGroup() : posIndexToVertices( 0x0 )
	{
	}

	~TriGroup() { delete[] posIndexToVertices; }
};


struct SceneNode
{
	bool					    typeMesh;
	bool                        typeJoint;
	bool					    typeLight;
	bool					    typeCamera;
	char                        name[256];
	Matrix4f                    matRel, matAbs;
	DaeNode                     *daeNode;
	DaeInstance                 *daeInstance;
	SceneNode                   *parent;
	std::vector< SceneNode * >  children;

	// Animation
	std::vector< Matrix4f >     frames;  // Relative transformation for every frame

	SceneNode()
	{
		memset(name, 0, sizeof(name));
		daeNode = 0x0;
		daeInstance = 0x0;
		parent = 0x0;
	}

	virtual ~SceneNode()
	{
		for( unsigned int i = 0; i < children.size(); ++i ) delete children[i];
	}
};


struct Mesh : public SceneNode
{
	std::vector< TriGroup* > triGroups;
	unsigned int             lodLevel;
	
	Mesh()
	{
		typeMesh = true;
		typeJoint = false;
		typeLight = false;
		typeCamera = false;
		parent = 0x0;
		lodLevel = 0;
	}

	~Mesh() { for( int i = triGroups.size(); i>0; ) delete triGroups[--i];  }

};

struct Camera : public SceneNode
{
	Camera()
	{
		typeMesh = false;
		typeJoint = false;
		typeLight = false;
		typeCamera = true;
		parent = 0x0;
		nearPlane = 0.0f;
		nearDist = 0.0f;
		farPlane = 0.0f;
		topPlane = 0.0f;
		bottomPlane = 0.0f;
		leftPlane = 0.0f;
		rightPlane = 0.0f;
	}

	float nearPlane;
	float nearDist;
	float farPlane;
	float topPlane;
	float bottomPlane;
	float leftPlane;
	float rightPlane;
};

struct Light : public SceneNode
{
	Light()
	{
		typeMesh = false;
		typeJoint = false;
		typeLight = true;
		typeCamera = false;
		lightSpot = false;
		lightPoint = false;
		lightDirectional = false;
		parent = 0x0;
		col = Vec3f(0, 0, 0);
		constantAttenuation = 0.0f;
		linearAttenuation = 0.0f;
		quadraticAttenuation = 0.0f;
		falloffAngle = 0.0f;
		falloffExponent = 0.0f;
	}

	//light types
	bool lightDirectional;
	bool lightSpot;
	bool lightPoint;

	//all shared
	Vec3f col;

	//spot and point shared
	float constantAttenuation;
	float linearAttenuation;
	float quadraticAttenuation;

	//spot only
	float falloffAngle;
	float falloffExponent;
};

struct Joint : public SceneNode
{
	unsigned int  index;
	Matrix4f      invBindMat;
	bool          used;

	// Temporary
	Matrix4f      daeInvBindMat;

	Joint()
	{
		typeMesh = false;
		typeJoint = true;
		typeLight = false;
		typeCamera = false;
		used = false;
	}
};


struct MorphDiff
{
	unsigned int  vertIndex;
	Vec3f         posDiff;
	Vec3f         normDiff, tanDiff, bitanDiff;
};


struct MorphTarget
{
	char                      name[256];
	std::vector< MorphDiff >  diffs;

	MorphTarget()
	{
		memset(name, 0, sizeof(name));
	}
};


class Converter
{
public:
	Converter( ColladaDocument &doc, const std::string &outPath, float *lodDists );
	~Converter();
	
	bool convertModel( bool optimize );
	
	bool writeModel( const std::string &assetPath, const std::string &assetName, const std::string &modelName );
	bool writeMaterials( const std::string &assetPath, const std::string &modelName, bool replace );
	bool hasAnimation();
	bool writeAnimation( const std::string &assetPath, const std::string &assetName );

private:
	Matrix4f getNodeTransform( DaeNode &node, unsigned int frame );
	SceneNode *findNode( const char *name, SceneNode *ignoredNode );
	void checkNodeName( SceneNode *node );
	bool validateInstance( const std::string &instanceId );
	SceneNode *processNode( DaeNode &node, SceneNode *parentNode,
	                        Matrix4f transAccum, std::vector< Matrix4f > animTransAccum );
	void calcTangentSpaceBasis( std::vector< Vertex > &vertices );
	void processJoints();
	void processMeshes( bool optimize );
	bool writeGeometry( const std::string &assetPath, const std::string &assetName );
	void writeSGNode( const std::string &assetPath, const std::string &modelName, SceneNode *node, unsigned int depth, std::ofstream &outf );
	bool writeSceneGraph( const std::string &assetPath, const std::string &assetName, const std::string &modelName );
	void writeAnimFrames( SceneNode &node, FILE *f );

private:
	ColladaDocument              &_daeDoc;
	
	std::vector< Vertex >        _vertices;
	std::vector< unsigned int >  _indices;
	std::vector< Mesh * >        _meshes;
	std::vector< Joint * >       _joints;
	std::vector< Light * >       _lights;
	std::vector< Camera * >	     _cameras;
	std::vector< MorphTarget >   _morphTargets;
	std::vector< SceneNode* >    _nodes;

	std::string                  _outPath;
	float                        _lodDist1, _lodDist2, _lodDist3, _lodDist4;
	unsigned int                 _frameCount;
	unsigned int                 _maxLodLevel;
	bool                         _animNotSampled;
};

#endif // _converter_H_

//---------------------------------------------------------------------------
//
// Copyright (c) 2015 Taehyun Rhee, Joshua Scott, Ben Allen
//
// This software is provided 'as-is' for assignment of COMP308 in ECS,
// Victoria University of Wellington, without any express or implied warranty. 
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// The contents of this file may not be copied or duplicated in any form
// without the prior permission of its owner.
//
//----------------------------------------------------------------------------

#version 120

// Used for shadow lookup
varying vec4 ShadowCoord;



void main()
{


     	ShadowCoord= gl_TextureMatrix[7] * gl_Vertex;
  
		gl_Position = ftransform();

		gl_FrontColor = gl_Color;
}

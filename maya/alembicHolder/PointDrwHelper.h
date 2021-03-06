//-*****************************************************************************
//
// Copyright (c) 2009-2011,
//  Sony Pictures Imageworks, Inc. and
//  Industrial Light & Magic, a division of Lucasfilm Entertainment Company Ltd.
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
// *       Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
// *       Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
// *       Neither the name of Sony Pictures Imageworks, nor
// Industrial Light & Magic nor the names of their contributors may be used
// to endorse or promote products derived from this software without specific
// prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//-*****************************************************************************

#ifndef _AlembicHolder_PointDrwHelper_h_
#define _AlembicHolder_PointDrwHelper_h_

#include "Foundation.h"
#include "DrawContext.h"
#include "RenderModules.h"
#include "boost/noncopyable.hpp"


namespace AlembicHolder {


//-*****************************************************************************
//! \brief Both the SubD and PolyMesh classes draw in the same way, so we
//! create this helper class to do the common work.



class PointDrwHelper
{
public:
    // Default constructor
    PointDrwHelper();

    // Destructor
    ~PointDrwHelper();

    // This is a "full update" of all parameters.
    // If N is empty, normals will be computed.
    void update( P3fArraySamplePtr iP,
                 Abc::Box3d iBounds = Abc::Box3d(), 
                 double alpha = 0.0 );

    // Update just positions and possibly normals
    /*void update( P3fArraySamplePtr iP,
                 P3fArraySamplePtr iPCeil,
                 N3fArraySamplePtr iN,
                 Abc::Box3d iBounds = Abc::Box3d(), 
                 double alpha = 0.0 );*/


    // This returns constancy.
    bool isConstant() const { return m_isConstant; }
    void setConstant( bool isConstant = true ) { m_isConstant = isConstant; }

    // This returns validity.
    bool valid() const { return m_valid; }

    // Return the bounds.
    Box3d getBounds() const { return m_bounds; }

    // And, finally, this draws.
    void draw( const DrawContext & iCtx) const;

    // This is a weird thing. Just makes the helper invalid
    // by nulling everything out. For internal use.
    void makeInvalid();

    void setName(std::string name) { m_name = name; }

protected:
    void computeBounds();

  /*  typedef Imath::Vec3<unsigned int> Tri;
    typedef std::vector<Tri> TriArray;*/

    P3fArraySamplePtr m_pointsP;
/*    N3fArraySamplePtr m_meshN;
    Int32ArraySamplePtr m_meshIndices;
    Int32ArraySamplePtr m_meshCounts;*/

    BufferObject buffer;

    MGLuint mVertexBuffer, mIndexBuffer;
    MGLenum mPrimType;
    MGLsizei mPrimNum;

    //std::vector<C4f> m_colors;

    bool m_valid;
    bool m_isConstant;
    Box3d m_bounds;

    std::string m_name;

};



} // End namespace AlembicHolder

#endif

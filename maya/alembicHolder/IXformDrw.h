//-*****************************************************************************
//
// Copyright (c) 2009-2010,
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

#ifndef _AlembicHolder_IXformDrw_h_
#define _AlembicHolder_IXformDrw_h_

#include "Foundation.h"
#include "IObjectDrw.h"

namespace AlembicHolder {

//-*****************************************************************************
//! Transform applies a matrix transformation to all of its children
//! objects. Because we have deep, deep hierarchies, the transformation
//! is applied via an internally managed transform stack, rather than
//! through OpenGL's.
class IXformDrw : public IObjectDrw
{
public:
    IXformDrw( IXform &iXform, std::vector<std::string> path);

    virtual ~IXformDrw();

    virtual bool valid();

    virtual void setTime( chrono_t iSeconds );

	virtual Box3d getBounds();
    virtual void draw( const DrawContext & iCtx );

    virtual int getNumTriangles();

    virtual void accept(DrawableVisitor& visitor) const { visitor.visit(*this); }

protected:
    IXform m_xform;
    M44d m_localToParent;
};

} // End namespace AlembicHolder

#endif

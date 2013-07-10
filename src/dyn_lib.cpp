/*
 * -----------------------------------------------------------------------------
 * This source file is part of OGRE
 *     (Object-oriented Graphics Rendering Engine)
 * For the latest info, see http://www.ogre3d.org/
 *
 * This source file has been incorporated into libalgol (http://www.algollabs.com)
 *
 * Copyright (c) 2000-2012 Torus Knot Software Ltd,
 *               2011-2012 algol
 *
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 * -----------------------------------------------------------------------------
 */

#include "algol/dyn_lib.hpp"

#if ALGOL_PLATFORM == ALGOL_PLATFORM_WIN32
#  define WIN32_LEAN_AND_MEAN
#  if !defined(NOMINMAX) && defined(_MSC_VER)
# define NOMINMAX // required to stop windows.h messing up std::min
#  endif
#  include <windows.h>
#else
#include <dlfcn.h>
# if ALGOL_PLATFORM == ALGOL_PLATFORM_APPLE || ALGOL_PLATFORM == ALGOL_PLATFORM_APPLE_IOS
#   include "macUtils.h"
# endif
#endif


namespace algol {

    //-----------------------------------------------------------------------
    dyn_lib::dyn_lib( const string_t& name )
    {
        mName = name;
        mInst = NULL;
    }

    //-----------------------------------------------------------------------
    dyn_lib::~dyn_lib()
    {
    }

  //-----------------------------------------------------------------------
  void dyn_lib::load()
  {
    // Log library load
    string_t name = mName;
#if ALGOL_PLATFORM == ALGOL_PLATFORM_LINUX
    // dlopen() does not add .so to the filename, like windows does for .dll
     if (name.find(".so") == string_t::npos)
       name += ".so";
#elif ALGOL_PLATFORM == ALGOL_PLATFORM_APPLE
    // dlopen() does not add .dylib to the filename, like windows does for .dll
    if (name.substr(name.length() - 6, 6) != ".dylib")
    name += ".dylib";
#elif ALGOL_PLATFORM == ALGOL_PLATFORM_WIN32
    // Although LoadLibraryEx will add .dll itself when you only specify the library name,
    // if you include a relative path then it does not. So, add it to be sure.
    if (name.substr(name.length() - 4, 4) != ".dll")
      name += ".dll";
#endif
    mInst = (DYNLIB_HANDLE)DYNLIB_LOAD( name.c_str() );

    if( !mInst )
      throw std::runtime_error("Could not load dynamic library " + mName + ". System error: " + dynlibError());
  }

    //-----------------------------------------------------------------------
    void dyn_lib::unload()
    {
      // Log library unload
      if( DYNLIB_UNLOAD( mInst ) ) {
        throw std::runtime_error("Could not unload dynamic library " + mName + ". System Error: " + dynlibError());
      }
    }

    //-----------------------------------------------------------------------
    void* dyn_lib::getSymbol( const string_t& strName ) const throw()
    {
      return (void*)DYNLIB_GETSYM( mInst, strName.c_str() );
    }
    //-----------------------------------------------------------------------
    string_t dyn_lib::dynlibError( void )
    {
#if ALGOL_PLATFORM == ALGOL_PLATFORM_WIN32
        LPVOID lpMsgBuf;
        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            GetLastError(),
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR) &lpMsgBuf,
            0,
            NULL
            );
        string_t ret = (char*)lpMsgBuf;
        // Free the buffer.
        LocalFree( lpMsgBuf );
        return ret;
#elif ALGOL_PLATFORM == ALGOL_PLATFORM_LINUX || ALGOL_PLATFORM == ALGOL_PLATFORM_APPLE
        return string_t(dlerror());
#else
        return string_t("");
#endif
    }

}

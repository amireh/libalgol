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

#ifndef H_ALGOL_DYNLIB_H
#define H_ALGOL_DYNLIB_H

#include <algol/algol.hpp>
#include <algol/platform.hpp>
#include <algol/logger.hpp>

#if ALGOL_PLATFORM == ALGOL_PLATFORM_WIN32
#    define DYNLIB_HANDLE hInstance
#    define DYNLIB_LOAD( a ) LoadLibraryEx( a, NULL, LOAD_WITH_ALTERED_SEARCH_PATH )
#    define DYNLIB_GETSYM( a, b ) GetProcAddress( a, b )
#    define DYNLIB_UNLOAD( a ) !FreeLibrary( a )

struct HINSTANCE__;
typedef struct HINSTANCE__* hInstance;

#elif ALGOL_PLATFORM == ALGOL_PLATFORM_LINUX || ALGOL_PLATFORM == ALGOL_PLATFORM_ANDROID
#    define DYNLIB_HANDLE void*
#    define DYNLIB_LOAD( a ) dlopen( a, RTLD_LAZY | RTLD_GLOBAL)
#    define DYNLIB_GETSYM( a, b ) dlsym( a, b )
#    define DYNLIB_UNLOAD( a ) dlclose( a )

#elif ALGOL_PLATFORM == ALGOL_PLATFORM_APPLE || ALGOL_PLATFORM == ALGOL_PLATFORM_APPLE_IOS
#    define DYNLIB_HANDLE void*
#    define DYNLIB_LOAD( a ) mac_loadDylib( a )
#    define DYNLIB_GETSYM( a, b ) dlsym( a, b )
#    define DYNLIB_UNLOAD( a ) dlclose( a )

#endif

namespace algol {

  /**
   * \addtogroup Core
   * @{
   * @class dyn_lib
   *
   * Resource holding data about a dynamic library.
   *
   * @remarks
   *     This class holds the data required to get symbols from
   *     libraries loaded at run-time (i.e. from DLL's for so's)
   * @author
   *     Adrian Cearn„u (cearny@cearny.ro)
   * @since
   *     27 January 2002
   */
  class dyn_lib {

  public:
    /**
     * Default constructor - used by algol::plugin_manager.
     * @warning
     *   Do not call directly
     */
    dyn_lib( const string_t& name );

    /** Default destructor */
    virtual ~dyn_lib();

    /** Load the library */
    void load();

    /** Unload the library */
    void unload();

    /** Get the name of the library */
    const string_t& getName(void) const { return mName; }

    /**
     * Returns the address of the given symbol from the loaded library.
     * @param
     *   strName The name of the symbol to search for
     * @return
     *   If the function succeeds, the returned value is a handle to
     *   the symbol.
     * @par
     *   If the function fails, the returned value is <b>NULL</b>.
     */
    void* getSymbol( const string_t& strName ) const throw();

  protected:
    string_t mName;

    /// Gets the last loading error
    string_t dynlibError(void);

    /// Handle to the loaded library.
    DYNLIB_HANDLE mInst;
  };
  /** @} */

}

#endif

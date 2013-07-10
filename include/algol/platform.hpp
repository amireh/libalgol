/* libalgol - a collection of plug-ins for developing back-end C++ web tools
 * Copyright (c) 2013 Algol Labs, LLC.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef H_ALGOL_PLATFORM_H
#define H_ALGOL_PLATFORM_H

/* Finds the current platform
 * Note: proudly stolen from Ogre3D code in OgrePlatform.h */
#define ALGOL_PLATFORM_WIN32 1
#define ALGOL_PLATFORM_LINUX 2
#define ALGOL_PLATFORM_APPLE 3
#define ALGOL_PLATFORM_SYMBIAN 4
#define ALGOL_PLATFORM_IPHONE 5

#if defined( __SYMBIAN32__ )
#   define ALGOL_PLATFORM ALGOL_PLATFORM_SYMBIAN
#elif defined( __WIN32__ ) || defined( _WIN32 )
#   define ALGOL_PLATFORM ALGOL_PLATFORM_WIN32
#elif defined( __APPLE_CC__) || defined(__APPLE__)
#   if __ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__ >= 30000 || __IPHONE_OS_VERSION_MIN_REQUIRED >= 30000
#       define ALGOL_PLATFORM ALGOL_PLATFORM_IPHONE
#   else
#       define ALGOL_PLATFORM ALGOL_PLATFORM_APPLE
#   endif
#else
#   define ALGOL_PLATFORM ALGOL_PLATFORM_LINUX
#endif

#endif

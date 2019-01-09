/*******************************************************************************

Copyright (C) The University of Auckland

OpenCOR is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OpenCOR is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*******************************************************************************/

//==============================================================================
// LLVMClang begin
//==============================================================================

#if defined(Q_OS_WIN)
    #pragma warning(push)
    #pragma warning(disable: 4141)
    #pragma warning(disable: 4146)
    #pragma warning(disable: 4267)
    #pragma warning(disable: 4291)
#elif defined(Q_OS_LINUX)
    #if defined(__GNUC__) && (__GNUC__ >= 8)
        #pragma GCC diagnostic ignored "-Wclass-memaccess"
    #endif
    #pragma GCC diagnostic ignored "-Wmissing-field-initializers"
    #pragma GCC diagnostic ignored "-Wstrict-aliasing"
    #pragma GCC diagnostic ignored "-Wunused-parameter"
#elif defined(Q_OS_MAC)
    #pragma GCC diagnostic ignored "-Wunused-parameter"
#else
    #error Unsupported platform
#endif

//==============================================================================
// End of file
//==============================================================================

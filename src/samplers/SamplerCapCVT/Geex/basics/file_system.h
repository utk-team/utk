/*
 *  _____ _____ ________  _
 * /  __//  __//  __/\  \//
 * | |  _|  \  |  \   \  / 
 * | |_//|  /_ |  /_  /  \ 
 * \____\\____\\____\/__/\\
 *
 * Graphics Environment for EXperimentations.
 *  Copyright (C) 2006 INRIA - Project ALICE
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  If you modify this software, you should include a notice giving the
 *  name of the person performing the modification, the date of modification,
 *  and the reason for such modification.
 *
 *  Contact: 
 *
 *     ALICE Project - INRIA
 *     INRIA Lorraine, 
 *     Campus Scientifique, BP 239
 *     54506 VANDOEUVRE LES NANCY CEDEX 
 *     FRANCE
 *
 *  Note that the GNU General Public License does not permit incorporating
 *  the Software into proprietary programs. 
 */
 
#ifndef __GEEX_BASICS_FILE_SYSTEM__
#define __GEEX_BASICS_FILE_SYSTEM__

#include <Geex/basics/types.h>

#include <string>
#include <vector>

namespace Geex {

    namespace FileSystem {

        // OS-dependent functions

        bool GEEX_API is_file(const std::string& filename) ;
        bool GEEX_API is_directory(const std::string& filename) ;    
        bool GEEX_API create_directory(const std::string& path) ; // Warning: path should be absolute.
        bool GEEX_API delete_directory(const std::string& path) ;
        bool GEEX_API delete_file(const std::string& filename) ;
        void GEEX_API get_directory_entries(
            const std::string& dirname_in, std::vector<std::string>& result
        ) ;
        std::string GEEX_API get_current_working_directory() ;
        bool GEEX_API set_current_working_directory(const std::string& path) ;
        bool GEEX_API rename_file(const std::string& old_name, const std::string& new_name) ;
        unsigned int GEEX_API get_time_stamp(const std::string& filename) ;

        // OS-independent functions

        std::string GEEX_API extension(const std::string& path) ;
        std::string GEEX_API base_name(const std::string& path) ;
        std::string GEEX_API dir_name(const std::string& path) ;

        void GEEX_API get_directory_entries(
            const std::string& dirname_in, 
            std::vector<std::string>& result, bool recursive 
        ) ;

        void GEEX_API get_files(
            const std::string& dirname, 
            std::vector<std::string>& result, bool recursive = false
        ) ;

        void GEEX_API get_subdirectories(
            const std::string& dirname, 
            std::vector<std::string>& result, bool recursive = false
        ) ;

        bool GEEX_API copy_file(const std::string& original, const std::string& copy);

        std::string GEEX_API get_project_root() ;

        Memory::pointer GEEX_API read_file(const std::string& file_name, bool ascii) ;
    }

}

#endif

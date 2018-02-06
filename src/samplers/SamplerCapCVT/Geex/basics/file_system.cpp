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
 
#include <Geex/basics/file_system.h>
#include <Geex/basics/line_stream.h>

#include <iostream>
#include <fstream>
#include <assert.h>
#include <stdio.h>

#ifdef WIN32
#include <windows.h>
#include <io.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <stdio.h>
#endif

#include <stdlib.h>

namespace Geex {

    namespace FileSystem {

// OS-dependent functions
#ifdef WIN32

        static void flip_slashes(std::string& s) {
            for(unsigned int i=0; i<s.length(); i++) {
                if(s[i] == '\\') {
                    s[i] = '/' ;
                }
            }
        }

        bool is_file(const std::string& filename) {
            WIN32_FIND_DATA file;
            HANDLE file_handle;
            std::string file_name;
            file_handle=FindFirstFile(filename.c_str(),&file);
            if (file_handle==INVALID_HANDLE_VALUE ) {
                return false;
            }
            FindClose(file_handle);
            return ((file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0);
        }

        bool is_directory(const std::string& filename) {
            WIN32_FIND_DATA file;
            HANDLE file_handle;
            std::string file_name;
            file_handle=FindFirstFile(filename.c_str(),&file);
            if (file_handle==INVALID_HANDLE_VALUE ) {
                return false;
            }
            FindClose(file_handle);
            return ((file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0 ); 
        }

        bool create_directory(const std::string& path_in) {
            std::vector<std::string> path ;
            String::split_string(path_in, '/', path) ;
            std::string current ;
            int start_at=0;
            if (path_in.at(1)==':') {
                current+=path_in.at(0);
                current+=path_in.at(1);
                start_at=1;
            }
            else if (path_in.at(0)!='/' && path_in.at(0)!='\\') {
                current+=get_current_working_directory();
            }
            for(unsigned int i=start_at; i<path.size(); i++) {
                current += "/";
                current += path[i] ;
				if (path[i].at(0) == '.' && path[i].at(1) == '.' && path[i].length() == 2)
					continue;
                if(!is_directory(current)) {
                    if(!::CreateDirectory(current.c_str(),NULL)) {
                        std::cerr << "could not mkdir " << current << std::endl ;
                        return false ;
                    }
                }
            }
            return true ;
        }

        bool delete_directory(const std::string& path) {
            return (::RemoveDirectory(path.c_str()) != FALSE);
        }

        bool delete_file(const std::string& filename) {
            return (::DeleteFile(filename.c_str())!= FALSE);
        }

        void get_directory_entries(
            const std::string& dirname_in, std::vector<std::string>& result
        ) {
            WIN32_FIND_DATA file;
            HANDLE file_handle;
            std::string file_name, dirname=dirname_in;
            if (dirname.at(dirname.size()-1)!='/' && dirname.at(dirname.size()-1)!='\\') {
                dirname+='/';
            }

            std::string current_directory=get_current_working_directory();

            bool dir_found = set_current_working_directory(dirname) ;
            assert(dir_found) ;
            file_handle=FindFirstFile("*.*",&file);
            do {
                if (file_handle != INVALID_HANDLE_VALUE ) {
                    file_name = file.cFileName;
                    if (file_name != "." && file_name != "..") {
                        file_name= dirname+file_name;
                        flip_slashes(file_name) ;
                        result.push_back(file_name);
                    }
                }
            } while ( FindNextFile(file_handle,&file) );
            FindClose(file_handle);
            set_current_working_directory(current_directory);

        }

        std::string get_current_working_directory() {
            char temp_current_directory[2048];
            std::string result="";
            if (GetCurrentDirectory(2048,temp_current_directory)) {
                result=temp_current_directory;
                flip_slashes(result) ;
            }
            //result = result.substr(2,result.length()-2) ;
            return result;
        }

        bool set_current_working_directory(const std::string& path_in) {
            std::string path=path_in;
            bool temp=path.at(path.size()-1)!='/';
            if (path.at(path.size()-1)!='/' && path.at(path.size()-1)!='\\') {
                path+="/";
            }
            return (SetCurrentDirectory(path.c_str())!=-1) ;
        }

        bool rename_file(
            const std::string& old_name,const std::string& new_name
        ) {
            return (::rename(old_name.c_str(), new_name.c_str())!=-1) ;
        }

        unsigned int get_time_stamp(const std::string& filename)
        {
                WIN32_FILE_ATTRIBUTE_DATA infos;
                if (!GetFileAttributesEx(filename.c_str(), GetFileExInfoStandard, &infos))
                        return 0;
                return infos.ftLastWriteTime.dwLowDateTime;
        }

#else


        bool is_file(const std::string& filename) {
            //   Using lstat instead of stat makes the system
            // ignore symlinks (this is what we want since we
            // do not want to use them in the source tree)
            struct stat buff ;
            if(lstat(filename.c_str(), &buff)) {
                return false ;
            }
            return (S_ISREG(buff.st_mode));
        }

        bool is_directory(const std::string& filename) {
            //   Using lstat instead of stat makes the system
            // ignore symlinks (this is what we want since we
            // do not want to use them in the source tree)
            struct stat buff ;
            if(lstat(filename.c_str(), &buff)) {
                return false ;
            }
            return (S_ISDIR(buff.st_mode));
        }

        bool create_directory(const std::string& path_in) {
            std::vector<std::string> path ;
            String::split_string(path_in, '/', path) ;
            std::string current ;
            for(unsigned int i=0; i<path.size(); i++) {
                current += "/" ;
                current += path[i] ;
                if(!is_directory(current)) {
                    if(mkdir(current.c_str(), ~0) != 0) {
                        std::cerr << "could not mkdir" << current << std::endl ;
                        return false ;
                    }
                }
            }
            return true ;
        }

        bool delete_directory(const std::string& path) {
            return (rmdir(path.c_str()) == 0) ;
        }

        bool delete_file(const std::string& filename) {
            return (unlink(filename.c_str()) == 0) ;
        }

        void get_directory_entries(
            const std::string& dirname_in, std::vector<std::string>& result
        ) {
            std::string dirname = dirname_in ;
            if(dirname[dirname.length() - 1] != '/') {
                dirname += "/" ;
            }
            DIR* dir = opendir(dirname.c_str()) ;
            if(dir == NULL) {
                std::cerr << "cannot open dir " << dirname << std::endl ;
                assert(false) ;
            }
            struct dirent* entry = readdir(dir) ;
            while(entry != NULL) {
                std::string current = std::string(entry->d_name) ;
                // Ignore . and ..
                if(current != "." && current != "..") {
                    if(dirname != "./") {
                        current = dirname + current ;
                    }
                    // Ignore symbolic links and other special Unix stuff
                    if(is_file(current) || is_directory(current)) {
                        result.push_back(current) ;
                    }
                }
                entry = readdir(dir) ;
            }
            closedir(dir) ;
        }

        std::string get_current_working_directory() {
            char buff[4096] ;
            return std::string(getcwd(buff, 4096)) ;
        }

        bool set_current_working_directory(const std::string& path) {
            return (chdir(path.c_str()) == 0) ;
        }

        bool rename_file(const std::string& old_name, const std::string& new_name) {
            if(is_file(new_name)) {
                return false ;
            }
            return (::rename(old_name.c_str(), new_name.c_str()) == 0) ;
        }

        unsigned int get_time_stamp(const std::string& filename) {
                struct stat buffer;
                if (!stat(filename.c_str(), &buffer))
                        return buffer.st_mtime;
                return 0;
        }

#endif

// OS-independent functions

        std::string extension(const std::string& path) {
            int len = int(path.length()) ;
            int point_pos  = -1 ;
            int base_start = -1 ;
            {for(int i=len-1; i>=0; i--) {
                if(point_pos == -1 && base_start == -1 && path[i] == '.') {
                    point_pos = i ;
                }
                if(base_start == -1 && (path[i] == '/' || path[i] == '\\')) {
                    base_start = i ;
                }
            }}
            if(point_pos == -1) {
                return "" ;
            }
            std::string result = path.substr(point_pos + 1, len - point_pos - 1) ;
            {for(unsigned int i=0; i<result.length(); i++) {
                result[i] = tolower(result[i]) ;
            }}
            return result ;
        }

        std::string base_name(const std::string& path) {
            int len = int(path.length()) ;
            int point_pos  = -1 ;
            int base_start = -1 ;
            for(int i=len-1; i>=0; i--) {
                if(point_pos == -1 && base_start == -1 && path[i] == '.') {
                    point_pos = i ;
                }
                if(base_start == -1 && (path[i] == '/' || path[i] == '\\')) {
                    base_start = i ;
                }
            }
            if(point_pos == -1) {
                point_pos = len ;
            }
            return path.substr(base_start + 1, point_pos - base_start - 1) ;
        }

        std::string dir_name(const std::string& path) {
            int len = int(path.length()) ;
            int base_start = -1 ;
            for(int i=len-1; i>=0; i--) {
                if(base_start == -1 && (path[i] == '/' ||path[i] == '\\')) {
                    base_start = i ;
                }
            }
            if(base_start == -1) {
                return "." ;
            }
            std::string result = path.substr(0, base_start + 1) ;
            if(result[result.length() - 1] == '/') {
                result = result.substr(0, result.length() - 1) ;
            }
            return result ;
        }

        void get_directory_entries(
            const std::string& path, std::vector<std::string>& result, bool recursive 
        ) {
            get_directory_entries(path, result) ;
            if(recursive) {
                for(unsigned int i=0; i<result.size(); i++) {
                    if(is_directory(result[i])) {
                        get_directory_entries(result[i], result) ;
                    }
                }
            }
        }

        void get_files(
            const std::string& dirname, std::vector<std::string>& result, bool recursive
        ) {
            std::vector<std::string> entries ;
            get_directory_entries(dirname, entries, recursive) ;
            for(unsigned int i=0; i<entries.size(); i++) {
                if(is_file(entries[i])) {
                    result.push_back(entries[i]) ;
                }
            }
        }

        void get_subdirectories(
            const std::string& dirname, std::vector<std::string>& result, bool recursive
        ) {
            std::vector<std::string> entries ;
            get_directory_entries(dirname, entries, recursive) ;
            for(unsigned int i=0; i<entries.size(); i++) {
                if(is_directory(entries[i])) {
                    result.push_back(entries[i]) ;
                }
            }
        }

        bool copy_file(const std::string& original, const std::string& copy)
        {
            std::cerr << "Copy " << original << " to " << copy << std::endl;
            std::ifstream in(original.c_str());
            if (!in)
                return false;
            std::ofstream out(copy.c_str());
            LineInputStream lis(in);
            while(!lis.eof())
            {
                lis.get_line();
                out << lis.current_line() << std::endl ;
            }
            return true;
        }

        std::string get_project_root() {
            std::string project_root ;
        
            std::string start = FileSystem::get_current_working_directory() ;
            std::vector<std::string> path ;
            String::split_string(start, '/', path) ;

            bool found = false ;

            while(path.size() > 0) {
                std::string tmp ;
                String::join_strings(path, '/', tmp) ;		
#ifdef WIN32
                project_root = tmp ;
#else
                project_root = "/" + tmp ;
#endif
                if(FileSystem::is_file(project_root + "/PROJECT_ROOT")) {
                    found = true ;
                    break ;
                }
                path.pop_back() ;
            }

            if(!found) {
                std::cerr << "Fatal error: did not find project root" << std::endl ;
                exit(-1) ;
            }

            return project_root ;
        }

        Memory::pointer read_file(const std::string& file_name, bool ascii) {
            FILE* f = fopen(file_name.c_str(), ascii ? "rt" : "rb") ;
            if(f == NULL) { return nil ; }
            fseek(f, 0, SEEK_END) ;
            long size = ftell(f) ;
            if(ascii) { size++ ; }
            Memory::pointer result = new Memory::byte[size] ;
            rewind(f) ;
            fread(result, 1, size, f) ;
            if(ascii) { result[size-1] = '\0' ; }
            return result ;
        }

    }

}

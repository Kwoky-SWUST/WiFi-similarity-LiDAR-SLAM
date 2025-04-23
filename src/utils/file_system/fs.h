#ifndef UTILS_FILE_SYSTEM_H
#define UTILS_FILE_SYSTEM_H

#include <string>
#include <vector>

/**
 *  Get a list of directories/files in a given folder
 */
std::vector<std::string> getDirectories( const std::string &directory, const bool full_abs_path = true );
std::vector<std::string> getFiles( const std::string &directory, const bool full_abs_path = true );
std::vector<std::string> getFilesAndDirectories( const std::string &directory, const bool full_abs_path = true );


/**
 *  Get a list of directories/files in a given folder satisfying given mask
 *  mask example: a*bcd*.cpp (prefix sensitive, i.e. l* != *l)
 */
std::vector<std::string> getDirectories( const std::string &directory, const std::string& mask, const bool full_abs_path = true );
std::vector<std::string> getFiles( const std::string &directory, const std::string& mask, const bool full_abs_path = true );
std::vector<std::string> getFilesAndDirectories( const std::string &directory, const std::string& mask, const bool full_abs_path = true );

/** tests whether a file exists
 * @param file_path path to file
 * @return true/false
 */
bool file_exists( const std::string& file_path );

#endif

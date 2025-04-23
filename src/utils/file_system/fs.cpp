#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "fs.h"


bool file_exists( const std::string& file_path ) {
  struct stat buffer;
  return (stat(file_path.c_str(), &buffer) == 0);
}

enum EEntityType {
  ETYPE_FILES = 1,
  ETYPE_DIRECTORIES = 2,
  ETYPE_FILES_AND_DIRECTORIES = 3
};


std::vector<std::string> getEntities(const std::string &directory, const EEntityType mode = ETYPE_FILES, const bool full_abs_path = true ) {
  DIR *dir;
  class dirent *ent;
  class stat st;
  std::vector<std::string> out;

  bool add_separator = directory[directory.size()-1] != '/';

  dir = opendir(directory.c_str());
  while ((ent = readdir(dir)) != NULL) {
    const std::string file_name = ent->d_name;

    std::string full_file_name= directory;
    if( add_separator )
      full_file_name += "/";
    full_file_name += file_name;

    if (file_name[0] == '.')
      continue;

    if (stat(full_file_name.c_str(), &st) == -1)
      continue;

    const bool is_directory = ((st.st_mode & S_IFDIR) != 0);

    const bool keep =
        (mode == ETYPE_FILES_AND_DIRECTORIES) ||
        (is_directory && (mode == ETYPE_DIRECTORIES)) ||
        (!is_directory && (mode == ETYPE_FILES));

    if (!keep)
      continue;

    if( full_abs_path )
      out.push_back(full_file_name);
    else
      out.push_back(file_name);
  }
  closedir(dir);
  return out;
}


std::vector<std::string> tokenizeMask( const std::string& mask ) {
  std::vector<std::string> tokens;
  if(mask.empty())
    return tokens;
  if( mask[0] == '*' )
    tokens.push_back("*");
  size_t begin = 0,
         end = 0,
         length = 0;
  do {
    end = mask.find( '*', begin );
    if( end == std::string::npos )
      end = mask.size();
    length = end - begin;
    if( length > 0 )
      tokens.push_back( std::string( mask, begin, length ) );
    begin = end + 1;
  } while( begin < mask.size() );
  return tokens;
}


bool checkEntity( const std::string& entity, const std::vector<std::string>& maskTokens ) {
  size_t begin = 0,
         end = 0,
         curTokenIndex = 1;

  // check for prefix if necessary
  if( maskTokens[0] != "*" ) {
    curTokenIndex = 0;
    const std::string& curMask = maskTokens[ 0 ];
    if( entity.find( curMask ) != 0 )
      return false;
  }

  bool passed = true;

  // check for other tokens
  do {
    const std::string& curMask = maskTokens[ curTokenIndex ];
    end = entity.find( curMask, begin );
    if( end == std::string::npos )
      passed = false;
    begin = end + curMask.length();
    curTokenIndex++;
  } while( passed && curTokenIndex < maskTokens.size() );

  return passed;
}


std::vector<std::string> getMaskedEntities( const std::vector<std::string>& entities, const std::string& mask ) {
  if( entities.empty() || mask.empty() || mask == "*" )
    return entities;

  std::vector< std::string > passingEntities;
  std::vector< std::string > maskTokens = tokenizeMask( mask );

  for( size_t i=0; i<entities.size(); ++i ) {
    const std::string& curEntity = entities[i];
    if( checkEntity( curEntity, maskTokens ) )
      passingEntities.push_back( curEntity );
  }

  return passingEntities;
}


std::vector<std::string> prependFullPath( const std::vector< std::string >& entities, std::string path ) {
  std::vector<std::string> ret;
  if(path[path.size()-1] != '/')
    path += '/';
  for( size_t i = 0; i < entities.size(); ++i ) {
    ret.push_back( path + entities[i] );
  }
  return ret;
}


std::vector<std::string> getDirectories(const std::string &directory, const bool full_abs_path /* = true */)
{
  return getEntities( directory, ETYPE_DIRECTORIES, full_abs_path );
}

std::vector<std::string> getFiles(const std::string &directory, const bool full_abs_path /* = true */)
{
  return getEntities( directory, ETYPE_FILES, full_abs_path );
}

std::vector<std::string> getFilesAndDirectories(const std::string &directory, const bool full_abs_path /* = true */)
{
  return getEntities( directory, ETYPE_FILES_AND_DIRECTORIES, full_abs_path );
}


std::vector<std::string> getDirectories(const std::string &directory, const std::string& mask, const bool full_abs_path /* = true */) {
  std::vector<std::string> passing_entities = getMaskedEntities( getDirectories( directory, false ), mask );
  if( full_abs_path )
    return prependFullPath( passing_entities, directory );
  else
    return passing_entities;
}

std::vector<std::string> getFiles(const std::string &directory, const std::string& mask, const bool full_abs_path /* = true */) {
  std::vector<std::string> passing_entities = getMaskedEntities( getFiles( directory, false ), mask );
  if( full_abs_path )
    return prependFullPath( passing_entities, directory );
  else
    return passing_entities;
}

std::vector<std::string> getFilesAndDirectories(const std::string &directory, const std::string& mask, const bool full_abs_path /* = true */) {
  std::vector<std::string> passing_entities = getMaskedEntities( getFilesAndDirectories( directory, false ), mask );
  if( full_abs_path )
    return prependFullPath( passing_entities, directory );
  else
    return passing_entities;
}

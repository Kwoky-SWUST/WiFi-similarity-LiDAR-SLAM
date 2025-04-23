#ifndef UTILS_STD_EXTENSIONS_HPP_984500932090
#define UTILS_STD_EXTENSIONS_HPP_984500932090

#include <set>
#include <map>
#include <vector>
#include <algorithm>

namespace std_ext {

  /// set operations

  template < class T >
  T set_union( const T & A, const T & B ) {
    T ret; // A OR B
    std::set_union(A.begin(), A.end(), B.begin(), B.end(), std::inserter( ret, ret.end() ));
    return ret;
  }

  template < class T >
  inline T set_intersection( const T & A, const T & B ) {
    T ret; // A AND B
    std::set_intersection(A.begin(), A.end(), B.begin(), B.end(), std::inserter( ret, ret.end() ));
    return ret;
  }

  template < class T >
  inline T set_difference( const T & A, const T & B ) {
    T ret; // A - B
    std::set_difference(A.begin(), A.end(), B.begin(), B.end(), std::inserter( ret, ret.end() ));
    return ret;
  }

  template < class T >
  inline T set_symmetric_difference( const T & A, const T & B ) {
    T ret; // A XOR B
    std::set_symmetric_difference(A.begin(), A.end(), B.begin(), B.end(), std::inserter( ret, ret.end() ));
    return ret;
  }


  /// generic sorting of indices/iterators/accessors of containers, based on their value type

  /** returns indices to vec, if vec was sorted
   * @param vec the input vector
   * @return vector of indices
   */
  template < class VALUE_T >
  inline std::vector< size_t > get_sorted_index_vector( const std::vector< VALUE_T >& vec ) {
    std::vector< size_t > ret;
    std::map< VALUE_T, std::vector< size_t > > sort_map;
    for( size_t i = 0; i<vec.size(); ++i ) {
      sort_map[ vec[i] ].push_back(i);
    }
    for( typename std::map< VALUE_T, std::vector< size_t > >::const_iterator it = sort_map.begin(); it != sort_map.end(); ++it ) {
      for( size_t i=0; i<it->second.size(); ++i )
        ret.push_back( it->second[i] );
    }
    return ret;
  }

  /** returns list of KEY_TYPES, if map would get sorted by its VALUE_TYPE
   * @param map the map to get the list of sorted KEY_TYPES
   * @return vector of KEY_TYPES
   */
  template < class KEY_T, class VALUE_T >
  inline std::vector< KEY_T > get_sorted_index_vector( const std::map< KEY_T, VALUE_T >& map ) {
    std::vector< KEY_T > ret;
    std::map< VALUE_T, std::vector< KEY_T > > sort_map;
    for( typename std::map< KEY_T, VALUE_T >::const_iterator it = map.begin(); it != map.end(); ++it ) {
      sort_map[ it->second ].push_back( it->first );
    }
    for( typename std::map< VALUE_T, std::vector< KEY_T > >::const_iterator it = sort_map.begin(); it != sort_map.end(); ++it ) {
      for( size_t i=0; i<it->second.size(); ++i )
        ret.push_back( it->second[i] );
    }
    return ret;
  }


}













#endif

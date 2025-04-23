#ifndef CIRCULAR_MESSAGE_BUFFER_3598804_H
#define CIRCULAR_MESSAGE_BUFFER_3598804_H

#include <boost/circular_buffer.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <string.h>
#include <iostream>

/** Template class for a circular message buffer with custom DeepCopy and Destructor functions.
 * This class assumes carmen-type messages, the messages will get ordered by their timestamp field.
 * Specialized deep copy as well as destructor functions are supported, since carmen messages generally
 * do not provide valid C++ object-oriented interfaces.
 * @author Artur Koch
 */
template < class T >
class CircularMessageBuffer {
  public:
    typedef typename boost::circular_buffer< T >::size_type size_type;
    typedef typename boost::circular_buffer< T >::iterator  iterator;
    /** Deep copy method-type for template type T (primitive fields are copied implicitly)
     * @param destination message pointer, i.e. pointer to the new message object
     * @param source message pointer, i.e. pointer to the original message object
     * @see defaultDeepCopy
     */
    typedef void (*DeepCopyFunctionType)( T*, const T* );
    /** Destructor/Deallocator-function-type for template type T, necessary for dynamically allocated fields in message
     * @param pointer to the message to be destroyed (i.e. this-pointer to the object to be destroyed)
     * @see defaultDestructor
     */
    typedef void (*DestructorFunctionType)( T* );


    CircularMessageBuffer( const bool appendBack = true );
    CircularMessageBuffer( size_type max_size,
                           const bool appendBack = true,
                           DeepCopyFunctionType fptr_deepCopy = &CircularMessageBuffer<T>::defaultDeepCopy,
                           DestructorFunctionType fptr_destructor = &CircularMessageBuffer<T>::defaultDestructor );
    ~CircularMessageBuffer();

    void clear();
    void reset();

    void setDeepCopyFunction( DeepCopyFunctionType fptr );
    void setDeallocFunction( DestructorFunctionType fptr );

    void initialize( size_type max_size,
                     const bool appendBack = true,
                     DeepCopyFunctionType fptr_deepCopy = &CircularMessageBuffer<T>::defaultDeepCopy,
                     DestructorFunctionType fptr_destructor = &CircularMessageBuffer<T>::defaultDestructor );

    size_type size() const { return m_data.size(); }
    size_type capacity() const { return m_data.capacity(); }
    bool full() const { return m_data.full(); }
    bool empty() const { return m_data.empty(); }

    /** Sets maximum size that the buffer can hold and removes exceeding elements if necessary
     */
    void setMaxSize( const size_type max_size );

    /** Defines whether new elements will be appended to the front or back of the buffer
     * @param appendBack true for appending at the back, false for front
     */
    void setDirection( const bool appendBack );
    bool getDirection() const { return m_appendBack; }

    /** Get latest message regarding the ordering
     * @return reference to latest message
     */
    const T& getLatestMessage() const;
    T& getLatestMessage();

    /** Get i-th element in the buffer, disregards ordering direction -> raw data-buffer access
     * @param index the index of the element
     * @return reference to i-th element
     */
    const T& operator[]( size_type index ) const { return m_data[index]; }
    T& operator[]( size_type index ) { return m_data[index]; }

    /** Get i-th element in the buffer, in regard to the order (direction) of the buffer
     * @param index index in [front,back] ~ [0,size()-1) of the element in order
     * @return reference to i-th ordered element
     */
    const T& getOrderedElement( size_type index ) const { return m_appendBack ? m_data[index] : m_data[size() - 1 - index]; }
    T& getOrderedElement( size_type index ) { return m_appendBack ? m_data[index] : m_data[size() - 1 - index]; }

    /** Get element at the back of the buffer
     * @return returns last element for appendBack, first element otherwise
     */
    const T& back() const { return m_appendBack ? m_data.back() : m_data.front(); }
    T& back() { return m_appendBack ? m_data.back() : m_data.front(); }

    /** Get element at the front of the buffer
     * @return returns first element for appendBack, last element otherwise
     */
    const T& front() const { return !m_appendBack ? m_data.back() : m_data.front(); }
    T& front() { return !m_appendBack ? m_data.back() : m_data.front(); }

//    const T& getMessageOfTimestamp( const double& timestamp ) const;
//    T& getMessageOfTimestamp( const double& timestamp );

    /** Add message to the buffer
     * The new message gets deep-copied. If an old message goes out of scope, the provided
     * destructor gets called on the message to be deleted. Additionally, a strong ordering
     * regarding the timestamp-field of the messages is guaranteed. Since messages get added in
     * respect to their timestamp, a message might not get added if it is actually too old.
     * @param message the message to be added
     * @return true if a new message was added, false otherwise
     */
    bool addMessage( const T& message );

    /** Find indices to nearest neighbor messages (regarding the timestamp)
     *  @param timestamp The timestamp to be searched for
     *  @param lower_index Outputs the lower nearest neighbor index, data.size() if invalid
     *  @param upper_index Outputs the upper nearest neighbor index, data.size() if invalid
     *  @param fraction Outputs the fraction (if supplied, i.e. != NULL) for linear interpolation
     *  @return true when found, false else
     */
    bool findNNIndicesOfTimestamp( double timestamp, size_type& lower_index, size_type& upper_index, double *fraction = NULL ) const;

    boost::circular_buffer< T >& getData() { return m_data; }
    const boost::circular_buffer< T >& getData() const { return m_data; }

    void pop_front();
    void pop_back();

    iterator erase( iterator it );

  protected:
    /** Default deep copy: every message-object should supply a ((char*)host)-field that needs to be allocated and freed */
    static void defaultDeepCopy( T* pDestMessage, const T* pSrcMessage );
    /** Default destructor: every message-object should supply a ((char*)host)-field that needs to be freed */
    static void defaultDestructor( T* pMessage );

  protected:
    bool                            m_appendBack;
    boost::circular_buffer< T >     m_data;
    DeepCopyFunctionType            m_fpDeepCopy;
    DestructorFunctionType          m_fpDestructor;
};


template< class T >
inline CircularMessageBuffer<T>::CircularMessageBuffer( const bool appendBack /* = true */ )
{
  initialize( 0, appendBack );
}

template< class T >
inline CircularMessageBuffer<T>::CircularMessageBuffer( const size_type max_size,
                                                        const bool appendBack /* = true */,
                                                        DeepCopyFunctionType fptr_deepCopy /* = &CircularMessageBuffer<T>::defaultDeepCopy*/,
                                                        DestructorFunctionType fptr_destructor /* = &CircularMessageBuffer<T>::defaultDestructor*/ )
{
  initialize( max_size, appendBack, fptr_deepCopy, fptr_destructor );
}


template< class T >
inline void CircularMessageBuffer<T>::initialize( const size_type max_size,
                                                  const bool appendBack /* = true */,
                                                  DeepCopyFunctionType fptr_deepCopy /* = &CircularMessageBuffer<T>::defaultDeepCopy*/,
                                                  DestructorFunctionType fptr_destructor /* = &CircularMessageBuffer<T>::defaultDestructor*/ )
{
  reset();
  m_appendBack = appendBack;
  setMaxSize( max_size );
  setDeepCopyFunction( fptr_deepCopy );
  setDeallocFunction( fptr_destructor );
}


template< class T >
inline CircularMessageBuffer<T>::~CircularMessageBuffer()
{
  if( m_fpDestructor != NULL ) {
    for(size_type i=0; i<m_data.size(); ++i) {
      (*m_fpDestructor)( &(m_data[i]) );
    }
  }
}


template< class T >
inline void CircularMessageBuffer<T>::pop_front() {
  if( empty() )
    return;
  if( m_fpDestructor != NULL )
    (*m_fpDestructor)( &(front()) );
  if( m_appendBack )
    m_data.pop_front();
  else
    m_data.pop_back();
}


template< class T >
inline void CircularMessageBuffer<T>::pop_back() {
  if( empty() )
    return;
  if( m_fpDestructor != NULL )
    (*m_fpDestructor)( &(back()) );
  if( m_appendBack )
    m_data.pop_back();
  else
    m_data.pop_front();
}


template< class T >
inline void CircularMessageBuffer<T>::setMaxSize( const size_type max_size )
{
  if( !empty() && m_fpDestructor != NULL && m_data.size() > max_size ) {
    while( m_data.size() > max_size ) {
      pop_front();
    }
  }
  m_data.set_capacity( max_size );
}


template< class T >
typename CircularMessageBuffer<T>::iterator CircularMessageBuffer<T>::
erase( iterator it ) {
  if( m_fpDestructor != NULL ) {
    (*m_fpDestructor)( &(*it) );
  }
  return m_data.erase( it );
}


template< class T >
inline const T& CircularMessageBuffer<T>::getLatestMessage() const
{
  if( m_appendBack )
    return m_data.back();
  else
    return m_data.front();
}


template< class T >
inline T& CircularMessageBuffer<T>::getLatestMessage()
{
  if( m_appendBack )
    return m_data.back();
  else
    return m_data.front();
}


//template< class T >
//inline const T& CircularMessageBuffer<T>::getMessageOfTimestamp( const double& timestamp ) const
//{
//  // TODO ...
//  double frac;
//  size_type lower, upper;
//  //findNNIndicesOfTimestamp(  )
//}


//template< class T >
//inline T& CircularMessageBuffer<T>::getMessageOfTimestamp( const double& timestamp )
//{
//}



template< class T >
inline bool CircularMessageBuffer<T>::findNNIndicesOfTimestamp( double timestamp,
                                                                size_type& lower_index,
                                                                size_type& upper_index,
                                                                double* fraction ) const
{
  bool lower_found = 0, upper_found = 0;
  lower_index = upper_index = 0;

  for ( size_type i = 0; i < m_data.size(); ++i )
  {
    // Check for (better) infimum
    if ( m_data[i].timestamp <= timestamp )
    {
      if ( !lower_found || m_data[i].timestamp > m_data[lower_index].timestamp )
      {
        lower_index = i;
        lower_found = true;
      }
    }

    // Check for (better) supremum
    if ( m_data[i].timestamp >= timestamp )
    {
      if ( !upper_found || m_data[i].timestamp < m_data[upper_index].timestamp )
      {
        upper_index = i;
        upper_found = true;
      }
    }
  }

  if ( lower_found && upper_found )
  {
    if( fraction != NULL ) {
      double lower_t = m_data[lower_index].timestamp;
      double upper_t = m_data[upper_index].timestamp;

      if ( lower_t != upper_t )
        *fraction = (timestamp - lower_t) / (upper_t - lower_t);
      else
        *fraction = 0.0; // any value will work fine...
    }
    return 1; // success
  }
  else { // no index bounds found
    if(!lower_found) lower_index = m_data.size();
    if(!upper_found) upper_index = m_data.size();
    return 0;
  }
}


template< class T >
inline bool CircularMessageBuffer<T>::addMessage( const T& message )
{
  // nothing to be done if message is too old!
  if( full() && message.timestamp < front().timestamp ) {
    return false;
  }

  // (1) if a message will go out of scope:
  // check whether destructor of object was supplied and needs to be called explicitly
  // for dynamically allocated fields (messages are not considered to provide destructors)
  if( m_data.full() ) {
    pop_front();
  }

  // (2) check for the ordering on timestamps
  if( empty() || back().timestamp < message.timestamp ) {
    // everything fine, message is newer as the latest in the buffer:
    // push new element into buffer and copy primitive fields, then call deep copy if necessary
    if( m_appendBack )
      m_data.push_back( message );
    else
      m_data.push_front( message );
    // call deep copy explicitly if supplied
    if( m_fpDeepCopy != NULL ) {
      (*m_fpDeepCopy)( &(back()), &message );
    }
  }
  else { // the new message is not the newest, we need to insert it somewhere in (front,back) into the buffer ...
    // find the position inside the buffer and insert the element
    typename boost::circular_buffer< T >::iterator it;
    if( m_appendBack ) {
      // find position
      for( it = m_data.begin(); (it != m_data.end()) && (it->timestamp <= message.timestamp); ++it ) {}
      // copy primitive fields and remember the element
      it = m_data.insert( it, message );
    } else {
      // find position
      for( it = m_data.begin(); (it != m_data.end()) && (it->timestamp >= message.timestamp); ++it ) {}
      // copy primitive fields and remember the element
      it = m_data.rinsert( it, message );
    }
    // call deep copy for the new element
    if( m_fpDeepCopy != NULL ) {
      (*m_fpDeepCopy)( &(*it), &message );
    }
  }

  // new message added to the buffer
  return true;
}

template< class T >
inline void CircularMessageBuffer<T>::setDirection( const bool appendBack )
{
  m_appendBack = appendBack;
}


template< class T >
inline void CircularMessageBuffer<T>::clear()
{
  if( m_fpDestructor != NULL )
    for( size_type i=0; i<m_data.size(); ++i )
      (*m_fpDestructor)( &(m_data[i]) );
  m_data.clear();
}


template< class T >
inline void CircularMessageBuffer<T>::reset()
{
  clear();
  setMaxSize(0);
  m_appendBack = true;
}


template< class T >
inline void CircularMessageBuffer<T>::setDeallocFunction( DestructorFunctionType fptr )
{
  m_fpDestructor = fptr;
}

template< class T >
inline void CircularMessageBuffer<T>::setDeepCopyFunction( DeepCopyFunctionType fptr )
{
  m_fpDeepCopy = fptr;
}

template< class T >
inline void CircularMessageBuffer<T>::defaultDeepCopy( T* pDestMessage, const T* pSrcMessage )
{
  if( pSrcMessage == pDestMessage || pSrcMessage == NULL || pDestMessage == NULL )
    return;
  if( pSrcMessage->host != NULL && strlen( pSrcMessage->host ) != 0 ) {
    pDestMessage->host = (char*) malloc( strlen( pSrcMessage->host ) );
    strcpy( pDestMessage->host, pSrcMessage->host );
  } else {
    pDestMessage->host = NULL;
  }
}

template< class T >
inline void CircularMessageBuffer<T>::defaultDestructor( T* pMessage )
{
  if( pMessage == NULL )
    return;
  if( pMessage->host != NULL )
    free( pMessage->host );
  pMessage->host = NULL;
}

#endif

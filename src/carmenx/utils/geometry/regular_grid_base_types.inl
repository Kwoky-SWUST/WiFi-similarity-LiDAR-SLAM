// specialization of the constructor
template <>
inline GridCell<double, Vector3d>::GridCell() : m_mean(0.), m_variance(0.) {}

// standard version
template< class TargetValueT, class VectorT >
inline GridCell<TargetValueT, VectorT>::GridCell() {}

template< class TargetValueT, class VectorT >
std::ostream & operator <<( std::ostream& os, const GridSampleValuePosition<TargetValueT, VectorT> & sample ) {
  os << "{ " << sample.value << " @ " << sample.position << " }";
  return os;
}

template< class TargetValueT, class VectorT >
std::ostream & operator <<( std::ostream & os, const GridCell<TargetValueT, VectorT> & grid_cell ) {
  os << "grid cell @ " << grid_cell.m_position << " with " << grid_cell.getSamples().size() << " samples:\n";
  for( int iSample = 0; iSample < grid_cell.getSamples().size(); ++iSample ) {
    os << "  [" << iSample << "] " << grid_cell[iSample] << '\n';
  }
  return os;
}

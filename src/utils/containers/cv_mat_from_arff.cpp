#include <ARFF/src/arff_parser.h>
#include <string>
#include <iostream>

#include "cv_mat_from_arff.h"

CvMatFromArff::CvMatFromArff( const std::string& pathToArffFile, const bool bReadFile /* = true */ )
    : m_filePath(pathToArffFile),
      m_pParser(NULL),
      m_pArffData(NULL)
{
  m_numSamples = m_numClasses = m_numAttributes = 0;
  if(bReadFile) readFile();
}

CvMatFromArff::~CvMatFromArff() {
  if( m_pParser != NULL )
    delete m_pParser;
}

void CvMatFromArff::readFile() {
  std::cout << "reading file \"" << m_filePath << "\" ... " << std::flush;

  if( m_pParser != NULL ) {
    delete m_pParser;
    m_pArffData = NULL;
  }
  m_pParser = new ArffParser( m_filePath );
  m_pArffData = m_pParser->parse();

  // initialize internal data fields from arff data (e.g number of samples, classes and attributes)
  m_numSamples = m_pArffData->num_instances();
  m_numAttributes = m_pArffData->num_attributes() - 1;
  for( int i=0; i<m_pArffData->num_attributes(); ++i ) {
    if( icompare( m_pArffData->get_attr( i )->name(), "class" ) ) {
      const ArffAttr* classAttrib = m_pArffData->get_attr( i );
      assert( classAttrib->type() == NOMINAL );
      m_classesAsStrings = m_pArffData->get_nominal( classAttrib->name() );
      m_numClasses = m_classesAsStrings.size();
      m_classesAsFloats.clear();
      for( unsigned int iClass = 0; iClass < m_numClasses; ++iClass ) {
        float classNumber;
        str2num<float>( m_classesAsStrings[iClass], classNumber );
        m_classesAsFloats.push_back( classNumber );
      }
    }
  }

  std::cout << "done!" << std::endl;

  // generate data matrices
  generateMatrices();

}

void CvMatFromArff::readFile( const std::string& pathToArffFile ) {
  setFileName( pathToArffFile );
  readFile();
}

void CvMatFromArff::generateMatrices() {
  std::cout << "generating matrices ... " << std::flush;

  if( m_pParser == NULL || m_pArffData == NULL ) {
    std::cout << "ERROR: Data not loaded or corrupt file!" << std::endl;
    return;
  }

  m_sampleMatrix = cv::Mat( m_numSamples, m_numAttributes, CV_32FC1 );
  m_classMatrix = cv::Mat( m_numSamples, 1, CV_32FC1 );
  for( unsigned int iRow = 0; iRow < m_numSamples; ++iRow ) {
    // fill sample data
    for( unsigned int iCol = 0; iCol < m_numAttributes; ++iCol ) {
      m_sampleMatrix.at< float >( iRow, iCol ) = float(*m_pArffData->get_instance( iRow )->get( iCol ));
    }
    // fill class label (i.e. numerical values!) data
    float classNumber;
    str2num<float>( std::string(*m_pArffData->get_instance( iRow )->get( m_numAttributes )), classNumber );
    m_classMatrix.at< float >( iRow, 0 ) = classNumber;
  }

  std::cout << "done: "
            << "created sample matrix (" << m_sampleMatrix.rows << "x" << m_sampleMatrix.cols
            << ") and class matrix (" << m_classMatrix.rows << "x" << m_classMatrix.cols << ")" << std::endl;
}


void CvMatFromArff::getSubmatrices( cv::Mat& Msamples, cv::Mat& Mclasses,
                                    unsigned int rowStart, unsigned int rowEnd,
                                    unsigned int colStart /* = 0*/, unsigned int colEnd /* = 0*/ )
{
  // clamp ranges
  if( rowEnd > m_sampleMatrix.rows )
    rowEnd = m_sampleMatrix.rows;
  if( colEnd > m_sampleMatrix.cols )
    colEnd = m_sampleMatrix.cols;
  const cv::Range rowRange( rowStart, rowEnd );
  cv::Range colRange( cv::Range::all() );
  if( colEnd > colStart ) {
    colRange = cv::Range( colStart, colEnd );
  }
  // extract and assign the submatrices
  Msamples = cv::Mat( m_sampleMatrix, rowRange, colRange );
  Mclasses = cv::Mat( m_classMatrix, rowRange );
}

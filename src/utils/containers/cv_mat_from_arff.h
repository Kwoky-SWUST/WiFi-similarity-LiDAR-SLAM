#ifndef CV_MAT_FROM_ARFF_9283490382_H
#define CV_MAT_FROM_ARFF_9283490382_H

#include <vector>
#include <cv.h>
#include <arff_data.h>

class ArffParser;
class ArffData;
class string;

class CvMatFromArff {
  public:
    /** Constructor, reads file if not specified differently
     * @param pathToArffFile path to the ARFF-file to be read
     * @param bReadFile flag indicating whether the file should be read in constructor
     */
    CvMatFromArff( const std::string& pathToArffFile, const bool bReadFile = true );
    ~CvMatFromArff();

    void setFileName( const std::string& pathToArffFile ) { m_filePath = pathToArffFile; }
    std::string getFileName() const { return m_filePath; }

    /** read the previously specified file
     */
    void readFile();
    void readFile( const std::string& pathToArffFile );

    /** generate matrices from previously read ARFF data
     */
    void generateMatrices();

    /** get sub matrices out of the sample data, e.g. useful for cross-validation
     *  the returned matrices are of size (rowEnd-1)
     * @param Msamples returned submatrix of samples
     * @param Mclasses returned submatrix of classes
     * @param rowStart index of first row (inclusive)
     * @param rowEnd   index of last+1 row (exclusive)
     * @param colStart index of first column (inclusive) -- only used for sample matrix!
     * @param colEnd   index of last+1 column (exclusive) -- only used for sample matrix!
     */
    void getSubmatrices( cv::Mat& Msamples, cv::Mat& Mclasses,
                         unsigned int rowStart, unsigned int rowEnd,
                         unsigned int colStart = 0, unsigned int colEnd = 0 );

    /// DATA:
    std::string           m_filePath;
    ArffParser*           m_pParser;
    ArffData*             m_pArffData;

    unsigned int          m_numSamples;
    unsigned int          m_numAttributes;
    unsigned int          m_numClasses;

    ArffNominal           m_classesAsStrings;
    std::vector<float>    m_classesAsFloats;

    cv::Mat               m_sampleMatrix;
    cv::Mat               m_classMatrix;
};


#endif

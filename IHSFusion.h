#ifndef IHSFUSION_H
#define IHSFUSION_H

//#include <QString>
#include <QString>
#include <functional>

#include <opencv2/opencv.hpp>

#include "gdal_priv.h"
#include "gdalwarper.h"
#include "gdal_alg.h"

class IHSFusion
{
public:

    IHSFusion();
    ~IHSFusion();

    //------------------------------------
    // Main Functions
    //------------------------------------

    bool openInputs(const QString &hhFile,
                    const QString &nirFile,

                    const QString &redFile);

    bool createOutput(const QString &outputFile);

    bool computeGlobalStatistics();

    bool processFullImage();

    bool processTiles();

    bool computeHistogramMatchingLUT();

    void close();

    //------------------------------------
    // Progress Callback
    //------------------------------------

    std::function<void(int)> progressCallback;

private:

    //------------------------------------
    // GDAL Datasets
    //------------------------------------

    GDALDataset *m_redDataset;
    GDALDataset *m_nirDataset;
    GDALDataset *m_hhDataset;
    GDALDataset *m_hhResampledDataset;
    GDALDataset *m_outputDataset;
    GDALDataset *m_redResampledDataset;
    GDALDataset *m_nirResampledDataset;

    // Normalizes a RED/NIR band using ONLY the pixels inside the
    // common-area crop window (offsetX/offsetY/cropCols/cropRows,
    // expressed at that band's OWN native resolution), via the
    // classic (pixel - min) / (max - min) formula, UNCLAMPED, then
    // resamples (nearest-neighbour) the normalized crop onto the HH
    // reference grid. outMin/outMax report the min/max actually used.
    bool normalizeAndResampleOptical(GDALDataset* srcDataset, GDALDataset*& dstDataset,
                                      int offsetX, int offsetY, int cropCols, int cropRows,
                                      double &outMin, double &outMax);

    // Normalizes HH directly in the dB domain (no dB->linear
    // conversion), using ONLY the pixels inside the common-area crop
    // (m_hhOffsetX/Y, m_commonCols/Rows), via
    // (pixel - min) / (max - min), UNCLAMPED. Result is written to
    // m_hhResampledDataset (already at the HH/reference resolution,
    // so no warp/resample is needed for HH).
    bool normalizeSARCommonArea();

    bool computePercentileMinMax(GDALDataset* dataset, double lowerPct, double upperPct,
                                  double &outMin, double &outMax);

    //------------------------------------
    // Image Information
    //------------------------------------

    int m_rows;
    int m_cols;

    double m_geoTransform[6];

    QString m_projection;

    //------------------------------------
    // Statistics
    //------------------------------------

    double m_redMin;
    double m_redMax;

    double m_nirMin;
    double m_nirMax;

    double m_hhMin;
    double m_hhMax;

    //------------------------------------
    // Helper Functions
    //------------------------------------

    bool readTile(GDALRasterBand *band,
                             int x,
                             int y,
                             int width,
                             int height,
                             cv::Mat &tile);

    bool writeTile(GDALRasterBand *band,
                              int x,
                              int y,
                              const cv::Mat &tile);

    //------------------------------------
    // Normalization
    //------------------------------------

    void normalizeOpticalBand(const cv::Mat& src,
                              cv::Mat& dst,
                              double minVal,
                              double maxVal);

    // No longer called anywhere in the pipeline (HH is normalized
    // directly in dB now, see normalizeSARCommonArea()). Kept only
    // for interface/back-compat purposes.
    void normalizeSARBand(const cv::Mat& src,
                          cv::Mat& dst,
                          double minVal,
                          double maxVal);

    // No longer called anywhere in the pipeline (HH is normalized
    // directly in dB now, no linear conversion). Kept only for
    // interface/back-compat purposes.
    void convertSARDBToLinear(const cv::Mat& src,
                              cv::Mat& dst);

    //------------------------------------
    // IHS
    //------------------------------------

    void rgbToIHS(const cv::Mat& R,
                  const cv::Mat& G,
                  const cv::Mat& B,
                  cv::Mat& I,
                  cv::Mat& H,
                  cv::Mat& S);

    void ihsToRGB(const cv::Mat& I,
                  const cv::Mat& H,
                  const cv::Mat& S,
                  cv::Mat& HH,
                  cv::Mat& NIR,
                  cv::Mat& R);

    //------------------------------------
    // Resampling
    //------------------------------------

    bool resampleHHToReference();

    //------------------------------------
    // Image Buffers
    //------------------------------------

    cv::Mat m_red;
    cv::Mat m_nir;
    cv::Mat m_hh;

    cv::Mat m_intensity;
    cv::Mat m_hue;
    cv::Mat m_saturation;

    cv::Mat m_fusedRed;
    cv::Mat m_fusednir;
    cv::Mat m_fusedHH;

    //------------------------------------
    // Common Extent
    //------------------------------------

    int m_commonRows;
    int m_commonCols;

    double m_commonGeoTransform[6];

    int m_redOffsetX;
    int m_redOffsetY;

    int m_nirOffsetX;
    int m_nirOffsetY;

    int m_hhOffsetX;
    int m_hhOffsetY;

    // RED/NIR crop size within the common area, expressed at each
    // band's OWN native resolution (e.g. 10 m) - computed in
    // computeCommonExtent() BEFORE any resampling onto the HH grid.
    int m_redCropCols;
    int m_redCropRows;

    int m_nirCropCols;
    int m_nirCropRows;

    bool computeCommonExtent();

    //histo

    void applyHistogramMatch(const cv::Mat &src, cv::Mat &dst);

    std::vector<float> m_hhLUT;
    float m_hhLUTMin, m_hhLUTMax;


private:
    static constexpr int TILE_SIZE = 512;
};

#endif

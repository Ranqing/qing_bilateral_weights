#ifndef QING_GET_BILATERAL_WEIGHTS_H
#define QING_GET_BILATERAL_WEIGHTS_H

#include "qing_common.h"
//#include "../../Qing/qing_bilateral_filter.h"

#define QING_FILTER_INTENSITY_RANGE 256
#define QING_FILTER_SPATIAL_RANGE   256

inline float * qing_get_range_weighted_table(float sigma_range, int len) {
    float * table_range, * range_table_x;
    table_range = new float[len];
    range_table_x = &table_range[0];
    for(int y = 0; y<len; ++y) {
        *range_table_x++ = exp(-double(y*y)/(2*sigma_range*sigma_range));
    }
    return table_range;
}

inline float * qing_get_spatial_weighted_table(float sigma_spatial, int len) {
    float * table_spatial, * spatial_table_x;
    table_spatial = new float[len];
    spatial_table_x = &table_spatial[0];
    for(int y = 0; y < len; ++y) {
        *spatial_table_x++ = exp(-double(y*y)/(2*sigma_spatial*sigma_spatial));
    }
    return table_spatial;
}

class qing_get_bilateral_weights
{
public:
    qing_get_bilateral_weights();
    ~qing_get_bilateral_weights();

    Mat m_bgr, m_weights;
    int m_w, m_h, m_ch, m_patch_size;
    float m_sigma_range, m_sigma_spatial;
    float * m_range_table, * m_spatial_table;

    void read_in_bgr(const string filename);
    void init_params(float sigma_range, float sigma_spatial, int patch_size);
    void get_weighted_table();
    void calc_bilateral_weights();
    void test();

    Mat get_bilateral_weights();

};

inline Mat qing_get_bilateral_weights::get_bilateral_weights() {
    return m_weights;
}

#endif // QING_GET_BILATERAL_WEIGHTS_H

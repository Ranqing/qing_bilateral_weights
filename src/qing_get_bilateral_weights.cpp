#include "qing_get_bilateral_weights.h"

qing_get_bilateral_weights::qing_get_bilateral_weights()
{
}

qing_get_bilateral_weights::~qing_get_bilateral_weights() {

}

void qing_get_bilateral_weights::read_in_bgr(const string filename) {

    m_bgr = imread(filename, CV_LOAD_IMAGE_UNCHANGED);
    if(NULL == m_bgr.data) {
        cerr << "failed to open " << filename << endl;
        exit(1);
    }

    m_h = m_bgr.size().height;
    m_w = m_bgr.size().width;
    m_ch = 3;
}

void qing_get_bilateral_weights::init_params(float sigma_range, float sigma_spatial, int patch_size) {
    m_sigma_range = sigma_range * QING_FILTER_INTENSITY_RANGE;
    m_sigma_spatial = sigma_spatial * min(m_w, m_h);
    m_patch_size = patch_size;
    //cout << "patch = " << patch_size << endl;
}

void qing_get_bilateral_weights::get_weighted_table() {
    m_range_table = qing_get_range_weighted_table(m_sigma_range, QING_FILTER_INTENSITY_RANGE);
    m_spatial_table = qing_get_spatial_weighted_table(m_sigma_spatial, QING_FILTER_SPATIAL_RANGE);
}

void qing_get_bilateral_weights::calc_bilateral_weights() {

    int patch2 = m_patch_size * m_patch_size;
    m_weights = Mat::zeros(m_h, m_w, CV_32FC(patch2));

    unsigned char * bgr_data = m_bgr.ptr<unsigned char>(0), *p_bgr_data, *q_bgr_data;
    unsigned char p_b, p_g, p_r;
    float * weight_data = m_weights.ptr<float>(0), * p_weight_data, weight;

    int idx, cur_x, cur_y, cur_idx;
    int delta_b, delta_g, delta_r, delta_bgr;
    int offset = m_patch_size * 0.5;

    for(int y = 0; y < m_h; ++y) {
        for(int x = 0; x < m_w; ++x) {
            idx = y * m_w + x;

            p_weight_data = weight_data + idx * patch2 - 1;
            p_bgr_data = bgr_data + idx * 3 - 1;
            p_b = *(++p_bgr_data);
            p_g = *(++p_bgr_data);
            p_r = *(++p_bgr_data);

            for(int dy = -offset; dy <= offset; ++dy) {
                cur_y = (y + dy + m_h) % m_h;
                for(int dx = -offset; dx <= offset; ++dx) {
                    cur_x = (x + dx + m_w) % m_w;
                    cur_idx = cur_y * m_w + cur_x;

                    q_bgr_data = bgr_data + cur_idx * 3 - 1;
                    delta_b = abs(p_b - *(++q_bgr_data));
                    delta_g = abs(p_g - *(++q_bgr_data));
                    delta_r = abs(p_r - *(++q_bgr_data));

                    delta_bgr = 0.333333 * (delta_b + delta_g + delta_r);
                    weight = m_range_table[delta_bgr] * m_spatial_table[abs(dy)] * m_spatial_table[abs(dx)] ;
                    (*++p_weight_data) = weight;
                }
            }
        }
    }
}

void qing_get_bilateral_weights::test() {

    int patch2 = m_patch_size * m_patch_size, offset = m_patch_size * 0.5;
    Mat filtered_img = Mat::zeros(m_h, m_w, CV_8UC3);

    unsigned char * f_bgr_data = filtered_img.ptr<unsigned char>(0), * p_f_data;
    unsigned char * bgr_data = m_bgr.ptr<unsigned char>(0), * p_data;
    float * weight_data = m_weights.ptr<float>(0), * p_weight_data;
    double weight, sum_div, sum_b, sum_g, sum_r;

    for(int y = 0; y < m_h; ++y) {
        int idy = y * m_w;
        for(int x = 0; x < m_w; ++x) {
            int idx = idy + x;

            p_weight_data = weight_data + idx * patch2 - 1;
            p_f_data = f_bgr_data + idx * 3 - 1;

            sum_b = 0.0, sum_g = 0.0, sum_r = 0.0, sum_div = 0.0;

            for(int dy = -offset; dy <= offset; ++dy) {
                int cur_y = (y + dy + m_h) % m_h;
                for(int dx = -offset; dx <= offset; ++dx) {
                    int cur_x = (x + dx + m_w) % m_w;
                    int cur_idx = cur_y * m_w + cur_x;

                    p_data = bgr_data + cur_idx * 3 - 1;

                    weight = *++p_weight_data;
                    sum_div += weight;
                    sum_b += weight * (*++p_data) ;
                    sum_g += weight * (*++p_data) ;
                    sum_r += weight * (*++p_data) ;
                }
            }
            (*++p_f_data) = sum_b / sum_div;
            (*++p_f_data) = sum_g / sum_div;
            (*++p_f_data) = sum_r / sum_div;
        }
    }

    imshow("test", m_bgr);
    imshow("filtered_test", filtered_img);
    waitKey(0);
    destroyAllWindows();
}

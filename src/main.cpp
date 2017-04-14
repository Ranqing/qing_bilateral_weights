#include "qing_get_bilateral_weights.h"


int main(int argc, char * argv[]) {
    string infile = "test.png";

    float sigma_range = 0.08;    //0.08 * 256
    float sigma_spatial = 0.03;
    int patch_size = 5;

    qing_get_bilateral_weights test;
    test.read_in_bgr(infile);
    test.init_params(sigma_range, sigma_spatial, patch_size);
    test.get_weighted_table();
    test.calc_bilateral_weights();
    test.test();

    //Mat bilateral_weights = test.get_bilateral_weights();

	return 1;
}

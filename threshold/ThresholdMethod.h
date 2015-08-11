#ifndef _THRESHOLDMETHOD_
#define _THRESHOLDMETHOD_

#include <getopt.h>

#include "../ematrix/EMatrix.h"

void print_threshold_usage();

/**
 * A base class for thresholding methods to inherit from.
 *
 * This class constructor will read arguments from the command-line, and
 * provides the getters and setters for commonly needed class members.
 */
class ThresholdMethod {

  protected:
    // The clustering method used.
    char * clustering;
    // The expression matrix object.
    EMatrix * ematrix;
    // The directory where the expression matrix is found
    char * bin_dir;
    // Specifies the correlation method that was used: pc, mi, sc
    char * method;


    // DATA FILTERS FOR CLUSTERED SIMILARITY DATA
    // ------------------------------------------
    // The maximum number of missing values in the comparision.
    int max_missing;
    // The minimum number of samples in a cluster.
    int min_cluster_size;


  public:
    ThresholdMethod(EMatrix *ematrix, char * method, char * clustering,
        int min_cluster_size, int max_missing);
    ~ThresholdMethod();

    // GETTERS
    // -------
    char * getCorMethod() { return method; }
    int getMaxMissing() { return max_missing; }
    int getMinClusterSize() { return min_cluster_size; }

    // TO BE IMPLEMENTED BY THE CHILD CLASS
    // ------------------------------------
    double findThreshold();
};

#endif
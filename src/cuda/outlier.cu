
// #include "sort.cu"






/*!
 * Remove outliers from a vector of pairwise data. Outliers are detected independently
 * on each axis using the Tukey method, and marked with the given marker. Only the
 * samples in the given cluster are used in outlier detection. For unclustered data,
 * all samples are labeled as 0, so a cluster value of 0 should be used.
 *
 * This function returns the number of clean samples remaining in the data array,
 * including samples in other clusters.
 *
 * @param x
 * @param y
 * @param labels
 * @param sampleSize
 * @param cluster
 * @param marker
 * @param x_sorted
 * @param y_sorted
 */
__device__
int removeOutliersCluster(
   const float *x,
   const float *y,
   char *labels,
   int sampleSize,
   char cluster,
   char marker,
   float *x_sorted,
   float *y_sorted)
{
   // extract samples from the given cluster into separate arrays
   int n = 0;

   for ( int i = 0; i < sampleSize; i++ )
   {
      if ( labels[i] == cluster )
      {
         x_sorted[n] = x[i];
         y_sorted[n] = y[i];
         n++;
      }
   }

   // get power of 2 size
   int N_pow2 = nextPower2(sampleSize);

   for ( int i = n; i < N_pow2; ++i )
   {
      x_sorted[i] = INFINITY;
      y_sorted[i] = INFINITY;
   }

   // return if the given cluster is empty
   if ( n == 0 )
   {
      return 0;
   }

   // sort samples for each axis
   bitonicSort(x_sorted, N_pow2);
   bitonicSort(y_sorted, N_pow2);

   // compute interquartile range and thresholds for each axis
   float Q1_x = x_sorted[n * 1 / 4];
   float Q3_x = x_sorted[n * 3 / 4];
   float T_x_min = Q1_x - 1.5f * (Q3_x - Q1_x);
   float T_x_max = Q3_x + 1.5f * (Q3_x - Q1_x);

   float Q1_y = y_sorted[n * 1 / 4];
   float Q3_y = y_sorted[n * 3 / 4];
   float T_y_min = Q1_y - 1.5f * (Q3_y - Q1_y);
   float T_y_max = Q3_y + 1.5f * (Q3_y - Q1_y);

   // mark outliers
   int numSamples = 0;

   for ( int i = 0; i < sampleSize; i++ )
   {
      // mark samples in the given cluster that are outliers on either axis
      if ( labels[i] == cluster && (x[i] < T_x_min || T_x_max < x[i] || y[i] < T_y_min || T_y_max < y[i]) )
      {
         labels[i] = marker;
      }

      // count the number of remaining samples in the entire data array
      else if ( labels[i] >= 0 )
      {
         numSamples++;
      }
   }

   // return number of remaining samples
   return numSamples;
}






/*!
 * Perform outlier removal on each cluster in a parwise data array.
 *
 * @param globalWorkSize
 * @param expressions
 * @param sampleSize
 * @param in_index
 * @param in_N
 * @param in_labels
 * @param in_K
 * @param marker
 */
__global__
void removeOutliers(
   int globalWorkSize,
   const float *expressions,
   int sampleSize,
   const int2 *in_index,
   int *in_N,
   char *in_labels,
   char *in_K,
   char marker,
   float *work_xy)
{
   int i = blockIdx.x * blockDim.x + threadIdx.x;

   if ( i >= globalWorkSize )
   {
      return;
   }

   // initialize workspace variables
   int N_pow2 = nextPower2(sampleSize);
   int2 index = in_index[i];
   const float *x = &expressions[index.x * sampleSize];
   const float *y = &expressions[index.y * sampleSize];
   int *p_N = &in_N[i];
   char *labels = &in_labels[i * sampleSize];
   char clusterSize = in_K[i];
   float *x_sorted = &work_xy[(2 * i + 0) * N_pow2];
   float *y_sorted = &work_xy[(2 * i + 1) * N_pow2];

   if ( marker == -7 )
   {
      clusterSize = 1;
   }

   // do not perform post-clustering outlier removal if there is only one cluster
   if ( marker == -8 && clusterSize <= 1 )
   {
      return;
   }

   // perform outlier removal on each cluster
   int N;

   for ( char k = 0; k < clusterSize; ++k )
   {
      N = removeOutliersCluster(x, y, labels, sampleSize, k, marker, x_sorted, y_sorted);
   }

   // save number of remaining samples
   *p_N = N;
}

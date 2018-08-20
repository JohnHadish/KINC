#include "pairwise_pearson.h"



using namespace Pairwise;






/*!
 * Compute the Pearson correlation for a cluster of a given dataset.
 *
 * Note that the dataset contains only those samples which were not removed
 * by pre-processing, while the labels contains all samples from the original
 * expression matrix.
 *
 * @param data
 * @param labels
 * @param cluster
 * @param minSamples
 */
float Pearson::computeCluster(
   const QVector<Vector2>& data,
   const QVector<qint8>& labels,
   qint8 cluster,
   int minSamples)
{
   // compute intermediate sums
   int n = 0;
   float sumx = 0;
   float sumy = 0;
   float sumx2 = 0;
   float sumy2 = 0;
   float sumxy = 0;

   for ( int i = 0; i < labels.size(); ++i )
   {
      if ( labels[i] == cluster )
      {
         float x_i = data[n].s[0];
         float y_i = data[n].s[1];

         sumx += x_i;
         sumy += y_i;
         sumx2 += x_i * x_i;
         sumy2 += y_i * y_i;
         sumxy += x_i * y_i;

         ++n;
      }
   }

   // compute correlation only if there are enough samples
   float result = NAN;

   if ( n >= minSamples )
   {
      result = (n*sumxy - sumx*sumy) / sqrt((n*sumx2 - sumx*sumx) * (n*sumy2 - sumy*sumy));
   }

   return result;
}

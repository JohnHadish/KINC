#include <ace/core/metadata.h>

#include "correlationmatrix.h"



using namespace std;
using namespace GenePair;






void CorrelationMatrix::Pair::addCluster(int amount) const
{
   // keep adding a new list of floats for given amount
   while ( amount-- > 0 )
   {
      _correlations.push_back({});
   }
}






QString CorrelationMatrix::Pair::toString() const
{
   // initialize list of strings and iterate through all clusters
   QStringList ret;
   for (const auto& cluster : _correlations)
   {
      // initialize list of strings for cluster and iterate through each correlation
      QStringList clusterStrings;
      for (const auto& correlation : cluster)
      {
         // add correlation value as string
         clusterStrings << QString::number(correlation);
      }

      // join all cluster strings into one string
      ret << clusterStrings.join(',').prepend('(').append(')');
   }

   // join all clusters and return as string
   return ret.join(',');
}






void CorrelationMatrix::Pair::writeCluster(EDataStream& stream, int cluster)
{
   // make sure cluster value is within range
   if ( cluster >= 0 && cluster < _correlations.size() )
   {
      // iterate through each correlation and write to object
      for (const auto& correlation : _correlations.at(cluster))
      {
         stream << correlation;
      }
   }
}






void CorrelationMatrix::Pair::readCluster(const EDataStream& stream, int cluster) const
{
   // make sure cluster value is within range
   if ( cluster >= 0 && cluster < _correlations.size() )
   {
      // clear cluster and reserve for given size
      _correlations[cluster].clear();
      _correlations[cluster].reserve(_cMatrix->_correlationSize);

      // read in number of correlations per cluster and add to list
      for (int i = 0; i < _cMatrix->_correlationSize ;++i)
      {
         float value;
         stream >> value;
         _correlations[cluster].push_back(value);
      }
   }
}






QVariant CorrelationMatrix::headerData(int section, Qt::Orientation orientation, int role) const
{
   // orientation is not used
   Q_UNUSED(orientation);

   // if role is not display return nothing
   if ( role != Qt::DisplayRole )
   {
      return QVariant();
   }

   // get genes metadata and make sure it is an array
   const EMetadata& genes {geneNames()};
   if ( genes.isArray() )
   {
      // make sure section is within limits of gene name array
      if ( section >= 0 && section < genes.toArray()->size() )
      {
         // return gene name
         return genes.toArray()->at(section)->toVariant();
      }
   }

   // no gene found return nothing
   return QVariant();
}






QVariant CorrelationMatrix::data(const QModelIndex& index, int role) const
{
   // if role is not display return nothing
   if ( role != Qt::DisplayRole )
   {
      return QVariant();
   }

   // if row and column are equal return one
   if ( index.row() == index.column() )
   {
      return 1;
   }

   // get constant pair and read in values
   const Pair pair(this);
   int x {index.row()};
   int y {index.column()};
   if ( y > x )
   {
      swap(x,y);
   }
   pair.read({x,y});

   // Return value of gene pair as a string
   return pair.toString();
}






void CorrelationMatrix::initialize(const EMetadata &geneNames, const EMetadata &correlationNames)
{
   // make sure correlation names is an array and is not empty
   if ( !correlationNames.isArray() || correlationNames.toArray()->isEmpty() )
   {
      E_MAKE_EXCEPTION(e);
      e.setTitle(tr("Domain Error"));
      e.setDetails(tr("Correlation names metadata is not an array or empty."));
      throw e;
   }

   // get map of metadata root and make copy of gene and correlation names
   EMetadata::Map* map {meta().toObject()};
   map->insert("correlations",new EMetadata(correlationNames));

   // save correlation size and initialize base class
   _correlationSize = correlationNames.toArray()->size();
   Base::initialize(geneNames,sizeof(float)*_correlationSize,DATA_OFFSET);
}
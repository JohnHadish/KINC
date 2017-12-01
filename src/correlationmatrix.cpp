#include <ace/core/metadata.h>

#include "correlationmatrix.h"



using namespace std;






void CorrelationMatrix::readData()
{
   // read in correlation base data
   CorrelationBase::readData();

   // read in all header values
   stream() >> _geneSize >> _sampleSize >> _correlationSize >> _maxModes;

   // make sure reading worked
   if ( !stream() )
   {
      E_MAKE_EXCEPTION(e);
      e.setTitle(tr("File IO Error"));
      e.setDetails(tr("Failed reading from data object file."));
      throw e;
   }
}






void CorrelationMatrix::finish()
{
   // write out correlation base's and this object's header
   CorrelationBase::finish();
   stream() << _geneSize << _sampleSize << _correlationSize << _maxModes;

   // make sure writing worked
   if ( !stream() )
   {
      E_MAKE_EXCEPTION(e);
      e.setTitle(QObject::tr("File IO Error"));
      e.setDetails(QObject::tr("Failed writing to data object file."));
      throw e;
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

   // get map to metadata root and check if genes key exists
   const EMetadata::Map* map {meta().toObject()};
   if ( map->contains("genes") )
   {
      // get genes metadata and make sure it is an array
      EMetadata* genes {(*map)["genes"]};
      if ( genes->isArray() )
      {
         // make sure section is within limits of gene name array
         if ( section >= 0 && section < genes->toArray()->size() )
         {
            // return gene name
            return genes->toArray()->at(section)->toVariant();
         }
      }
   }

   // no gene found return nothing
   return QVariant();
}






int CorrelationMatrix::rowCount(const QModelIndex& parent) const
{
   // return gene size
   Q_UNUSED(parent);
   return _geneSize;
}






int CorrelationMatrix::columnCount(const QModelIndex& parent) const
{
   // return gene size multiplied by number of modes
   Q_UNUSED(parent);
   return _geneSize*_maxModes*_correlationSize;
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

   // return first correlation value of first mode
   return pair.at(0,0);
}






void CorrelationMatrix::initialize(const EMetadata& geneNames, qint32 sampleSize
                                   , const EMetadata& correlationNames, qint8 maxModes)
{
   // make sure correlation names is an array and is not empty
   if ( !correlationNames.isArray() || correlationNames.toArray()->isEmpty() )
   {
      E_MAKE_EXCEPTION(e);
      e.setTitle(tr("Domain Error"));
      e.setDetails(tr("Correlation names metadata is not an array or empty."));
      throw e;
   }

   // make sure gene names metadata is an array and is not empty
   if ( !geneNames.isArray() || geneNames.toArray()->isEmpty() )
   {
      E_MAKE_EXCEPTION(e);
      e.setTitle(tr("Domain Error"));
      e.setDetails(tr("Gene names metadata is not an array or empty."));
      throw e;
   }

   // get map of metadata root and make copy of gene and correlation names
   EMetadata::Map* map {meta().toObject()};
   map->insert("genes",new EMetadata(geneNames));
   map->insert("correlations",new EMetadata(correlationNames));

   // save gene, sample, correlation sizes and max modes
   _geneSize = geneNames.toArray()->size();
   _sampleSize = sampleSize;
   _correlationSize = correlationNames.toArray()->size();
   _maxModes = maxModes;

   // initialize correlation base class
   CorrelationBase::initialize(_geneSize,sizeof(float)*_maxModes*_correlationSize,DATA_OFFSET);
}






const EMetadata& CorrelationMatrix::getGeneNames() const
{
   // get metadata root and make sure genes key exist
   const EMetadata::Map* map {meta().toObject()};
   if ( !map->contains("genes") )
   {
      E_MAKE_EXCEPTION(e);
      e.setTitle(tr("Null Return Reference"));
      e.setDetails(tr("Requesting reference to gene names when none exists."));
      throw e;
   }

   // return gene names list
   return *(*map)["genes"];
}






void CorrelationMatrix::readPair(Iterator index, float* correlations) const
{
   // find correlation if it exists
   if ( findCorrelation(index) )
   {
      // read in all correlations and make sure reading worked
      stream().read(correlations,sizeof(float)*_maxModes*_correlationSize);
      if ( !stream() )
      {
         E_MAKE_EXCEPTION(e);
         e.setTitle(tr("File IO Error"));
         e.setDetails(tr("Failed reading from data object file."));
         throw e;
      }
   }

   // else correlation does not exist
   else
   {
      // set all correlations for gene pair to NAN
      for (int i = 0; i < _maxModes*_correlationSize ;++i)
      {
         correlations[i] = NAN;
      }
   }
}






void CorrelationMatrix::readPair(qint64 index, float *correlations) const
{
   // find correlation read in all correlations and make sure reading worked
   findCorrelation(index);
   stream().read(correlations,sizeof(float)*_maxModes*_correlationSize);
   if ( !stream() )
   {
      E_MAKE_EXCEPTION(e);
      e.setTitle(tr("File IO Error"));
      e.setDetails(tr("Failed reading from data object file."));
      throw e;
   }
}






void CorrelationMatrix::writePair(Iterator index, const float* correlations)
{
   // write new correlation
   write(index);
   stream().write(correlations,_maxModes*_correlationSize);

   // make sure writing worked
   if ( !stream() )
   {
      E_MAKE_EXCEPTION(e);
      e.setTitle(tr("File IO Error"));
      e.setDetails(tr("Failed reading from data object file."));
      throw e;
   }
}






CorrelationMatrix::Pair::Pair(CorrelationMatrix* matrix):
   _matrix(matrix),
   _cMatrix(matrix)
{
   // make sure matrix pointer is valid
   if ( !matrix )
   {
      E_MAKE_EXCEPTION(e);
      e.setTitle(tr("Domain Error"));
      e.setDetails(tr("Attempting to create gene pair with null correlation matrix pointer."));
      throw e;
   }

   // create new array for correlation values
   _correlations = new float[_matrix->_maxModes*_matrix->_correlationSize];
}






CorrelationMatrix::Pair::Pair(const CorrelationMatrix* matrix):
   _cMatrix(matrix)
{
   // make sure matrix pointer is valid
   if ( !matrix )
   {
      E_MAKE_EXCEPTION(e);
      e.setTitle(tr("Domain Error"));
      e.setDetails(tr("Attempting to create gene pair with null correlation matrix pointer."));
      throw e;
   }

   // create new array for correlation values
   _correlations = new float[_cMatrix->_maxModes*_cMatrix->_correlationSize];
}






void CorrelationMatrix::Pair::readFirst() const
{
   // set index to beginning and read in correlations
   _index = 0;
   _cMatrix->readPair(_index,_correlations);
}






bool CorrelationMatrix::Pair::readNext() const
{
   // check if there are any more correlations to read
   if ( _index < (_cMatrix->correlationSize() - 1) )
   {
      // read in correlations of next correlation and return true
      _cMatrix->readPair(++_index,_correlations);
      return true;
   }

   // no more correlations to read return false
   return false;
}






float& CorrelationMatrix::Pair::at(int mode, int correlation)
{
   // make sure mode and index are valid
   if ( mode < 0 || mode > _matrix->_maxModes || correlation < 0
        || correlation > _matrix->_correlationSize )
   {
      E_MAKE_EXCEPTION(e);
      e.setTitle(tr("Domain Error"));
      e.setDetails(tr("Invalid mode of %1 and index of %2 with max modes of %3 and correlation "
                      "size of %4").arg(mode).arg(correlation).arg(_matrix->_maxModes)
                   .arg(_matrix->_correlationSize));
      throw e;
   }

   // return correlation value
   return _correlations[mode*_matrix->_correlationSize + correlation];
}






const float& CorrelationMatrix::Pair::at(int mode, int correlation) const
{
   // make sure mode and index are valid
   if ( mode < 0 || mode > _cMatrix->_maxModes || correlation < 0
        || correlation > _cMatrix->_correlationSize )
   {
      E_MAKE_EXCEPTION(e);
      e.setTitle(tr("Domain Error"));
      e.setDetails(tr("Invalid mode of %1 and index of %2 with max modes of %3 and correlation "
                      "size of %4").arg(mode).arg(correlation).arg(_cMatrix->_maxModes)
                   .arg(_cMatrix->_correlationSize));
      throw e;
   }

   // return correlation value
   return _correlations[mode*_cMatrix->_correlationSize + correlation];
}

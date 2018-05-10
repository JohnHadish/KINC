#ifndef RMT_H
#define RMT_H
#include <ace/core/AceCore.h>



class CorrelationMatrix;



class RMT : public EAbstractAnalytic
{
public:
   enum Arguments
   {
      InputData = 0
      ,LogFile
      ,ThresholdStart
      ,ThresholdStep
      ,ThresholdStop
      ,MinUnfoldingPace
      ,MaxUnfoldingPace
      ,Total
   };

   virtual int getArgumentCount() override final { return Total; }
   virtual ArgumentType getArgumentData(int argument) override final;
   virtual QVariant getArgumentData(int argument, Role role) override final;
   virtual void setArgument(int argument, QVariant value) override final;
   virtual void setArgument(int argument, QFile* file) override final;
   virtual void setArgument(int argument, EAbstractData* data) override final;
   virtual quint32 getCapabilities() const override final { return Capabilities::Serial; }
   virtual bool initialize() override final;
   virtual void runSerial() override final;

private:
   void computeGeneThresholds();
   QVector<double> computePruneMatrix(float threshold, int* size);
   QVector<float> computeEigenvalues(QVector<double>* pruneMatrix, int size);
   float computeChiSquare(QVector<float>* eigens);
   float computePaceChiSquare(const QVector<float>& eigens, int pace);
   void degenerate(QVector<float>* eigens);
   QVector<float> unfold(const QVector<float>& eigens, int pace);

   CorrelationMatrix* _input {nullptr};
   QFile* _logfile {nullptr};
   float _thresholdStart {0.99};
   float _thresholdStep {0.001};
   float _thresholdStop {0.5};
   float _chiSquareThreshold1 {99.607};
   float _chiSquareThreshold2 {200};
   int _minEigenvalueSize {50};
   int _minUnfoldingPace {10};
   int _maxUnfoldingPace {40};
   float _chiSquareBinSize {0.05};
   int _maxClusterSize {5};
   QVector<float> _maxCorrelations;
};



#endif

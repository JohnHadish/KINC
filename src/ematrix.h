#ifndef EMATRIX_H
#define EMATRIX_H
#include <ace.h>
#include <fstream>
#include <vector>



namespace ExprMatrixData {

using namespace AccelCompEng;

struct Header : public FileMem::Static<33>
{
   using FPtr = FileMem::Ptr;
   using FileMem::Static<33>::Static;
   uint32_t& sampleSize() { get<uint32_t>(0); }
   const uint32_t& sampleSize() const { get<uint32_t>(0); }
   uint32_t& geneSize() { get<uint32_t>(4); }
   const uint32_t& geneSize() const { get<uint32_t>(4); }
   uint8_t& transform() { get<uint8_t>(8); }
   FPtr& samplePtr() { get<FPtr>(9); }
   const FPtr& samplePtr() const { get<FPtr>(9); }
   FPtr& genePtr() { get<FPtr>(17); }
   const FPtr& genePtr() const { get<FPtr>(17); }
   FPtr& expPtr() { get<FPtr>(25); }
   const FPtr& expPtr() const { get<FPtr>(25); }
};

struct SampleHdr : public FileMem::Static<8>
{
   using FPtr = FileMem::Ptr;
   using FileMem::Static<8>::Static;
   FPtr& name() { get<FPtr>(0); }
};

struct GeneHdr : public FileMem::Static<8>
{
   using FPtr = FileMem::Ptr;
   using FileMem::Static<8>::Static;
   FPtr& name() { get<FPtr>(0); }
};

struct Expression : public FileMem::Static<4>
{
   using FileMem::Static<4>::Static;
   float& val() { get<float>(0); }
};

struct Expressions : public FileMem::Object
{
   using FPtr = FileMem::Ptr;
   Expressions(int amt, FPtr ptr = FileMem::nullPtr): Object(4*amt,ptr) {}
   float& val(int n) { get<float>(4*n); }
   const float& val(int n) const { get<float>(4*n); }
};

}



class EMatrix : public AccelCompEng::DataPlugin
{
public:
   // *
   // * ENUMERATIONS
   // *
   enum Transform { none=0,log,log2,log10 };
   // *
   // * DECLERATIONS
   // *
   using Hdr = ExprMatrixData::Header;
   using gHdr = ExprMatrixData::GeneHdr;
   using sHdr = ExprMatrixData::SampleHdr;
   using Exp = ExprMatrixData::Expression;
   using Exps = ExprMatrixData::Expressions;
   using GetOpts = AccelCompEng::GetOpts;
   using Terminal = AccelCompEng::Terminal;
   using FileMem = AccelCompEng::FileMem;
   using FileSync = AccelCompEng::FileSync;
   using KincFile = AccelCompEng::File;
   using FString = AccelCompEng::FString;
   //using ace = AccelCompEng;
   using string = std::string;
   using ifile = std::ifstream;
   // *
   // * EXCEPTIONS
   // *
   ACE_EXCEPTION(EMatrix,NotNewFile)
   ACE_EXCEPTION(EMatrix,CannotOpen)
   ACE_EXCEPTION(EMatrix,InvalidFile)
   ACE_EXCEPTION(EMatrix,InvalidArg)
   ACE_EXCEPTION(EMatrix,OutOfRange)
   ACE_EXCEPTION(EMatrix,BufferNotLoaded)
   // *
   // * BASIC METHODS
   // *
   EMatrix(const string& type, const string& file);
   // *
   // * VIRTUAL FUNCTIONS
   // *
   void load(GetOpts &ops, Terminal &tm) override final;
   void dump(GetOpts &ops, Terminal &tm) override final;
   void query(GetOpts &ops, Terminal &tm) override final;
   bool empty() override final;
   // *
   // * FUNCTIONS
   // *
   int sample_size() const;
   int gene_size() const;
   string sample_name(int) const;
   string gene_name(int) const;
   void load_buffer();
   void clear_buffer();
   const float* gene(int) const;
private:
   // *
   // * FUNCTIONS
   // *
   void lookup(GetOpts&,Terminal&);
   void load_samples(Terminal&,ifile&);
   void load_genes(Terminal&,ifile&,string,Transform);
   // *
   // * VARIABLES
   // *
   Hdr _hdr;
   FileMem& _mem;
   Exps* _data;
};



#endif

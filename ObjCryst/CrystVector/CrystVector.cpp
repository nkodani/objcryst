#include "CrystVector/CrystVector.h"
#include "Quirks/VFNStreamFormat.h"

#ifdef __LIBCRYST_VECTOR_USE_BLITZ__

#define CrystVector Array<double,1>
#define CrystMatrix Array<double,2>

template<class T> T MaxDifference(const Array<T,1> &a,const Array<T,1> &b)
{
   return max(fabs(a-b));
}

template<class T> T MaxDifference(const Array<T,2> &a,const Array<T,2> &b)
{
   return max(fabs(a-b));
}

#else  // __LIBCRYST_VECTOR_USE_BLITZ__

//Still using pointers instead of blitz for geometrical structure factor, 
//due to huge memory requirements with gcc when using blitz.
#define __VFN_GEOM_STRUCT_FACTOR_USE_POINTERS

#include "Quirks/VFNDebug.h"
#include "Quirks/VFNStreamFormat.h"

//######################################################################
//  CrystVector
//######################################################################
template<class T> CrystVector<T>::CrystVector():mpData(0),mNumElements(0),mIsAreference(false) {}
   
template<class T> CrystVector<T>::CrystVector(const long nbElements):
mNumElements(nbElements),mIsAreference(false)
{
   mpData=new T[mNumElements];
}
   
template<class T> CrystVector<T>::CrystVector(const CrystVector &old):
mNumElements(old.numElements()),mIsAreference(false)
{
   mpData=new T[mNumElements];
   T *p1=mpData;
   const T *p2=old.data();
   for(long i=0;i<mNumElements;i++) *p1++=*p2++;
}
   
template<class T> CrystVector<T>::~CrystVector(){ if(!mIsAreference)delete[] mpData;}

template<class T> void CrystVector<T>::operator=(const CrystVector &old)
{
   VFN_DEBUG_MESSAGE("CrystVector<T>::operator=()",0)
   this->resize(old.numElements());
	mIsAreference=false;
   T *p1=mpData;
   const T *p2=old.data();
   for(long i=0;i<mNumElements;i++) *p1++=*p2++;
}

template<class T> void CrystVector<T>::reference(CrystVector &old)
{
   //if(!mIsAreference)delete[] mpData ;
   mNumElements=old.numElements();
   mpData=old.data();
   mIsAreference=true;
}
   
template<class T> long CrystVector<T>::numElements()const {return mNumElements;}
   
template<class T> T CrystVector<T>::sum()const
{
   register T tmp=0;
   register const T *p=this->data();
   for(long i=0;i<this->numElements();i++) tmp += *p++ ;
   return tmp;
}
template<class T> T CrystVector<T>::min()const
{
   register T tmp=0;
   register const T *p=this->data();
   tmp=*p++;
   for(long i=1;i<this->numElements();i++)
   {
      if(tmp>*p) tmp=*p;
      p++;
   }
   return tmp;
}
template<class T> T CrystVector<T>::max()const
{
   register T tmp=0;
   register const T *p=this->data();
   tmp=*p++;
   for(long i=1;i<this->numElements();i++)
   {
      if(tmp<*p) tmp=*p;
      p++;
   }
   return tmp;
}

template<class T> T * CrystVector<T>::data() {return mpData;}
template<class T> const T * CrystVector<T>::data() const {return mpData;}

template<class T> void CrystVector<T>::resize(const long newNbElements)
{  
   if(mNumElements==newNbElements) return;
   VFN_DEBUG_MESSAGE("CrystVector<T>::resize():("<<mNumElements<<"->"
      <<newNbElements<<").",0)
   if((mIsAreference==false) && (mNumElements != 0))delete[] mpData ;
   mpData=0;
   mNumElements=newNbElements;
   if(mNumElements>0) mpData=new T[mNumElements];
   mIsAreference=false;
}

template<class T> void CrystVector<T>::resizeAndPreserve(const long newNbElements)
{
   //:TODO: check memory allocation
   if(newNbElements == mNumElements) return;
   register T *p=mpData;
   register T *p2,*p1;
   mpData=0;
   mpData=new T[newNbElements];
   p2=mpData;
   p1=p;
   long tmp= (newNbElements > mNumElements) ? mNumElements : newNbElements ;
   for(long i=0;i<tmp;i++) *p2++ = *p1++ ;
   if(mIsAreference==false)delete[] p ;
   mNumElements=newNbElements;
   mIsAreference=false;
}

template<class T> void CrystVector<T>::operator*=(const T num)
{
   register T *p=mpData;
   for(int i=0;i<mNumElements;i++) *p++ *= num;
}

template<class T> void CrystVector<T>::operator*=(const CrystVector<T> &vect)
{
   #ifdef __DEBUG__
   if( vect.numElements() != mNumElements)
   {
      cout<<"CrystVector::operator*=(&vect)(i): Number of elements differ:"<< mNumElements \
            << "!="<< vect.numElements() <<endl;
      throw 0;
   }
   #endif
   register T *p=mpData;
   register const T *rhs=vect.data();
   for(int i=0;i<mNumElements;i++) { *p *= *rhs; p++ ; rhs++;}
}

template<class T> void CrystVector<T>::operator/=(const T num)
{
   register T *p=mpData;
   for(int i=0;i<mNumElements;i++) *p++ /= num;
}

template<class T> void CrystVector<T>::operator+=(const T num)
{
   register T *p=mpData;
   for(int i=0;i<mNumElements;i++) *p++ += num;
}

template<class T> void CrystVector<T>::operator+=(const CrystVector<T> &vect)
{
   #ifdef __DEBUG__
   if( vect.numElements() != mNumElements)
   {
      cout<<"CrystVector::operator+=(&vect)(i): Number of elements differ:"<< mNumElements \
            << "!="<< vect.numElements() <<endl;
      throw 0;
   }
   #endif
   register T *p=mpData;
   const T *rhs=vect.data();
   for(int i=0;i<mNumElements;i++) *p++ += *rhs++;
}

template<class T> void CrystVector<T>::operator-=(const CrystVector<T> &vect)
{
   #ifdef __DEBUG__
   if( vect.numElements() != mNumElements)
   {
      cout<<"CrystVector::operator-=(&vect)(i): Number of elements differ:"<< mNumElements \
            << "!="<< vect.numElements() <<endl;
      throw 0;
   }
   #endif
   register T *p=mpData;
   const T *rhs=vect.data();
   for(int i=0;i<mNumElements;i++) *p++ -= *rhs++;
}

template<class T> void CrystVector<T>::operator=(const T num)
{
   register T *p=mpData;
   for(int i=0;i<mNumElements;i++) *p++ = num;
}

template<class T> T CrystVector<T>::operator()(const long i) const
{
   #ifdef __DEBUG__
   if( (i<0) || (i>=mNumElements))
   {
      cout<<"CrystVector::operator()(i): Tried to access an element out of bounds :"<<i<<endl;
      throw 0;
   }
   #endif
   return mpData[i];
}

template<class T> T& CrystVector<T>::operator()(const long i)
{
   #ifdef __DEBUG__
   if( (i<0) || (i>=mNumElements))
   {
      cout<<"CrystVector::operator()(i): Tried to access an element out of bounds !"<<i<<endl;
      throw 0;
   }
   #endif
   return mpData[i];
}

template<class T> ostream& operator<<(ostream &os, CrystVector<T> &vect)
{
   //return os << FormatHorizVector(vect);
   for(long i=0;i<vect.numElements();i++) os << vect(i) <<endl;
   return os;
}

template<class T> CrystVector<long> SortSubs(const CrystVector<T> &vect)
{
   CrystVector<long> subscript(vect.numElements());
   {
      long *pLong=subscript.data();
      for(long i=0;i<subscript.numElements();i++) *pLong++ = i;
   }
   CrystVector<T> v;
   v=vect;
   QuickSortSubs(v,subscript,v.numElements()-1,0,0);
   return subscript;
}

template<class T> long QuickSortSubs(CrystVector<T> &vect,
                                     CrystVector<long> &subscript,
                                     long last,long first, int depth)
{
	//assert(depth++ <50);//for up to 2^50 elements
	static long count=0;
	long low, high;
	T tmpT, sepValeur ;
	long tmpSubs;
	low = first;
	high = last;
	sepValeur = vect( (first + last) / 2 );
	do 
	{
		while( vect(low) < sepValeur ) low++;
		while( vect(high) > sepValeur ) high--;
		if( low <= high )
		{
			count++;
			tmpT = vect(low);
			vect(low) = vect(high);
			vect(high) = tmpT;
			tmpSubs = subscript(low);
			subscript(low++) = subscript(high);
			subscript(high--) = tmpSubs;
		}
	} while( low <= high);
	if( first < high ) QuickSortSubs(vect,subscript, high, first,depth);
	if( low < last ) QuickSortSubs(vect,subscript, last, low,depth);
	return count ;
}

template<class T> CrystVector<T> cos(const CrystVector<T> &vect)
{
   CrystVector<T> cosVect(vect.numElements());
   for(long i=0;i<vect.numElements();i++) cosVect(i) = (T) cos(vect(i));
   return cosVect;
}

template<class T> CrystVector<T> sin(const CrystVector<T> &vect)
{
   CrystVector<T> sinVect(vect.numElements());
   for(long i=0;i<vect.numElements();i++) sinVect(i) = (T) sin(vect(i));
   return sinVect;
}

template<class T> CrystVector<T> tan(const CrystVector<T> &vect)
{
   CrystVector<T> tanVect(vect.numElements());
   for(long i=0;i<vect.numElements();i++) tanVect(i) = (T) tan(vect(i));
   return tanVect;
}

template<class T> CrystVector<T> sqrt(const CrystVector<T> &vect)
{
   CrystVector<T> tanVect(vect.numElements());
   for(long i=0;i<vect.numElements();i++) tanVect(i) = (T) sqrt(vect(i));
   return tanVect;
}

//######################################################################
//  CrystMatrix
//######################################################################
template<class T> CrystMatrix<T> ::CrystMatrix():
mpData(0),mNumElements(0),mXSize(0),mYSize(0),mIsAreference(false){}

template<class T> CrystMatrix<T>::CrystMatrix(const long ySize,const long xSize):
mNumElements(xSize*ySize),mXSize(xSize),mYSize(ySize),mIsAreference(false)
{
   mpData=new T[mNumElements];
}

template<class T> CrystMatrix<T>::CrystMatrix(const CrystMatrix &old):
mNumElements(old.numElements()),mXSize(old.cols()),mYSize(old.rows()),mIsAreference(false)
{
   mpData=new T[mNumElements];
   register T *p1=mpData;
   register const T *p2=old.data();
   for(long i=0;i<mNumElements;i++) *p1++=*p2++;
}

template<class T> CrystMatrix<T>::~CrystMatrix(){ if(!mIsAreference)delete[] mpData;}

template<class T> void CrystMatrix<T>::operator=(const CrystMatrix<T> &old)
{
	mXSize=old.cols();
	mYSize=old.rows();
	mIsAreference=false;
	if(mNumElements!=old.numElements())
   {
      mNumElements=old.numElements();
      if(mIsAreference==false)delete[] mpData ;
      mpData=new T[mNumElements];
   }
   register T *p1=mpData;
   register const T *p2=old.data();
   for(long i=0;i<mNumElements;i++) *p1++=*p2++;
}

template<class T> void CrystMatrix<T>::reference(CrystMatrix<T> &old)
{
   if(mIsAreference==false)delete[] mpData ;
	mIsAreference=true;
   mNumElements=old.numElements();
   mpData=old.data();
}

template<class T> long CrystMatrix<T>::numElements()const {return mNumElements;}

template<class T> T CrystMatrix<T>::sum()const
{
   register T tmp=0;
   register const T *p=this->data();
   for(long i=0;i<this->numElements();i++) tmp += *p++ ;
   return tmp;
}

template<class T> T CrystMatrix<T>::min()const
{
   register T tmp=0;
   register const T *p=this->data();
   tmp=*p++;
   for(long i=1;i<this->numElements();i++)
   {
      if(tmp>*p) tmp=*p;
      p++;
   }
   return tmp;
}

template<class T> T CrystMatrix<T>::max()const
{
   register T tmp=0;
   register const T *p=this->data();
   tmp=*p++;
   for(long i=1;i<this->numElements();i++)
   {
      if(tmp<*p) tmp=*p;
      p++;
   }
   return tmp;
}

template<class T> long CrystMatrix<T>::cols()const {return mXSize;}

template<class T> long CrystMatrix<T>::rows()const {return mYSize;}

template<class T> T* CrystMatrix<T>::data() {return mpData;}
template<class T> const T* CrystMatrix<T>::data() const {return mpData;}

template<class T> void CrystMatrix<T>::resize(const long ySize,const long xSize)
{
   mXSize=xSize;
   mYSize=ySize;
   if(xSize*ySize == mNumElements) return;
   if(!mIsAreference)delete[] mpData ;
   mpData=0;
   mXSize=xSize;
   mYSize=ySize;
   mNumElements=xSize*ySize;
   if(mNumElements>0) mpData=new T[mNumElements];
}

template<class T> void CrystMatrix<T>::resizeAndPreserve(const long ySize,const long xSize)
{
   mXSize=xSize;
   mYSize=ySize;
   if(xSize*ySize == mNumElements) return;
   register T *p=mpData;
   register T *p2,*p1;
   mpData=0;
   mpData=new T[xSize*ySize];
   p2=mpData;
   p1=p;
   long tmp= ( (xSize*ySize) > mNumElements) ? mNumElements : xSize*ySize;
   for(long i=0;i<tmp;i++) *p2++ = *p1++ ;
   mNumElements=xSize*ySize;
   if(!mIsAreference)delete[] p ;
   mIsAreference=false;
}

template<class T> void CrystMatrix<T>::operator*=(const T num)
{
   register T *p=mpData;
   for(int i=0;i<mNumElements;i++) *p++ *= num;
}

template<class T> void CrystMatrix<T>::operator*=(const CrystMatrix<T> &vect)
{
   #ifdef __DEBUG__
   if( this->numElements() != vect.numElements())
   {
      cout<<"CrystMatrix::operator*=(): Number of elements differ !"<<endl;
      throw 0;
   }
   #endif
   register T *p=mpData;
   register const T *rhs=vect.data();
   for(int i=0;i<mNumElements;i++) *p++ *= *rhs++;
}

template<class T> void CrystMatrix<T>::operator/=(const T num)
{
   register T *p=mpData;
   for(int i=0;i<mNumElements;i++) *p++ /= num;
}

template<class T> void CrystMatrix<T>::operator+=(const T num)
{
   register T *p=mpData;
   for(int i=0;i<mNumElements;i++) *p++ += num;
}

template<class T> void CrystMatrix<T>::operator-=(const T num)
{
   register T *p=mpData;
   for(int i=0;i<mNumElements;i++) *p++ -= num;
}

template<class T> T CrystMatrix<T>::operator()(const long i) const
{
   #ifdef __DEBUG__
   if( (i<0) || (i>=mNumElements))
   {
      cout<<"CrystMatrix::operator()(i): element out of bounds !"<<i<<endl;
      throw 0;
   }
   #endif
   return mpData[i];
}

template<class T> T CrystMatrix<T>::operator()(const long i,const long j) const
{  
   #ifdef __DEBUG__
   if( (i<0) || (j<0) || (i>=mYSize) || (j>=mXSize) )
   {
      cout<<"CrystMatrix::operator()(i,j): element out of bounds:"<<i<<","<<j<<endl;
      cout<<"dimensions:"<<mYSize<<","<<mXSize<<endl;
      throw 0;
   }
   #endif
   return mpData[i*mXSize+j];
}

template<class T> T& CrystMatrix<T>::operator()(const long i)
{
   #ifdef __DEBUG__
   if( (i<0) || (i>=mNumElements))
   {
      cout<<"CrystMatrix::operator()(i): element out of bounds !"<<i<<endl;
      throw 0;
   }
   #endif
   return mpData[i];
}

template<class T> T& CrystMatrix<T>::operator()(const long i,const long j) 
{  
   #ifdef __DEBUG__
   if( (i<0) || (j<0) || (i>=mYSize) || (j>=mXSize) )
   {
      cout<<"CrystMatrix::operator()(i,j): element out of bounds:"<<i<<","<<j<<endl;
      throw 0;
   }
   #endif
   return mpData[i*mXSize+j];
}

template<class T> CrystMatrix<T> CrystMatrix<T>::transpose(const int dim1, const int dim2)const
{
   CrystMatrix<T> newM(this->cols(),this->rows());
   for(long i=0;i<this->cols();i++)
      for(long j=0;j<this->rows();j++) newM(i,j)=(*this)(j,i);
   return newM;
}

template<class T> ostream& operator<<(ostream &os, const CrystMatrix<T> &vect)
{
   //return os << FormatHorizVector(vect);
   for(long i=0;i<vect.rows();i++) 
   {
      for(long j=0;j<vect.cols();j++) os << FormatFloat(vect(i,j)) ;
      os << endl;
   }
   return os;
}

template<class T> T MaxDifference(const CrystVector<T> &a,const CrystVector<T> &b)
{
   const T *p1=a.data();
   const T *p2=b.data();
   double max=0;
   double tmp=0;
   for(long i=0;i<a.numElements();i++)
   {
      tmp=(T)fabs((double) (*p1++ - *p2++));
      if(tmp>max) max=tmp;
   }
   return (T)max;
}

template<class T> T MaxDifference(const CrystMatrix<T> &a,const CrystMatrix<T> &b)
{
   const T *p1=a.data();
   const T *p2=b.data();
   T max=0;
   T tmp=0;
   for(long i=0;i<a.numElements();i++)
   {
      tmp=(T)fabs((double)(*p1++ - *p2++));
      if(tmp>max) max=tmp;
   }
   return max;
}

template<class T> CrystMatrix<T> product(const CrystMatrix<T> &a,const CrystMatrix<T> &b)
{
   VFN_DEBUG_ENTRY("CrystMatrix<T> product()",2)
	//:TODO: check a.cols()==b.rows()
   CrystMatrix<T> ab(a.rows(),b.cols());
   for(long i=0;i<ab.rows();i++)
   {
      for(long j=0;j<ab.cols();j++)
      {
         ab(i,j)=0;
         for(long k=0;k<b.rows();k++) ab(i,j) += a(i,k)*b(k,j);
      }
   }
   VFN_DEBUG_EXIT("CrystMatrix<T> product()",2)
   return ab;
}

//explicit instantiation
template class CrystVector<double>;
template double MaxDifference(const CrystVector<double>&,const CrystVector<double>&);
template ostream& operator<<(ostream &os, CrystVector<double> &vect);
template CrystVector<long> SortSubs(const CrystVector<double> &vect);
template long QuickSortSubs(CrystVector<double> &vect,CrystVector<long> &subscript,
                            long last,long first, int depth);
template class CrystMatrix<double>;
template double MaxDifference(const CrystMatrix<double>&,const CrystMatrix<double>&);
template CrystMatrix<double> product(const CrystMatrix<double>&,const CrystMatrix<double>&);
template ostream& operator<<(ostream &os, const CrystMatrix<double> &vect);
template CrystVector<double> cos(const CrystVector<double>&);
template CrystVector<double> sin(const CrystVector<double>&);
template CrystVector<double> tan(const CrystVector<double>&);
template CrystVector<double> sqrt(const CrystVector<double>&);

template class CrystVector<float>;
template float MaxDifference(const CrystVector<float>&,const CrystVector<float>&);
template ostream& operator<<(ostream &os, CrystVector<float> &vect);
template CrystVector<long> SortSubs(const CrystVector<float> &vect);
template long QuickSortSubs(CrystVector<float> &vect,CrystVector<long> &subscript,
                            long last,long first, int depth);
template class CrystMatrix<float>;
template float MaxDifference(const CrystMatrix<float>&,const CrystMatrix<float>&);
template CrystMatrix<float> product(const CrystMatrix<float>&,const CrystMatrix<float>&);
template ostream& operator<<(ostream &os, const CrystMatrix<float> &vect);
template CrystVector<float> cos(const CrystVector<float>&);
template CrystVector<float> sin(const CrystVector<float>&);
template CrystVector<float> tan(const CrystVector<float>&);
template CrystVector<float> sqrt(const CrystVector<float>&);

template class CrystVector<long>;
template long MaxDifference(const CrystVector<long>&,const CrystVector<long>&);
template ostream& operator<<(ostream &os, CrystVector<long> &vect);
template CrystVector<long> SortSubs(const CrystVector<long> &vect);
template long QuickSortSubs(CrystVector<long> &vect,CrystVector<long> &subscript,
                            long last,long first, int depth);
template class CrystMatrix<long>;
template long MaxDifference(const CrystMatrix<long>&,const CrystMatrix<long>&);
template CrystMatrix<long> product(const CrystMatrix<long>&,const CrystMatrix<long>&);
template ostream& operator<<(ostream &os, const CrystMatrix<long> &vect);


template class CrystVector<int>;
template int MaxDifference(const CrystVector<int>&,const CrystVector<int>&);
template ostream& operator<<(ostream &os, CrystVector<int> &vect);
template CrystVector<long> SortSubs(const CrystVector<int> &vect);
template long QuickSortSubs(CrystVector<int> &vect,CrystVector<long> &subscript,
                            long last,long first, int depth);
template class CrystMatrix<int>;
template int MaxDifference(const CrystMatrix<int>&,const CrystMatrix<int>&);
template CrystMatrix<int> product(const CrystMatrix<int>&,const CrystMatrix<int>&);
template ostream& operator<<(ostream &os, const CrystMatrix<int> &vect);

template class CrystVector<unsigned int>;
template unsigned int MaxDifference(const CrystVector<unsigned int>&,const CrystVector<unsigned int>&);
template ostream& operator<<(ostream &os, CrystVector<unsigned int> &vect);
template CrystVector<long> SortSubs(const CrystVector<unsigned int> &vect);
template long QuickSortSubs(CrystVector<unsigned int> &vect,CrystVector<long> &subscript,
                            long last,long first, int depth);
template class CrystMatrix<unsigned int>;
template unsigned int MaxDifference(const CrystMatrix<unsigned int>&,const CrystMatrix<unsigned int>&);
template CrystMatrix<unsigned int> product(const CrystMatrix<unsigned int>&,const CrystMatrix<unsigned int>&);
template ostream& operator<<(ostream &os, const CrystMatrix<unsigned int> &vect);


template class CrystVector<bool>;
template ostream& operator<<(ostream &os, CrystVector<bool> &vect);
template class CrystMatrix<bool>;
template bool MaxDifference(const CrystMatrix<bool>&,const CrystMatrix<bool>&);
template ostream& operator<<(ostream &os, const CrystMatrix<bool> &vect);



#endif // __LIBCRYST_VECTOR_USE_BLITZ__

//######################################################################
//  Functions
//######################################################################


CrystMatrix_double InvertMatrix(const CrystMatrix_double &m)
{//:TODO: Check Pivoting...
   VFN_DEBUG_ENTRY("InvertMatrix()",2)
	VFN_DEBUG_MESSAGE("->Matrix to invert :"<<endl<<m,1)
   double eps = 1e-8;
   //check matrix is square
      if( (m.rows() != m.cols()) || (m.rows() <2))
      {//DoSomethingBad
         cout << "Matrix inversion: Cannot invert matrix !" <<endl;
         throw 0;
      }
   //prepare...
      long size=m.rows();
      CrystMatrix_double im(size,size),cm(size,size);//(future) invert matrix & copy of matrix
      cm=m;
      im=0;
      for(long i=0;i<size;i++) im(i,i)=1.;
      CrystMatrix_long rowIndex(size,1);//Keep track of pivoted rows
      for(long i=0;i<size;i++) rowIndex(i)=i;
      double det=1;
   //Make an upper triangular matrix
      for(long i=0;i<size;i++)
      {
         //get absolute maximum the ith column
            long rowMax=i;
            {
               double max=fabs(m(i,i));
               for(long j=i+1;j<m.rows();j++)
                  if(fabs(m(j,i)) > max)
                  {
                     max=fabs(m(j,i));
                     rowMax=j;
                  }
                  
               //Check if pivot is non-singular
               
               if(max < eps)
               {
                  throw i;
               }
            }
         //pivot
            if(rowMax != i)
            {
               //cout << "Exchanging rows: "<< i << " and " << rowMax <<endl;
               MatrixExchangeRows(cm,i,(long)rowMax);
               MatrixExchangeRows(im,i,(long)rowMax);
               MatrixExchangeRows(rowIndex,i,(long)rowMax);
            }
         //cout << cm <<endl;
         /*
         */
         //substract
            double a;
            for(long j=i+1;j<size;j++)
            {
               a=cm(j,i)/cm(i,i);
               for(long k=0;k<size;k++) im(j,k) -= im(i,k)*a;
               for(long k=0;k<size;k++) cm(j,k) -= cm(i,k)*a;
            }
            det *= a;
         //cout << cm <<endl;
         //cout << im <<endl;
      }
   VFN_DEBUG_MESSAGE("Finish solving from the upper triangular matrix...",1);
      for(long i=0;i<size;i++)
      {
         double a;
         for(long j=i-1;j>=0;j--)
         {
            a=cm(j,i)/cm(i,i);
            for(long k=0;k<size;k++) im(j,k) -= im(i,k)*a;
            for(long k=0;k<size;k++) cm(j,k) -= cm(i,k)*a;
         }
         a=cm(i,i);
         for(long k=0;k<size;k++) im(i,k) /= a;
         for(long k=0;k<size;k++) cm(i,k) /= a;
         //cout << cm <<endl;
         //cout << im <<endl;
      }
   //bring back to initial order of rows
   /*
   for(long i=0;i<size;i++)
   {
      if(rowIndex(i) != i)
      {
         long rowExch=0;
         for(; rowIndex(rowExch) != i ; rowExch++);
         cout << rowExch << endl;
         MatrixExchangeRows(im,i,rowExch);
      }
   }
   */
	VFN_DEBUG_MESSAGE("->Inverted Matrix :"<<endl<<im,1)
   VFN_DEBUG_EXIT("InvertMatrix()",2)
   return im;
}

template<class T> void MatrixExchangeRows(CrystMatrix_T &m, const long row1, const long row2)
{
   //cout << "Exchanging rows:begin" <<endl;
   if(row1 == row2) return;
   long rowSize=m.cols();
   T tmp;
   for(long i=0;i<rowSize;i++)
   {
      tmp=m(row1,i);
      m(row1,i)=m(row2,i);
      m(row2,i)=tmp;
   }
   //cout << "Exchanging rows:end" <<endl;
}

template void MatrixExchangeRows(CrystMatrix_double &m, const long row1, const long row2);



template<class T> T MaxAbs(const CrystVector_T &vector)
{
   const T* pData=vector.data();
   T max =(T) fabs((double) *pData++);
   for(long i=1;i<vector.numElements();i++)
   {
      if ( fabs((double) *pData) > max) max=(T) fabs((double) *pData);
      pData++;
   }
   return max;
}

//Explicit instatiation
template double  MaxAbs(const CrystVector_double &vector);
template float MaxAbs(const CrystVector_float &vector);
template int    MaxAbs(const CrystVector_int &vector);
template unsigned int    MaxAbs(const CrystVector_uint &vector);
template long   MaxAbs(const CrystVector_long &vector);


template<class T> T MinAbs(const CrystVector_T &vector)
{
   const T* pData=vector.data();
   T min =(T)  fabs((double) *pData++);
   for(long i=1;i<vector.numElements();i++)
   {
      if ( fabs((double) *pData) < min) min=(T) fabs((double) *pData);
      pData++;
   }
   return min;
}

//Explicit instatiation
template double  MinAbs(const CrystVector_double &vector);
template float MinAbs(const CrystVector_float &vector);
template int    MinAbs(const CrystVector_int &vector);
template unsigned int    MinAbs(const CrystVector_uint &vector);
template long   MinAbs(const CrystVector_long &vector);

//######################################################################
//  CubicSpline
//######################################################################

CubicSpline::CubicSpline(const CrystVector_double &x, const CrystVector_double &y, 
            const double yp0, const double ypn):
mX(x),mY(y),mYsecond(x.numElements())
{
   VFN_DEBUG_MESSAGE("CubicSpline::CubicSpline(x,y,yp0,ypn)",5)
   const long n=x.numElements();
   CrystVector_double u(mX.numElements());
   mYsecond(0)=-0.5;
   u(0)=(3/(mX(1)-mX(0)))*((mY(1)-mY(0))/(mX(1)-mX(0))-yp0);
   double a,b;
   for(long i=1;i<(n-1);i++)
   {
      a=(mX(i)-mX(i-1))/(mX(i+1)-mX(i-1));
      b=a*mYsecond(i-1)+2;
      mYsecond(i)=(a-1.)/b;
      u(i)=(6*((mY(i+1)-mY(i))/(mX(i+1)-mX(i))-(mY(i)-mY(i-1))/(mX(i)-mX(i-1)))
           /(mX(i+1)-mX(i-1))-a*u(i-1))/b;
   }
   const double c=(3./(mX(n-1)-mX(n-2)))*(ypn-(mY(n-1)-mY(n-2))/(mX(n-1)-mX(n-2)));
   mYsecond(n-1)=(c-.5*u(n-2))/(0.5*mYsecond(n-2)+1);
   for(long i=(n-2);i>=0;i--) mYsecond(i)=mYsecond(i)*mYsecond(i+1)+u(i);
   VFN_DEBUG_MESSAGE("CubicSpline::CubicSpline(x,y,yp0,ypn):End",5)
}

CubicSpline::CubicSpline(const CrystVector_double &x, const CrystVector_double &y):
mX(x),mY(y),mYsecond(x.numElements())
{
   VFN_DEBUG_MESSAGE("CubicSpline::CubicSpline(x,y)",5)
   const long n=x.numElements();
   CrystVector_double u(mX.numElements());
   mYsecond(0)=0;
   u(0)=0;
   double a,b;
   for(long i=1;i<=(n-2);i++)
   {
      a=(mX(i)-mX(i-1))/(mX(i+1)-mX(i-1));
      b=a*mYsecond(i-1)+2;
      mYsecond(i)=(a-1)/b;
      u(i)=(6*((mY(i+1)-mY(i))/(mX(i+1)-mX(i))-(mY(i)-mY(i-1))/(mX(i)-mX(i-1)))
           /(mX(i+1)-mX(i-1))-a*u(i-1))/b;
   }
   mYsecond(n-1)=0;
   for(long i=(n-2);i>=0;i--) mYsecond(i)=mYsecond(i)*mYsecond(i+1)+u(i);
   VFN_DEBUG_MESSAGE("CubicSpline::CubicSpline(x,y):End",5)
}

CubicSpline::~CubicSpline()
{
}

double CubicSpline::operator()(const double x) const
{
   //:TODO: faster!
   long i;
   if(x<mX(0)) return mY(0);
   for(i=0;i<(mX.numElements()-1);i++)
      if(x<mX(i+1))
      {
         VFN_DEBUG_MESSAGE("CubicSpline::operator()(x):"<<x<<":"<<mX(i+1)<<":"<<i,0)
         const double e=mX(i+1)-mX(i);
         const double a=(mX(i+1)-x)/e;
         const double b=1.-a;
         const double c=1./6.*(a*a*a-a)*e*e;
         const double d=1./6.*(b*b*b-b)*e*e;
         return a*mY(i)+b*mY(i+1)+c*mYsecond(i)+d*mYsecond(i+1);
      }
   return mY(mY.numElements()-1);
}

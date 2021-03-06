#ifndef MAP_H
#define MAP_H

#include "Point.h"
#include "Array2D.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>

#ifndef FuncName
#define FuncName __FUNCTION__
#endif


template < class T >
class DataPoint : public Point<T> {
public:
   T dis, data;

public:
   DataPoint(const Point<T> ptin, const T datain = -12345., const T disin = -12345. )
      : Point<T>(ptin)
      , dis(disin), data(datain) {}

   DataPoint(const T lonin = -12345., const T latin = -12345., const T datain = -12345., const T disin = -12345. )
      : Point<T>(lonin, latin)
      , dis(disin), data(datain) {}

   inline const T& Dis() const { return dis; }
   inline	T& Dis() { return dis; }
   inline const T& Data() const { return data; }
   inline	T& Data() { return data; }

	/*
	DataPoint<T>& operator=( T dp2 ) {
		data = dp2.data;
		return *this;
	}

	const operator T() const { return data; }
	*/

   friend std::ostream& operator << (std::ostream& o, DataPoint a) { 
      o << a.lon << " " << a.lat << " " <<a.data<<" "<<a.dis; 
      return o; 
   }

};


class Map {
public:
   //Map( const char *inname );
	Map( const float grdlon = 1., const float grdlat = 1. );
   Map( const std::string& inname, const float grdlon = 1.0, const float grdlat = 1.0 );
   Map( const std::string& inname, const Point<float>& srcin, const float grdlon = 1.0, const float grdlat = 1.0 );
   Map( const Map& );
   Map( Map&& );
   Map& operator= ( const Map& );
   Map& operator= ( Map&& );
   ~Map();

	/* ------------ set source location ------------ */
	//void SetSource( const Point<float>& src );

	/* ----- map boundaries ----- */
	float LonMin() const;
	float LonMax() const;
	float LatMin() const;
	float LatMax() const;

	bool isReg() const;

	/* ------------ IO and resets ------------ */
	void Load( const std::string& fnamein );
	/* ------------ set source location ------------ */
	void SetSource( const float lon, const float lat ) { SetSource( Point<float>(lon, lat) ); }
	void SetSource( const Point<float>& srcin );

	size_t size() const;

	/* --- clip the map around the source location (to speed up the average methods) --- */
	void Clip( const float lonmin, const float lonmax, const float latmin, const float latmax );

	/* ------------ compute number of points near the given location ------------ */
	float NumberOfPoints(Point<float> rec, const float xhdis, const float yhdis) const {
		float loneff, lateff;
		NumberOfPoints( rec, xhdis, yhdis, loneff, lateff );
	}
	float NumberOfPoints(Point<float> rec, const float xhdis, const float yhdis, float& loneff, float& lateff) const;

   /* ------------ compute average value on the point rec ------------ */
   float PointAverage(Point<float> Prec, float hdis ) {
      float weit;
      return PointAverage(Prec, hdis, weit);
   }
   float PointAverage(Point<float> rec, float hdis, float& weit);

   /* ------------ compute average value along the path src-rec ------------ */
	// acc = false: accurate to ~ 0.2 km, 10 times faster
	// acc = true: accurate to ~ 0.1 m (reduce toler in DisAzi.h if higher accuracy is required)
	/*
   float PathAverage(Point<float> Prec, const float lambda = 0., const bool acc = false) {
      float perc;
      return (PathAverage(Prec, perc, lambda, acc)).Data();
   }
	*/
   DataPoint<float> PathAverage(Point<float> Prec, float& perc, const float lambda = 0., const bool acc = false);

   /* ------------ compute average along the path src-rec weighted by the reciprocal of map values ------------ */
	/*
   float PathAverage_Reci(Point<float> Prec, const float lambda = 0., const bool acc = false) {
      float perc;
      return (PathAverage_Reci(Prec, perc, lambda, acc)).Data();
   }
	*/
   DataPoint<float> PathAverage_Reci(Point<float> Prec, float& perc, const float lambda = 0., const bool acc = false);

	// trace along the great circle path. return a vector of map values along the path
	template<class Functor>
	void TraceGCP( Point<float> Psrc, const Point<float>& Prec, float dis_step, const Functor& func );

	friend std::ostream& operator<<( std::ostream& o, const Map& map ) {
		o<<map.LonMin()<<" - "<<map.LonMax()<<"   "<<map.LatMin()<<" - "<<map.LatMax();
		return o;
	}

protected:
	static constexpr float NaN = -12345.;
	static constexpr float trace_step = 20.;	// sample each 10 km,
	static const int npts_min = 5;				// or at least 5 sample points

private:
	std::string fname;
	Point<float> src;
   struct Mimpl;
   std::unique_ptr<Mimpl> pimplM;

};

namespace ErrorM {
   class Base : public std::runtime_error {
   public:
      Base(const std::string message)
         : runtime_error(message) {
            //PrintStacktrace();
      }
   };

	class BadFile : public Base {
   public:
      BadFile(const std::string funcname, const std::string info = "")
         : Base("Error("+funcname+"): Cannot access file ("+info+").") {}
   };

	class BadParam : public Base {
   public:
      BadParam(const std::string funcname, const std::string info = "")
         : Base("Error("+funcname+"): Bad parameters ("+info+").") {}
   };

	class IrregGrid : public Base {
   public:
      IrregGrid(const std::string funcname, const std::string info = "")
         : Base("Error("+funcname+"): Irregular Grid ("+info+").") {}
   };

	class Others : public Base {
   public:
      Others(const std::string funcname, const std::string info = "")
         : Base("Error("+funcname+"): General exception ("+info+").") {}
   };
}

#endif

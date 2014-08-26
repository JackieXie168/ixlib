// ----------------------------------------------------------------------------
//  Description      : Polygon handling
// ----------------------------------------------------------------------------
//  Remarks          : none.
//
// ----------------------------------------------------------------------------
//  (c) Copyright 2000 by iXiONmedia, all rights reserved.
// ----------------------------------------------------------------------------




#ifndef IXLIB_POLYGON
#define IXLIB_POLYGON




#include <vector>
#include <ixlib_geometry.hh>




namespace ixion {
  template<class T,int Dim>
  inline double getAngle(coord_vector<T,Dim> const &vec1,coord_vector<T,Dim> const &vec2) {
    double ip = vec1*vec2/(sqrt(vec1*vec1)*sqrt(vec2*vec2));
    return acos(ip);
    }
  
  
  
  
  template<class T>
  inline double getAngle(coord_vector<T,2> const &vec) {
    return atan2(vec[1],vec[0]);
    }
  
  
  
  
  template<class T>
  class polygon_segment : public vector<coord_vector<T,2> > {
      // vertices are enumerated in counter-clockwise 
      // (mathematically positive) order
      // boolean operations depend on this
    public:
      typedef coord_vector<T,2>      vertex_2d;

    private:
      typedef vector<vertex_2d>      Super;

    public:
      polygon_segment() {
        }
      polygon_segment(rectangle<T> const &src);
      polygon_segment(polygon_segment const &src) 
        : Super(src) {
        }
  
      void push_back_c(T x,T y);
      void insert_c(iterator it,T x,T y);

      bool isPointInside(T x,T y);
      
      void removeCrossings();
      void makeConvexHull(polygon_segment &dest) const;
      void smooth(polygon_segment &dest) const;
      void translate(T x,T y);
  
      rectangle<T> getBoundingBox() const;
      vertex_2d getCenter() const;
      vertex_2d getWeightedCenter() const;
      vertex_2d getPointOnOutside() const;
    };




  template<class T>
  class polygon : public vector<polygon_segment<T> *> {
      typedef vector<polygon_segment<T> *>      Super;
      
    public:
      typedef polygon_segment<T>::vertex_2d	vertex_2d;

    public:
      polygon() {
        }
      polygon(polygon const &src);
      polygon &operator=(polygon const &src);
      ~polygon();
  
      void clear();
      
      bool isPointInside(T x,T y);

      void smooth();
      void translate(T x,T y);
  
      void unite(polygon &dest,polygon const &src) const;
      void intersect(polygon &dest,polygon const &src) const;
      void subtract(polygon &dest,polygon const &subtrahend) const;
  
      rectangle<T> getBoundingBox() const;
      vertex_2d getCenter() const;
      vertex_2d getWeightedCenter() const;
  
      template<class HLineRoutine>
      void drawScanlines(HLineRoutine const &hlr,T step = 1) const;
      
    private:
      void freeSegments();
    };
  }




#endif
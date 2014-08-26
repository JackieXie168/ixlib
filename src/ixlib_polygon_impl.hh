// ----------------------------------------------------------------------------
//  Description      : Polygon handling
// ----------------------------------------------------------------------------
//  (c) Copyright 2000 by iXiONmedia, all rights reserved.
// ----------------------------------------------------------------------------




#include <algorithm>
#include <ixlib_base.hh>
#include <ixlib_polygon.hh>




// tool function --------------------------------------------------------------
namespace {
  using namespace ixion;
  
  
  
  enum TLineCrossState { CROSS,CROSS_OUTER,PARALLEL,ENDPOINT };
  
  
  
  
  template<class T>
  TLineCrossState lineCross(coord_vector<T,2> const &a1,coord_vector<T,2> const &b1,
  coord_vector<T,2> const &a2,coord_vector<T,2> const &b2,coord_vector<T,2> *crossing = NULL) {
    coord_vector<T,2> d1 = b1-a1,d2 = b2-a2,s = a2-a1;
    double det = d1[0]*d2[1] - d1[1]*d2[0];
    if (det == 0) return PARALLEL;
    
    double alpha = (s[0]*d2[1] - s[1]*d2[0]) / det;
    double beta = (s[0]*d1[1] - s[1]*d1[0]) / det;

    if (crossing) {
      *crossing = a1;
      (*crossing)[0] += T(d1[0]*alpha);
      (*crossing)[1] += T(d1[1]*alpha);
      }

    if (alpha == 0 || alpha == 1) return ENDPOINT;
    if (beta == 0 || beta == 1) return ENDPOINT;
    if (alpha < 0 || alpha > 1) return CROSS_OUTER;
    if (beta < 0 || beta > 1) return CROSS_OUTER;

    return CROSS;
    }




  template<class T>
  struct vertex_identifier { 
    // also identifies the line formed by this and the next vertex
    polygon_segment<T>			*Segment;
    polygon_segment<T>::const_iterator	Vertex;
    
    vertex_identifier(polygon_segment<T> *segment,polygon_segment<T>::const_iterator vertex)
      : Segment(segment),Vertex(vertex) {
      }
    polygon_segment<T>::const_iterator next() const {
      if (Vertex+1 == Segment->end()) return Segment->begin();
      else return Vertex+1;
      }
    polygon_segment<T>::const_iterator previous() const {
      if (Vertex == Segment->begin()) return Segment->end() - 1;
      else return Vertex-1;
      }
    T operator[](int index) const {
      return (*Vertex)[index];
      }
    bool operator<(vertex_identifier const &vid2) const {
      return (*Vertex)[1] < vid2[1];
      }
    };




  template<class T>
  struct vertex_and_angle {
    const polygon_segment<T>::vertex_2d	*Vertex;
    double				Angle;
    
    vertex_and_angle(polygon_segment<T>::vertex_2d const *vertex,double angle)
      : Vertex(vertex),Angle(angle) {
      }
    bool operator<(vertex_and_angle const &vaa2) const {
      return Angle < vaa2.Angle;
      }
    };
  }




// polygon_segment ------------------------------------------------------------
template<class T>
ixion::polygon_segment<T>::polygon_segment(rectangle<T> const &src) {
  push_back(src.A);
  push_back_c(src.A[0],src.B[1]);
  push_back(src.B);
  push_back_c(src.B[0],src.A[1]);
  }




template<class T>
void ixion::polygon_segment<T>::push_back_c(T x,T y) {
  push_back(vertex_2d(x,y));
  }




template<class T>
void ixion::polygon_segment<T>::insert_c(iterator it,T x,T y) {
  insert(it,vertex_2d(x,y));
  }




template<class T>
bool ixion::polygon_segment<T>::isPointInside(T x,T y) {
  if (size() <= 2) return false;
  
  vertex_2d poo = getPointOnOutside();
  vertex_2d poi(x,y);
  TSize crossings = 0;
  TSize endpoints = 0;
  
  const_iterator next = begin(),
    last = end(),
    previous = last - 1;
  while (next != last) {
    TLineCrossState result = lineCross(poi,poo,*previous,*next);
    if (result == ENDPOINT) endpoints++;
    if (result == CROSS) crossings++;
    
    previous = next;
    next++;
    }
  crossings += endpoints/2;
  return crossings % 2 == 1;
  }




template<class T>
void ixion::polygon_segment<T>::removeCrossings() {
  if (size() < 4) return;
  
  // we need to use numbers to index since iterators become invalid 
  // when changing the container

  vertex_2d crossing;
  TIndex next,last,previous,runner;

restart_algorithm:
  next = 0;
  last = size();
  previous = last - 1;
  
  while (next != last) {
    runner = next + 1;
    while (runner < last - 1) {
      TLineCrossState result = lineCross(
        operator[](previous),operator[](next),
	operator[](runner),operator[](runner + 1),&crossing);
      if (result == CROSS) {
        reverse(begin() + next,begin() + runner + 1);
        insert(begin() + runner + 1,crossing);
        insert(begin() + next,crossing);
        
        if (next < previous)
	  /* this is necessary if we are still processing
	   * the back() ---> front() edge.
	   *
	   * if we reach this place, we have inserted an edge at the very
	   * front of the polygon, of whose crossing behavior we cannot 
	   * assume anything.
	   *
	   * we do not lose much since the algorithm has only just started.
	   * (and it's O(n^2) anyway :-)
	   */
          goto restart_algorithm;
	
	runner = next + 1;
	last = size();
        }
      else 
        runner++;
      }
    
    previous = next;
    next++;
    }
  }




template<class T>
void ixion::polygon_segment<T>::smooth(polygon_segment &dest) const {
  dest.clear();
  
  const_iterator next = begin(),
    last = end(),
    previous = last - 1;
  while (next != last) {
    dest.push_back(*previous + (*next-*previous)/3);
    dest.push_back(*previous + (2*(*next-*previous))/3);
    
    previous = next;
    next++;
    }
  }




template<class T>
void ixion::polygon_segment<T>::makeConvexHull(polygon_segment &dest) const {
  // this is an implementation of graham's scan
  
  // determine center
  vertex_2d center = getWeightedCenter();
  
  // create list of vertices, sorted by angle with center and x axis
  typedef vector<vertex_and_angle<T> > angle_list;
  angle_list angles;
  
  FOREACH_CONST(first,*this,polygon_segment<T>)
    angles.push_back(vertex_and_angle<T>(&*first,getAngle(*first-center)));
    
  sort(angles.begin(),angles.end());
  
  // obtain point that we know to be in the convex hull: the rightmost one
  const_iterator rightmost_vertex = begin();
  FOREACH_CONST(first,*this,polygon_segment<T>)
    if ((*first)[0] > (*rightmost_vertex)[0])
      rightmost_vertex = first;
  
  // push rightmost vertex and subsequent vertex into hull.
  const_iterator first = rightmost_vertex;
  dest.push_back(*first);
  first++;
  if (first == end()) first = begin();
  dest.push_back(*first);
  first++;
  if (first == end()) first = begin();
  
  // do the scan
  while (first != rightmost_vertex) {
    double alpha = getAngle(dest[dest.size()-1]-dest[dest.size()-2]);
    double beta = getAngle(*first-dest[dest.size()-1]);
    double inner_angle = alpha-beta;
    
    if ((-Pi <= inner_angle && inner_angle <= 0) || (Pi <= inner_angle)) {
      // we would create an inner angle above PI ---> remove last vertex
      dest.pop_back();
      }
    else {
      dest.push_back(*first);
      
      first++;
      if (first == end()) first = begin();
      }
    }
  }




template<class T>
void ixion::polygon_segment<T>::translate(T x,T y) {
  coord_vector<T,2> displacement(x,y);
  FOREACH(first,*this,polygon_segment<T>)
    *first += displacement;
  }




template<class T>
ixion::rectangle<T> ixion::polygon_segment<T>::getBoundingBox() const {
  if (size() == 0) EXGEN_THROW(EC_EMPTYSET)
  
  rectangle<T> result(front(),front());
  const_iterator first = begin() + 1,last = end();
  while (first != last) {
    if ((*first)[0] < result.A[0]) result.A[0] = (*first)[0];
    if ((*first)[1] < result.A[1]) result.A[1] = (*first)[1];
    if ((*first)[0] > result.B[0]) result.B[0] = (*first)[0];
    if ((*first)[1] > result.B[1]) result.B[1] = (*first)[1];
    first++;
    }
  return result;
  }




template<class T>
ixion::polygon_segment<T>::vertex_2d ixion::polygon_segment<T>::getCenter() const {
  rectangle<T> bobox = getBoundingBox();
  return (bobox.A + bobox.B) / 2;
  }




template<class T>
ixion::polygon_segment<T>::vertex_2d ixion::polygon_segment<T>::getWeightedCenter() const {
  if (size() == 0) EXGEN_THROW(EC_EMPTYSET)
  vertex_2d result = front();
  const_iterator first = begin() + 1,last = end();
  while (first != last) 
    result += *first++;
  return result / size();
  }




template<class T>
ixion::polygon_segment<T>::vertex_2d ixion::polygon_segment<T>::getPointOnOutside() const {
  return getBoundingBox().A - vertex_2d(1,1);
  }




// polygon --------------------------------------------------------------------
template<class T>
ixion::polygon<T>::polygon(polygon const &src) {
  FOREACH_CONST(first,src,polygon) {
    polygon_segment<T> *copy = new polygon_segment<T>(**first);
    EX_MEMCHECK(copy)
    push_back(copy);
    }
  }




template<class T>
ixion::polygon<T> &polygon<T>::operator=(polygon const &src) {
  clear();
  FOREACH_CONST(first,src,polygon) {
    polygon_segment<T> *copy = new polygon_segment<T>(**first);
    EX_MEMCHECK(copy)
    push_back(copy);
    }
  return *this;
  }




template<class T>
ixion::polygon<T>::~polygon() {
  freeSegments();
  }




template<class T>
void ixion::polygon<T>::clear() {
  freeSegments();
  clear();
  }




template<class T>
bool ixion::polygon<T>::isPointInside(T x,T y) {
  TSize insides = 0;
  FOREACH_CONST(first,*this,polygon)
    if ((*first)->isPointInside(x,y)) insides++;
  return insides % 2 == 1;
  }




template<class T>
void ixion::polygon<T>::smooth() {
  FOREACH(first,*this,polygon) {
    polygon_segment<T> *copy = new polygon_segment<T>;
    EX_MEMCHECK(copy)
    (*first)->smooth(*copy);
    delete *first;
    *first = copy;
    }
  }




template<class T>
void ixion::polygon<T>::translate(T x,T y) {
  FOREACH_CONST(first,*this,polygon)
    (*first)->translate(x,y);
  }




template<class T>
void ixion::polygon<T>::unite(polygon &dest,polygon const &src) const {
  EXGEN_NYI
  }




template<class T>
void ixion::polygon<T>::intersect(polygon &dest,polygon const &src) const {
  EXGEN_NYI
  }




template<class T>
void ixion::polygon<T>::subtract(polygon &dest,polygon const &subtrahend) const {
  EXGEN_NYI
  }




template<class T>
rectangle<T> ixion::polygon<T>::getBoundingBox() const {
  if (size() == 0) EXGEN_THROW(EC_EMPTYSET)
  
  rectangle<T> result(front()->getBoundingBox());
  const_iterator first = begin() + 1,last = end();
  while (first != last)
    result.unite((*first++)->getBoundingBox());

  return result;
  }




template<class T>
ixion::polygon<T>::vertex_2d ixion::polygon<T>::getCenter() const {
  rectangle<T> bobox = getBoundingBox();
  return (bobox.A + bobox.B) / 2;
  }




template<class T>
ixion::polygon<T>::vertex_2d ixion::polygon<T>::getWeightedCenter() const {
  TSize total = front()->size();
  vertex_2d result(front()->getWeightedCenter()*total);
  
  const_iterator first = begin() + 1,last = end();
  while (first != last) {
    result += (*first)->getWeightedCenter()*(*first)->size();
    total += (*first)->size();
    first++;
    }
  return result / total;
  }




template<class T>
template<class HLineRoutine>
void ixion::polygon<T>::drawScanlines(HLineRoutine const &hlr,T step = 1) const {
  typedef vector<vertex_identifier<T> > line_list;
  line_list all_vertices;

  // create list of all vertices, sorted by y coordinate
  FOREACH_CONST(first_seg,*this,polygon<T>) {
    if ((*first_seg)->size() >= 3) {
      FOREACH_CONST(first,**first_seg,polygon_segment<T>)
	all_vertices.push_back(vertex_identifier<T>(*first_seg,first));
      }
    }
  sort(all_vertices.begin(),all_vertices.end());

  // create list of "active lines"
  // a line is specified by the vertex that begins it
  // active means "crossing the current scanline"
  line_list active_vertices;
  
  line_list::iterator 
    next = all_vertices.begin(),
    end_all = all_vertices.end();
  
  typedef vector<T> intersection_list;
  intersection_list intersections;
  
  rectangle<T> bobox = getBoundingBox();
  for (T y = bobox.A[1];y <= bobox.B[1];y += step) {
    // determine vertices passed by this scanline
    line_list::iterator end_this_line = next;
    while (end_this_line < end_all && (*end_this_line)[1] <= y)
      end_this_line++;
    
    // update active_list accordingly
    { line_list::iterator first = next;
      while (first != end_this_line) {
        // examine previous line
	{ polygon_segment<T>::const_iterator
	    start = first->previous(),
	    end = first->Vertex;
	  if ((*start)[1] != (*end)[1]) {
	    // not a horizontal line
	    if ((*start)[1] > (*end)[1]) {
	      // start below end, end == current vertex => make active
	      active_vertices.push_back(vertex_identifier<T>(first->Segment,start));
	      }
	    else {
	      // start above end, end == current vertex => deactivate
	      FOREACH(first,active_vertices,line_list)
	        if (first->Vertex == start) {
		  active_vertices.erase(first);
		  break;
		  }
	      }
	    }
	  }

        // examine next line
	{ polygon_segment<T>::const_iterator
	    start = first->Vertex,
	    end = first->next();
	  if ((*start)[1] != (*end)[1]) {
	    // not a horizontal line
	    if ((*start)[1] > (*end)[1]) {
	      // start below end, start == current vertex => deactivate
	      FOREACH(first,active_vertices,line_list)
	        if (first->Vertex == start) {
		  active_vertices.erase(first);
		  break;
		  }
	      }
	    else {
	      // start above end, start == current vertex => make active
	      active_vertices.push_back(vertex_identifier<T>(first->Segment,start));
	      }
	    }
	  }
	
	// next
	first++;
        }
      }
    
    // create sorted list of intersection points
    // of active lines with current scanline
    // assuming horizontal lines never make it onto the active list
    intersections.clear();
    FOREACH_CONST(first,active_vertices,line_list) {
      polygon_segment<T>::const_iterator start = first->Vertex;
      polygon_segment<T>::const_iterator end = first->next();
      
      T dx = (*end)[0] - (*start)[0];
      T dy = (*end)[1] - (*start)[1];
      intersections.push_back((*start)[0] + (y - (*start)[1]) * dx / dy);
      }
    sort(intersections.begin(),intersections.end());
    
    // draw hlines appropriately
    // assuming we have an even number of intersections
    { intersection_list::const_iterator
        first = intersections.begin(),last = intersections.end();
      while (first != last) {
        hlr(*first,y,*(first+1));
	first += 2;
	}
      }
    
    
    next = end_this_line;
    }
  }




template<class T>
void ixion::polygon<T>::freeSegments() {
  FOREACH_CONST(first,*this,polygon)
    delete *first;
  }

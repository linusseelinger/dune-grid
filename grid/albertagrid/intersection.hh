// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:
#ifndef DUNE_ALBERTA_INTERSECTION_HH
#define DUNE_ALBERTA_INTERSECTION_HH

#include <dune/common/smallobject.hh>

#include <dune/grid/common/intersection.hh>

#include <dune/grid/albertagrid/transformation.hh>
#include <dune/grid/albertagrid/agmemory.hh>
#include <dune/grid/albertagrid/elementinfo.hh>
#include <dune/grid/albertagrid/geometry.hh>

#define ALBERTA_CACHED_LOCAL_INTERSECTION_GEOMETRIES 1

namespace Dune
{

  // External Forward Declarations
  // -----------------------------

  template< int codim, int dim, class GridImp >
  class AlbertaGridEntity;



  // AlbertaGridIntersectionBase
  // ---------------------------

  template< class Grid >
  class AlbertaGridIntersectionBase
    : public SmallObject
  {
    typedef AlbertaGridIntersectionBase< Grid > This;

  public:
    typedef typename Grid::ctype ctype;

    static const int dimension = Grid::dimension;
    static const int dimensionworld = Grid::dimensionworld;

    typedef FieldVector< ctype, dimensionworld > NormalVector;
    typedef FieldVector< ctype, dimension-1 > LocalCoordType;

    typedef typename Grid::template Codim< 0 >::Entity Entity;
    typedef typename Grid::template Codim< 0 >::EntityPointer EntityPointer;

    typedef typename Grid::template Codim< 1 >::Geometry Geometry;
    typedef typename Grid::template Codim< 1 >::LocalGeometry LocalGeometry;

    typedef Alberta::ElementInfo< dimension > ElementInfo;

  protected:
    typedef AlbertaGridEntity< 0, dimension, Grid > EntityImp;
    typedef AlbertaGridGeometry< dimension-1, dimensionworld, Grid > GeometryImp;
    typedef AlbertaGridGeometry< dimension-1, dimension, Grid > LocalGeometryImp;

    struct GlobalCoordReader;
    struct LocalCoordReader;

  public:
    AlbertaGridIntersectionBase ( const EntityImp &entity, const int oppVertex );

    EntityPointer inside () const;

    bool boundary () const;
    int boundaryId () const;
    unsigned int boundaryIndex () const;

    int indexInInside () const;

    GeometryType type () const;

    const NormalVector integrationOuterNormal ( const LocalCoordType &local ) const;
    const NormalVector outerNormal ( const LocalCoordType &local ) const;
    const NormalVector unitOuterNormal ( const LocalCoordType &local ) const;


    AlbertaTransformation transformation () const;


    const Grid &grid () const;
    const ElementInfo &elementInfo () const;

  protected:
    const Grid *grid_;
    ElementInfo elementInfo_;
    int oppVertex_;
  };



  // AlbertaGridLeafIntersection
  // ---------------------------

  template< class GridImp >
  class AlbertaGridLeafIntersection
    : public AlbertaGridIntersectionBase< GridImp >
  {
    typedef AlbertaGridLeafIntersection< GridImp > This;
    typedef AlbertaGridIntersectionBase< GridImp > Base;

    friend class AlbertaGridEntity< 0, GridImp::dimension, GridImp >;

  public:
    typedef This ImplementationType;

    static const int dimension = Base::dimension;

    typedef typename Base::NormalVector NormalVector;
    typedef typename Base::LocalCoordType LocalCoordType;

    typedef typename Base::Entity Entity;
    typedef typename Base::EntityPointer EntityPointer;

    typedef typename Base::Geometry Geometry;
    typedef typename Base::LocalGeometry LocalGeometry;

    typedef typename Base::ElementInfo ElementInfo;

  protected:
    typedef typename Base::EntityImp EntityImp;
    typedef typename Base::GeometryImp GeometryImp;
    typedef typename Base::LocalGeometryImp LocalGeometryImp;

    typedef typename Base::GlobalCoordReader GlobalCoordReader;
    typedef typename Base::LocalCoordReader LocalCoordReader;

    using Base::grid;
    using Base::elementInfo;

  public:
    using Base::inside;

    AlbertaGridLeafIntersection ( const EntityImp &entity, const int n );

    AlbertaGridLeafIntersection ( const This &other );

    This &operator= ( const This &other );

    bool operator== ( const This &other ) const;

    void next ();

    EntityPointer outside () const;

    bool neighbor () const;

    bool conforming () const;

    const LocalGeometry &geometryInInside () const;
    const LocalGeometry &geometryInOutside () const;

    const Geometry &geometry () const;

    int indexInOutside () const;


    int twistInInside () const;
    int twistInOutside () const;

  protected:
    using Base::oppVertex_;

  private:
    mutable ElementInfo neighborInfo_;
    mutable MakeableInterfaceObject< Geometry > geo_;
#if not ALBERTA_CACHED_LOCAL_INTERSECTION_GEOMETRIES
    mutable MakeableInterfaceObject< LocalGeometry > fakeNeighObj_;
    mutable MakeableInterfaceObject< LocalGeometry > fakeSelfObj_;
#endif
  };

}

#endif // #ifndef DUNE_ALBERTA_INTERSECTION_HH

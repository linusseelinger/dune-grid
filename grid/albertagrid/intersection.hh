// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:
#ifndef DUNE_ALBERTA_INTERSECTION_HH
#define DUNE_ALBERTA_INTERSECTION_HH

#include <dune/grid/common/intersection.hh>
#include <dune/grid/common/intersectioniterator.hh>

namespace Dune
{

  //**********************************************************************
  //
  // --AlbertaGridIntersectionIterator
  // --IntersectionIterator
  /*!
     Mesh entities of codimension 0 ("elements") allow to visit all neighbors, where
     a neighbor is an entity of codimension 0 which has a common entity of codimension 1
     These neighbors are accessed via a IntersectionIterator. This allows the implementation of
     non-matching meshes. The number of neigbors may be different from the number of faces
     of an element!
   */
  template< class GridImp >
  class AlbertaGridIntersectionIterator
  {
    typedef AlbertaGridIntersectionIterator This;

    friend class AlbertaGridEntity< 0, GridImp::dimension, GridImp >;

  public:
    //! define type used for coordinates in grid module
    typedef typename GridImp::ctype ctype;

    //! know your own dimension
    static const int dimension = GridImp::dimension;
    //! know your own dimension of world
    static const int dimensionworld = GridImp::dimensionworld;

    typedef Dune::Intersection< GridImp, Dune::AlbertaGridIntersectionIterator >
    Intersection;
    typedef This ImplementationType;

    typedef AGMemoryProvider< This > StorageType;
    typedef typename GridImp::template Codim<0>::Entity Entity;
    typedef typename GridImp::template Codim<0>::EntityPointer EntityPointer;

    typedef typename GridImp::template Codim<1>::Geometry Geometry;
    typedef typename GridImp::template Codim<1>::LocalGeometry LocalGeometry;

    typedef AlbertaGridEntity< 0, dimension, GridImp > EntityImp;
    typedef AlbertaGridGeometry< dimension-1, dimensionworld, GridImp > GeometryImp;
    typedef AlbertaGridGeometry< dimension-1, dimension, GridImp > LocalGeometryImp;

    const Intersection &dereference () const
    {
      return reinterpret_cast< const Intersection & >( *this );
    }

    //! equality
    bool equals ( const This &other ) const;

    //! increment
    void increment();

    //! access neighbor
    EntityPointer outside() const;

    //! access element where IntersectionIterator started
    EntityPointer inside() const;

    //! The default Constructor
    AlbertaGridIntersectionIterator ( const GridImp &grid, int level );

    //! The Constructor
    AlbertaGridIntersectionIterator ( const GridImp & grid, int level,
                                      ALBERTA EL_INFO *elInfo, bool leafIt );
    //! The copy constructor
    AlbertaGridIntersectionIterator( const This &other );

    //! assignment operator, implemented because default does not the right thing
    void assign ( const This &other );

    //! The Destructor
    //~AlbertaGridIntersectionIterator();

    //! return true if intersection is with boundary.
    bool boundary () const;

    //! return true if across the edge an neighbor on this level exists
    bool neighbor () const;

    //! return information about the Boundary
    int boundaryId () const;

    //! return true if intersection is conform.
    bool conforming () const;

    //! intersection of codimension 1 of this neighbor with element where
    //! iteration started.
    //! Here returned element is in LOCAL coordinates of the element
    //! where iteration started.
    const LocalGeometry& intersectionSelfLocal () const;
    /*! intersection of codimension 1 of this neighbor with element where iteration started.
       Here returned element is in LOCAL coordinates of neighbor
     */
    const LocalGeometry& intersectionNeighborLocal () const;
    /*! intersection of codimension 1 of this neighbor with element where iteration started.
       Here returned element is in GLOBAL coordinates of the element where iteration started.
     */
    const Geometry& intersectionGlobal () const;

    //! local number of codim 1 entity in self where intersection is contained in
    int numberInSelf () const;
    //! local number of codim 1 entity in neighbor where intersection is contained in
    int numberInNeighbor () const;

    //! twist of the face seen from the inner element
    int twistInSelf() const;

    //! twist of the face seen from the outer element
    int twistInNeighbor() const;

    //! return unit outer normal, this should be dependent on local
    //! coordinates for higher order boundary
    typedef FieldVector< ctype, GridImp::dimensionworld > NormalVector;
    typedef FieldVector< ctype, GridImp::dimension-1 > LocalCoordType;

    const NormalVector unitOuterNormal ( const LocalCoordType &local ) const;

    //! return outer normal, this should be dependent on local
    //! coordinates for higher order boundary
    const NormalVector outerNormal ( const LocalCoordType &local ) const;

    //! return outer normal, this should be dependent on local
    //! coordinates for higher order boundary
    const NormalVector integrationOuterNormal ( const LocalCoordType &local ) const;

    //! return level of inside entity
    int level () const;

    //**********************************************************
    //  private methods
    //**********************************************************

    // reset IntersectionIterator
    void first ( const EntityImp &entity, int level );

    // calls EntityPointer done and sets done_ to true
    void done ();

  private:
    // returns true if actual neighbor has same level
    bool neighborHasSameLevel () const;

    //! make Iterator set to begin of actual entitys intersection Iterator
    void makeBegin (const GridImp & grid,
                    int level,
                    ALBERTA EL_INFO * elInfo ) const;

    //! set Iterator to end of actual entitys intersection Iterator
    void makeEnd (const GridImp & grid,int level ) const;

    // put objects on stack
    void freeObjects () const;

    //! setup the virtual neighbor
    void setupVirtEn () const;

    //! calculate normal to current face
    void calcOuterNormal ( NormalVector &n ) const;

    // return whether the iterator was called from a LeafIterator entity or
    // LevelIterator entity
    bool leafIt () const { return leafIt_; }
    ////////////////////////////////////////////////
    // private member variables
    ////////////////////////////////////////////////

    //! know the grid were im coming from
    const GridImp& grid_;

    //! the actual level
    mutable int level_;

    //! count on which neighbor we are lookin' at
    mutable int neighborCount_;

    //! implement with virtual element
    //! Most of the information can be generated from the ALBERTA EL_INFO
    //! therefore this element is only created on demand.
    mutable bool builtNeigh_;

    bool leafIt_;

    //! pointer to the EL_INFO struct storing the real element information
    mutable ALBERTA EL_INFO * elInfo_;

    // the objects holding the real implementations
    mutable MakeableInterfaceObject< LocalGeometry > fakeNeighObj_;
    mutable MakeableInterfaceObject< LocalGeometry > fakeSelfObj_;
    mutable MakeableInterfaceObject< Geometry > neighGlobObj_;

    //! EL_INFO th store the information of the neighbor if needed
    mutable ALBERTA EL_INFO neighElInfo_;

    // twist seen from the neighbor
    mutable int twist_;
  };

}

#endif

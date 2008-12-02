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
  template<class GridImp>
  class AlbertaGridIntersectionIterator
  {
    enum { dim      = GridImp::dimension };
    enum { dimworld = GridImp::dimensionworld };

    friend class AlbertaGridEntity<0,dim,GridImp>;
    typedef AlbertaGridIntersectionIterator<GridImp> ThisType;

  public:
    typedef Dune::Intersection< GridImp, Dune::AlbertaGridIntersectionIterator >
    Intersection;
    typedef ThisType ImplementationType;

    typedef AGMemoryProvider< ThisType > StorageType;
    typedef typename GridImp::template Codim<0>::Entity Entity;
    typedef typename GridImp::template Codim<1>::Geometry Geometry;
    typedef typename GridImp::template Codim<1>::LocalGeometry LocalGeometry;

    typedef MakeableInterfaceObject< Entity > EntityObject;
    typedef typename EntityObject::ImplementationType EntityImp;

    //typedef AlbertaGridMakeableGeometry<dim-1,dimworld,GridImp> LocalGeometryImp;
    typedef AlbertaGridGeometry<dim-1,dimworld,GridImp> LocalGeometryImp;
    typedef typename GridImp::template Codim<0>::EntityPointer EntityPointer;

    //! know your own dimension
    enum { dimension=dim };
    //! know your own dimension of world
    enum { dimensionworld=dimworld };
    //! define type used for coordinates in grid module
    typedef typename GridImp::ctype ctype;

    const Intersection &dereference () const
    {
      return reinterpret_cast< const Intersection & >( *this );
    }

    //! equality
    bool equals (const AlbertaGridIntersectionIterator<GridImp> & i) const;

    //! increment
    void increment();

    //! equality
    bool operator==(const AlbertaGridIntersectionIterator<GridImp>& i) const;

    //! access neighbor
    EntityPointer outside() const;

    //! access element where IntersectionIterator started
    EntityPointer inside() const;

    //! The default Constructor
    AlbertaGridIntersectionIterator(const GridImp & grid,
                                    int level);

    //! The Constructor
    AlbertaGridIntersectionIterator(const GridImp & grid,
                                    int level,
                                    ALBERTA EL_INFO *elInfo,
                                    bool leafIt );
    //! The copy constructor
    AlbertaGridIntersectionIterator(const AlbertaGridIntersectionIterator<GridImp> & org);

    //! assignment operator, implemented because default does not the right thing
    void assign (const AlbertaGridIntersectionIterator<GridImp> & org);

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
    typedef FieldVector<albertCtype, GridImp::dimensionworld> NormalVecType;
    typedef FieldVector<albertCtype, GridImp::dimension-1> LocalCoordType;

    const NormalVecType & unitOuterNormal (const LocalCoordType & local) const;

    //! return outer normal, this should be dependent on local
    //! coordinates for higher order boundary
    const NormalVecType & outerNormal (const LocalCoordType & local) const;

    //! return outer normal, this should be dependent on local
    //! coordinates for higher order boundary
    const NormalVecType & integrationOuterNormal (const LocalCoordType & local) const;

    //! return level of inside entity
    int level () const;

    //**********************************************************
    //  private methods
    //**********************************************************

    // reset IntersectionIterator
    template <class EntityType>
    void first(const EntityType & en, int level );

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
    void calcOuterNormal () const;

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

    typedef MakeableInterfaceObject<LocalGeometry> LocalGeometryObject;

    // the objects holding the real implementations
    mutable LocalGeometryObject fakeNeighObj_;
    mutable LocalGeometryObject fakeSelfObj_;
    mutable LocalGeometryObject neighGlobObj_;

    //! pointer to element holding the intersectionNeighbourLocal information.
    //! This element is created on demand.
    mutable LocalGeometryImp & fakeNeigh_;
    //! pointer to element holding the intersectionSelfLocal information.
    //! This element is created on demand.
    mutable LocalGeometryImp & fakeSelf_;
    //! pointer to element holding the neighbor_global and neighbor_local
    //! information. This element is created on demand.
    mutable LocalGeometryImp & neighGlob_;

    //! EL_INFO th store the information of the neighbor if needed
    mutable ALBERTA EL_INFO neighElInfo_;

    mutable NormalVecType outNormal_;
    mutable NormalVecType unitNormal_;

    // tmp memory for normal calculation
    mutable FieldVector<albertCtype, dimworld> tmpU_;
    mutable FieldVector<albertCtype, dimworld> tmpV_;

    // twist seen from the neighbor
    mutable int twist_;

    //! is true when iterator finished
    bool done_;
  };

}

#endif

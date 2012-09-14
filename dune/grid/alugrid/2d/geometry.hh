// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:
#ifndef DUNE_ALUGRID_2D_GEOMETRY_HH
#define DUNE_ALUGRID_2D_GEOMETRY_HH

#include <dune/common/misc.hh>
#include <dune/grid/common/grid.hh>
#include <dune/geometry/genericgeometry/topologytypes.hh>

#include <dune/grid/alugrid/2d/alu2dinclude.hh>
#include <dune/grid/alugrid/3d/mappings.hh>

namespace Dune
{

  // External Forward declarations
  // -----------------------------

  template< int cd, int dim, class GridImp >
  class ALU2dGridEntity;

  template<int cd, class GridImp >
  class ALU2dGridEntityPointer;

  template< int mydim, int cdim, class GridImp >
  class ALU2dGridGeometry;

  template< int dim, int dimworld, ALU2DSPACE ElementType eltype >
  class ALU2dGrid;



  // MyALU2dGridGeometryImpl
  // -----------------------

  template< int mydim, int cdim, ALU2DSPACE ElementType eltype >
  class MyALU2dGridGeometryImpl;

  // geometry implementation for vertices
  template< int cdim, ALU2DSPACE ElementType eltype >
  class MyALU2dGridGeometryImpl< 0, cdim, eltype >
  {
    typedef LinearMapping< cdim, 0 > MappingType;

    typedef typename MappingType::ctype ctype;

    typedef typename MappingType::map_t map_t;
    typedef typename MappingType::world_t world_t;

    typedef typename MappingType::matrix_t matrix_t;
    typedef typename MappingType::inv_t inv_t;

    MappingType mapping_;
    bool valid_ ;

    const MappingType& mapping() const
    {
      assert( valid() );
      return mapping_;
    }

  public:
    MyALU2dGridGeometryImpl() : mapping_(), valid_( false ) {}

    // returns true if goemetry info is valid
    bool valid () const { return valid_; }

    // reset geometry status
    void invalidate() { valid_ = false ; }

    bool affine() const
    {
      assert( valid() );
      return mapping().affine();
    }

    int corners () const
    {
      return 1;
    }

    GeometryType type () const
    {
      return GeometryType(
               (eltype == ALU2DSPACE triangle ?
                GenericGeometry :: SimplexTopology< 0 > :: type :: id :
                GenericGeometry :: CubeTopology   < 0 > :: type :: id),
               0 );
    }

    void map2world ( const map_t &m, world_t &w ) const
    {
      return mapping().map2world( m, w );
    }

    void world2map ( const world_t &w, map_t &m ) const
    {
      return mapping().world2map( w, m );
    }

    const matrix_t &jacobianTransposed ( const map_t &m ) const
    {
      return mapping().jacobianTransposed( m );
    }

    const inv_t &jacobianInverseTransposed ( const map_t &m ) const
    {
      return mapping().jacobianInverseTransposed( m );
    }

    ctype det ( const map_t &m ) const
    {
      return mapping().det( m );
    }

    // update geometry coordinates
    template< class Vector >
    void update ( const Vector &p0 )
    {
      mapping_.buildMapping( p0 );
      valid_ = true ;
    }
  };

  // geometry implementation for lines
  template< int cdim, ALU2DSPACE ElementType eltype >
  class MyALU2dGridGeometryImpl< 1, cdim, eltype >
  {
    static const int ncorners = 2;

    typedef LinearMapping< cdim, 1 > MappingType;

    typedef typename MappingType::ctype ctype;

    typedef typename MappingType::map_t map_t;
    typedef typename MappingType::world_t world_t;

    typedef typename MappingType::matrix_t matrix_t;
    typedef typename MappingType::inv_t inv_t;

    MappingType mapping_;
    bool valid_;

    const MappingType& mapping() const
    {
      assert( valid() );
      return mapping_;
    }

  public:
    MyALU2dGridGeometryImpl() : mapping_(), valid_( false ) {}

    // returns true if goemetry info is valid
    bool valid () const { return valid_; }

    // reset geometry status
    void invalidate() { valid_ = false ; }

    bool affine() const
    {
      return mapping().affine();
    }

    int corners () const
    {
      return ncorners;
    }

    GeometryType type () const
    {
      return GeometryType(
               (eltype == ALU2DSPACE triangle ?
                GenericGeometry :: SimplexTopology< 1 > :: type :: id :
                GenericGeometry :: CubeTopology   < 1 > :: type :: id),
               1 );
    }

    void map2world ( const map_t &m, world_t &w ) const
    {
      return mapping().map2world( m, w );
    }

    void world2map ( const world_t &w, map_t &m ) const
    {
      return mapping().world2map( w, m );
    }

    const matrix_t &jacobianTransposed ( const map_t &m ) const
    {
      return mapping().jacobianTransposed( m );
    }

    const inv_t &jacobianInverseTransposed ( const map_t &m ) const
    {
      return mapping().jacobianInverseTransposed( m );
    }

    ctype det ( const map_t &m ) const
    {
      return mapping().det( m );
    }

    // update geometry in father coordinates
    template< class Geo, class LocalGeo >
    void updateLocal ( const Geo &geo, const LocalGeo &localGeo )
    {
      assert( localGeo.corners() == ncorners );
      // compute the local coordinates in father refelem
      FieldMatrix< alu2d_ctype, ncorners, cdim > coord;
      for( int i = 0; i < ncorners; ++i )
      {
        // calculate coordinate
        coord[ i ] = geo.local( localGeo.corner( i ) );
        // to avoid rounding errors
        for( int j = 0; j < cdim; ++j )
          coord[ i ][ j ] = (coord[ i ][ j ] < 1e-14 ? 0 : coord[ i ][ j ]);
      }
      mapping_.buildMapping( coord[ 0 ], coord[ 1 ] );
      valid_ = true ;
    }

    // update geometry coordinates
    template< class Vector >
    void update ( const Vector &p0, const Vector &p1 )
    {
      mapping_.buildMapping( p0, p1 );
      valid_ = true ;
    }
  };

  // geometry implementation for triangles
  template< int cdim >
  class MyALU2dGridGeometryImpl< 2, cdim, ALU2DSPACE triangle >
  {
    static const int ncorners = 3;

    typedef LinearMapping< cdim, 2 > MappingType;

    typedef typename MappingType::ctype ctype;

    typedef typename MappingType::map_t map_t;
    typedef typename MappingType::world_t world_t;

    typedef typename MappingType::matrix_t matrix_t;
    typedef typename MappingType::inv_t inv_t;

    MappingType mapping_;
    bool valid_;

    const MappingType& mapping() const
    {
      assert( valid() );
      return mapping_;
    }

  public:
    MyALU2dGridGeometryImpl() : mapping_(), valid_( false ) {}

    // returns true if goemetry info is valid
    bool valid () const { return valid_; }

    // reset geometry status
    void invalidate() { valid_ = false ; }

    bool affine () const
    {
      return mapping().affine();
    }

    int corners () const
    {
      return ncorners;
    }

    GeometryType type () const
    {
      return GeometryType( GenericGeometry :: SimplexTopology< 2 > :: type :: id , 2 );
    }

    void map2world ( const map_t &m, world_t &w ) const
    {
      return mapping().map2world( m, w );
    }

    void world2map ( const world_t &w, map_t &m ) const
    {
      return mapping().world2map( w, m );
    }

    const matrix_t &jacobianTransposed ( const map_t &m ) const
    {
      return mapping().jacobianTransposed( m );
    }

    const inv_t &jacobianInverseTransposed ( const map_t &m ) const
    {
      return mapping().jacobianInverseTransposed( m );
    }

    ctype det ( const map_t &m ) const
    {
      return mapping().det( m );
    }

    // update geometry in father coordinates
    template< class Geo, class LocalGeo >
    void updateLocal ( const Geo &geo, const LocalGeo &localGeo )
    {
      assert( localGeo.corners() == ncorners );
      // compute the local coordinates in father refelem
      FieldMatrix< alu2d_ctype, ncorners, cdim > coord;
      for( int i = 0; i < ncorners; ++i )
      {
        // calculate coordinate
        coord[ i ] = geo.local( localGeo.corner( i ) );
        // to avoid rounding errors
        for( int j = 0; j < cdim; ++j )
          coord[ i ][ j ] = (coord[ i ][ j ] < 1e-14 ? 0 : coord[ i ][ j ]);
      }
      mapping_.buildMapping( coord[ 0 ], coord[ 1 ], coord[ 2 ] );
      valid_ = true ;
    }

    template< class HElement >
    void update ( const HElement &item )
    {
      mapping_.buildMapping( item.getVertex( 0 )->coord(), item.getVertex( 1 )->coord(),
                             item.getVertex( 2 )->coord() );
      valid_ = true ;
    }
  };

  // geometry implementation for quadrilaterals
  template< int cdim >
  class MyALU2dGridGeometryImpl< 2, cdim, ALU2DSPACE quadrilateral >
  {
    static const int ncorners = 4;

    typedef BilinearMapping< cdim > MappingType;

    typedef typename MappingType::ctype ctype;

    typedef typename MappingType::map_t map_t;
    typedef typename MappingType::world_t world_t;

    typedef typename MappingType::matrix_t matrix_t;
    typedef typename MappingType::inv_t inv_t;

    MappingType mapping_;
    bool valid_ ;

    const MappingType& mapping() const
    {
      assert( valid() );
      return mapping_;
    }

  public:
    MyALU2dGridGeometryImpl() : mapping_(), valid_( false ) {}

    // returns true if goemetry info is valid
    bool valid () const { return valid_; }

    // reset geometry status
    void invalidate() { valid_ = false ; }

    bool affine () const
    {
      return mapping().affine();
    }

    int corners () const
    {
      return ncorners;
    }

    GeometryType type () const
    {
      return GeometryType( GeometryType::cube, 2 );
    }

    void map2world ( const map_t &m, world_t &w ) const
    {
      return mapping().map2world( m, w );
    }

    void world2map ( const world_t &w, map_t &m ) const
    {
      return mapping().world2map( w, m );
    }

    const matrix_t &jacobianTransposed ( const map_t &m ) const
    {
      return mapping().jacobianTransposed( m );
    }

    const inv_t &jacobianInverseTransposed ( const map_t &m ) const
    {
      return mapping().jacobianInverseTransposed( m );
    }

    ctype det ( const map_t &m ) const
    {
      return mapping().det( m );
    }

    // update geometry in father coordinates
    template< class Geo, class LocalGeo >
    void updateLocal ( const Geo &geo, const LocalGeo &localGeo )
    {
      assert( localGeo.corners() == ncorners );
      // compute the local coordinates in father refelem
      FieldMatrix< alu2d_ctype, ncorners, cdim > coord;
      for( int i = 0; i < ncorners; ++i )
      {
        // calculate coordinate
        coord[ i ] = geo.local( localGeo.corner( i ) );
        // to avoid rounding errors
        for( int j = 0; j < cdim; ++j )
          coord[ i ][ j ] = (coord[ i ][ j ] < 1e-14 ? 0 : coord[ i ][ j ]);
      }
      mapping_.buildMapping( coord[ 0 ], coord[ 1 ], coord[ 2 ], coord[ 3 ] );
      valid_ = true ;
    }

    template< class HElement >
    void update ( const HElement &item )
    {
      mapping_.buildMapping( item.getVertex( 0 )->coord(), item.getVertex( 1 )->coord(),
                             item.getVertex( 3 )->coord(), item.getVertex( 2 )->coord() );
      valid_ = true ;
    }
  };

  // geometry implementation for triangles
  template< int cdim >
  class MyALU2dGridGeometryImpl< 2, cdim, ALU2DSPACE mixed >
  {
    typedef Dune::LinearMapping< cdim, 2 > LinearMapping;
    typedef Dune::BilinearMapping< cdim > BilinearMapping;

    typedef typename LinearMapping::ctype ctype;

    typedef typename LinearMapping::map_t map_t;
    typedef typename LinearMapping::world_t world_t;

    typedef typename LinearMapping::matrix_t matrix_t;
    typedef typename LinearMapping::inv_t inv_t;

    static const int lms = sizeof( LinearMapping );
    static const int bms = sizeof( BilinearMapping );

    char mapping_[ lms > bms ? lms : bms ];
    int corners_;
    bool valid_ ;

  public:
    MyALU2dGridGeometryImpl () : corners_( 0 ), valid_( false ) {}

    MyALU2dGridGeometryImpl ( const MyALU2dGridGeometryImpl &other )
      : corners_( other.corners() ), valid_( other.valid_ )
    {
      if( corners_ == 3 )
        new( &mapping_ )LinearMapping( other.linearMapping() );
      if( corners_ == 4 )
        new( &mapping_ )BilinearMapping( other.bilinearMapping() );
    }

    // returns true if goemetry info is valid
    bool valid () const { return valid_; }

    // reset geometry status
    void invalidate() { valid_ = false ; }

    bool affine () const
    {
      return (corners() == 3 ? linearMapping().affine() : bilinearMapping().affine());
    }

    int corners () const
    {
      return corners_;
    }

    GeometryType type () const
    {
      return GeometryType( (corners_ == 3 ?
                            GenericGeometry :: SimplexTopology< 2 > :: type :: id :
                            GenericGeometry :: CubeTopology   < 2 > :: type :: id), 2);
    }

    void map2world ( const map_t &m, world_t &w ) const
    {
      if( corners() == 3 )
        linearMapping().map2world( m, w );
      else
        bilinearMapping().map2world( m, w );
    }

    void world2map ( const world_t &w, map_t &m ) const
    {
      if( corners() == 3 )
        linearMapping().world2map( w, m );
      else
        bilinearMapping().world2map( w, m );
    }

    const matrix_t &jacobianTransposed ( const map_t &m ) const
    {
      return (corners() == 3 ? linearMapping().jacobianTransposed( m ) : bilinearMapping().jacobianTransposed( m ));
    }

    const inv_t &jacobianInverseTransposed ( const map_t &m ) const
    {
      return (corners() == 3 ? linearMapping().jacobianInverseTransposed( m ) : bilinearMapping().jacobianInverseTransposed( m ));
    }

    ctype det ( const map_t &m ) const
    {
      return (corners() == 3 ? linearMapping().det( m ) : bilinearMapping().det( m ));
    }

    // update geometry in father coordinates
    template< class Geo, class LocalGeo >
    void updateLocal ( const Geo &geo, const LocalGeo &localGeo )
    {
      const int corners = localGeo.corners();

      // compute the local coordinates in father refelem
      FieldMatrix< alu2d_ctype, 4, cdim > coord;
      for( int i = 0; i < corners; ++i )
      {
        // calculate coordinate
        coord[ i ] = geo.local( localGeo.corner( i ) );
        // to avoid rounding errors
        for( int j = 0; j < cdim; ++j )
          coord[ i ][ j ] = (coord[ i ][ j ] < 1e-14 ? 0 : coord[ i ][ j ]);
      }

      updateMapping( corners );
      if( corners == 3 )
        linearMapping().buildMapping( coord[ 0 ], coord[ 1 ], coord[ 2 ] );
      else
        bilinearMapping().buildMapping( coord[ 0 ], coord[ 1 ], coord[ 2 ], coord[ 3 ] );

      valid_ = true ;
    }

    template< class HElement >
    void update ( const HElement &item )
    {
      const int corners = item.numvertices();
      updateMapping( corners );
      if( corners == 3 )
        linearMapping().buildMapping( item.getVertex( 0 )->coord(), item.getVertex( 1 )->coord(),
                                      item.getVertex( 2 )->coord() );
      else
        bilinearMapping().buildMapping( item.getVertex( 0 )->coord(), item.getVertex( 1 )->coord(),
                                        item.getVertex( 3 )->coord(), item.getVertex( 2 )->coord() );

      valid_ = true ;
    }

  private:
    MyALU2dGridGeometryImpl &operator= ( const MyALU2dGridGeometryImpl &other );

    const LinearMapping &linearMapping () const
    {
      assert( valid() );
      return static_cast< const LinearMapping * >( &mapping_ );
    }

    LinearMapping &linearMapping ()
    {
      assert( valid() );
      return static_cast< LinearMapping * >( &mapping_ );
    }

    const BilinearMapping &bilinearMapping () const
    {
      assert( valid() );
      return static_cast< const BilinearMapping * >( &mapping_ );
    }

    BilinearMapping &bilinearMapping ()
    {
      assert( valid() );
      return static_cast< BilinearMapping * >( &mapping_ );
    }

    void updateMapping ( const int corners )
    {
      assert( (corners == 3) || (corners == 4) );
      if( corners != corners_ )
      {
        destroyMapping();
        corners = corners_;
        if( corners == 3 )
          new( &mapping_ )LinearMapping;
        else
          new( &mapping_ )BilinearMapping;
      }
    }

    void destroyMapping ()
    {
      if( corners() == 3 )
        linearMapping().~LinearMapping();
      else if( corners() == 4 )
        bilinearMapping().~BilinearMapping();
    }
  };


  // ALU2dGridGeometry
  // -----------------

  template< int mydim, int cdim, class GridImp >
  class ALU2dGridGeometry
    : public GeometryDefaultImplementation< mydim, cdim, GridImp, ALU2dGridGeometry >
  {
    static const ALU2DSPACE ElementType eltype = GridImp::elementType;

    //! type of our Geometry interface
    typedef typename GridImp::template Codim<0>::Geometry Geometry;
    //! type of our Geometry implementation
    typedef ALU2dGridGeometry<mydim,cdim,GridImp> GeometryImp;

    typedef typename ALU2dImplTraits< GridImp::dimensionworld, eltype >::HElementType HElementType ;
    typedef typename ALU2dImplInterface< 0, GridImp::dimensionworld, eltype >::Type VertexType;

    // type of specialized geometry implementation
    typedef MyALU2dGridGeometryImpl< mydim, cdim, eltype > GeometryImplType;

  public:
    typedef FieldVector< alu2d_ctype, cdim > GlobalCoordinate;
    typedef FieldVector< alu2d_ctype, mydim > LocalCoordinate;

  public:
    //! for makeRefGeometry == true a Geometry with the coordinates of the
    //! reference element is made
    ALU2dGridGeometry();

    //! return the element type identifier
    //! line , triangle or tetrahedron, depends on dim
    const GeometryType type () const { return geoImpl_.type(); }

    //! return the number of corners of this element. Corners are numbered 0...n-1
    int corners () const { return geoImpl_.corners(); }

    //! access to coordinates of corners. Index is the number of the corner
    GlobalCoordinate corner ( int i ) const;

    //! maps a local coordinate within reference element to
    //! global coordinate in element
    GlobalCoordinate global ( const LocalCoordinate& local ) const;

    //! maps a global coordinate within the element to a
    //! local coordinate in its reference element
    LocalCoordinate local (const GlobalCoordinate& global) const;

    //! A(l) , see grid.hh
    alu2d_ctype integrationElement (const LocalCoordinate& local) const;

    //! return volume of geometry
    alu2d_ctype volume () const;

    //! return true if geometry has affine mapping
    bool affine() const { return geoImpl_.affine(); }

    //! jacobian inverse transposed
    const FieldMatrix<alu2d_ctype,cdim,mydim>& jacobianInverseTransposed (const LocalCoordinate& local) const;

    //! jacobian transposed
    const FieldMatrix<alu2d_ctype,mydim,cdim>& jacobianTransposed (const LocalCoordinate& local) const;

    //***********************************************************************
    //!  Methods that not belong to the Interface, but have to be public
    //***********************************************************************
    //! generate the geometry for out of given ALU2dGridElement
    // method for elements
    bool buildGeom(const HElementType & item);
    // method for edges
    bool buildGeom(const HElementType & item, const int aluFace);
    // method for vertices
    bool buildGeom(const VertexType & item, const int );

    //! build geometry for intersectionSelfLocal and
    //! intersectionNeighborLocal
    template <class GeometryType, class LocalGeomType >
    bool buildLocalGeom(const GeometryType & geo , const LocalGeomType & lg);

    //! build local geometry given local face number
    bool buildLocalGeometry ( int aluFace, int twist, int corners );

    //! return non-const reference to coord vecs
    GlobalCoordinate& getCoordVec (int i);

    //! print internal data
    void print (std::ostream& ss) const;

    //! build geometry with local coords of child in reference element
    inline bool buildGeomInFather(const Geometry &fatherGeom,
                                  const Geometry & myGeom );

    // returns true if geometry information is valid
    inline bool valid() const { return geoImpl_.valid(); }

    // invalidate geometry information
    inline void invalidate() const { geoImpl_.invalidate(); }

  protected:
    // return reference coordinates of the alu triangle
    static std::pair< FieldMatrix< alu2d_ctype, 4, 2 >, FieldVector< alu2d_ctype, 4 > >
    calculateReferenceCoords ( const int corners );

    // implementation of coord and mapping
    mutable GeometryImplType geoImpl_;

    // determinant
    mutable alu2d_ctype det_;
  };



  // Implementation of ALU2dGridGeometry
  // -----------------------------------

  template< int mydim, int cdim, class GridImp >
  inline ALU2dGridGeometry< mydim, cdim, GridImp >::ALU2dGridGeometry ()
    : geoImpl_(),
      det_( 1.0 )
  {}


  template< int mydim, int cdim, class GridImp >
  inline void ALU2dGridGeometry< mydim, cdim, GridImp >::print ( std::ostream &out ) const
  {
    out << "ALU2dGridGeometry< " << mydim << ", " << cdim << " > = ";
    char c = '{';
    for( int i = 0; i < corners(); ++i )
    {
      out << c << " (" << corner( i ) << ")";
      c = ',';
    }
    out << " }" << std::endl;
  }


  template< int mydim, int cdim, class GridImp >
  inline typename ALU2dGridGeometry< mydim, cdim, GridImp >::GlobalCoordinate
  ALU2dGridGeometry< mydim, cdim, GridImp >::corner ( int i ) const
  {
    const ReferenceElement< alu2d_ctype, mydim > &refElement
      = ReferenceElements< alu2d_ctype, mydim >::general( type() );
    return global( refElement.position( i, mydim ) );
  }


  template< int mydim, int cdim, class GridImp >
  inline typename ALU2dGridGeometry< mydim, cdim, GridImp >::GlobalCoordinate
  ALU2dGridGeometry< mydim, cdim, GridImp >::global ( const LocalCoordinate &local ) const
  {
    GlobalCoordinate global;
    geoImpl_.map2world( local, global );
    return global;
  }


  template< int mydim, int cdim, class GridImp >
  inline typename ALU2dGridGeometry< mydim, cdim, GridImp >::LocalCoordinate
  ALU2dGridGeometry< mydim, cdim, GridImp >::local ( const GlobalCoordinate &global ) const
  {
    if( mydim == 0 )
      return LocalCoordinate( 1 );

    LocalCoordinate local;
    geoImpl_.world2map( global, local );
    return local;
  }


  template< int mydim, int cdim, class GridImp >
  inline alu2d_ctype
  ALU2dGridGeometry< mydim, cdim, GridImp >::integrationElement ( const LocalCoordinate &local ) const
  {
    if ( eltype == ALU2DSPACE triangle || mydim < 2 )
    {
      assert( geoImpl_.valid() );
      return (mydim == 0 ? 1.0 : det_);
    }
    else
      return geoImpl_.det(local);
  }


  template< int mydim, int cdim, class GridImp >
  inline alu2d_ctype ALU2dGridGeometry< mydim, cdim, GridImp >::volume () const
  {
    assert( geoImpl_.valid() );
    if( mydim == 2 )
    {
      switch( GridImp::elementType )
      {
      case ALU2DSPACE triangle :
        return 0.5 * det_;

      case ALU2DSPACE quadrilateral :
        return det_;

      case ALU2DSPACE mixed :
        DUNE_THROW( NotImplemented, "Geometry::volume() not implemented for ElementType mixed." );
      }
    }
    else
      return (mydim == 0 ? 1.0 : det_);
  }


  template< int mydim, int cdim, class GridImp >
  inline const FieldMatrix<alu2d_ctype,mydim,cdim>&
  ALU2dGridGeometry< mydim, cdim, GridImp>::jacobianTransposed ( const LocalCoordinate &local ) const
  {
    return geoImpl_.jacobianTransposed( local );
  }


  template< int mydim, int cdim, class GridImp >
  inline const FieldMatrix<alu2d_ctype,cdim,mydim>&
  ALU2dGridGeometry< mydim, cdim, GridImp >::jacobianInverseTransposed ( const LocalCoordinate &local ) const
  {
    return geoImpl_.jacobianInverseTransposed( local );
  }


  //! built Geometry for triangles
  template< int mydim, int cdim, class GridImp >
  inline bool
  ALU2dGridGeometry< mydim, cdim, GridImp >::buildGeom( const HElementType &item )
  {
    // check item
    assert( &item );
    assert( mydim == 2 );

    // update geometry impl
    geoImpl_.update( item );

    // store volume
    det_ = (geoImpl_.corners() == 3 ? 2 * item.area() : item.area());

    // geometry built
    return true;
  }


  template <int mydim, int cdim, class GridImp>
  inline bool ALU2dGridGeometry<mydim,cdim,GridImp>::
  buildGeom(const HElementType & item, const int aluFace)
  {
    assert( mydim == 1 );
    // face 1 is twisted
    const int nf = item.numfaces();

    // for triangles face 1 is twisted and for quatrilaterals faces 1 and 2
    const int twist = (nf == 3) ? (aluFace % 2) : (aluFace>>1)^(aluFace&1);
    // std::cout << nf << " " << aluFace << " " << twist << std::endl;
    //           << " ( " item.getVertex( (aluFace + 1 + twist ) % nf )->coord() ,
    //           << " , " item.getVertex( (aluFace + 2 - twist ) % nf )->coord()
    //           << " ) " << std::endl;

    // check item
    assert( &item );


    // update geometry impl
    geoImpl_.update( item.getVertex( (aluFace + 1 + twist ) % nf )->coord() ,
                     item.getVertex( (aluFace + 2 - twist ) % nf )->coord() );

    // store volume
    det_ = item.sidelength( aluFace );
    //assert( std::abs( det_ - geoImpl_.det( LocalCoordinate(0.5) ) ) < 1e-14 );

    // geometry built
    return true;
  }

  template <int mydim, int cdim, class GridImp>
  inline bool ALU2dGridGeometry<mydim,cdim,GridImp>::
  buildGeom(const VertexType & item , const int )
  {
    assert( mydim == 0 );

    assert( &item );
    // update geometry impl
    geoImpl_.update( item.coord() );

    // volume is already 1.0

    return true;
  }

  template <int mydim, int cdim, class GridImp>
  template <class GeometryType, class LocalGeometryType >
  inline bool ALU2dGridGeometry<mydim,cdim,GridImp>::
  buildLocalGeom(const GeometryType &geo, const LocalGeometryType & localGeom)
  {
    // update geometry
    geoImpl_.updateLocal( geo, localGeom );

    // calculate volume
    LocalCoordinate local( 0.25 );
    det_ = geoImpl_.det( local );

    // geometry built
    return true;
  }

  // built Geometry (faceNumber is in generic numbering)
  template< int mydim, int cdim, class GridImp >
  inline std::pair< FieldMatrix< alu2d_ctype, 4, 2 >, FieldVector< alu2d_ctype, 4 > >
  ALU2dGridGeometry< mydim, cdim, GridImp >::calculateReferenceCoords ( const int corners )
  {
    // calculate reference coordinates of aLUGrid reference triangle

    FieldMatrix< alu2d_ctype, 4, 2 > refCoord( 0. );
    FieldVector< alu2d_ctype, 4 > lengths( 1. );

    // point 1
    refCoord[1][0] = 1.0;
    // point (corners-1)
    refCoord[corners-1][1] = 1.0;
    if( corners == 3 )
    {
      lengths[0] = M_SQRT2;
    }
    else
    {
      // point 2
      refCoord[2][0] = 1.0;
      refCoord[2][1] = 1.0;
    }
    return std::make_pair( refCoord, lengths );
  }


  template< int mydim, int cdim, class GridImp >
  inline bool ALU2dGridGeometry< mydim, cdim, GridImp >
  ::buildLocalGeometry ( int aluFace, int twist, int corners )
  {
    assert( mydim == 1 );
    assert( (twist == 0) || (twist == 1) );

    // get coordinates of reference element
    typedef std::pair< FieldMatrix< alu2d_ctype, 4, 2 >, FieldVector< alu2d_ctype, 4 > > RefCoord;
    RefCoord refCoord( calculateReferenceCoords( corners ) );

    geoImpl_.update( refCoord.first[ ( aluFace + 1+twist ) % corners ],
                     refCoord.first[ ( aluFace + 2-twist ) % corners ] );

    // get length of faces
    det_ = refCoord.second[ aluFace ];

    // geometry built
    return true;
  }

  // built Geometry
  template <int mydim, int cdim, class GridImp >
  inline bool ALU2dGridGeometry<mydim, cdim,GridImp>::
  buildGeomInFather(const Geometry & fatherGeom ,
                    const Geometry & myGeom)
  {
    // update geometry
    geoImpl_.updateLocal( fatherGeom, myGeom );

    // store volume which is a part of one
    det_ = myGeom.volume() / fatherGeom.volume();
    assert( (det_ > 0.0) && (det_ < 1.0) );

    return true;
  }

} // namespace Dune

#endif // #ifndef DUNE_ALUGRID_2D_GEOMETRY_HH

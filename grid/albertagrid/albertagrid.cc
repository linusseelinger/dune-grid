// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:
#ifndef DUNE_ALBERTAGRID_CC
#define DUNE_ALBERTAGRID_CC

//************************************************************************
//
//  implementation of AlbertaGrid
//
//  namespace Dune
//
//************************************************************************
#include "datahandle.hh"

#include "geometry.cc"
#include "entity.cc"
#include "treeiterator.cc"
#include "intersection.cc"

namespace Dune
{

  namespace Alberta
  {
    static void *adaptationDataHandler_;
  }


  template< int dim, int dimworld >
  static void checkAlbertaDimensions ()
  {
    // If this check fails, reconfigure with --with-alberta-world-dim=dimworld
    dune_static_assert( (dimworld == Alberta::dimWorld),
                        "Template Parameter dimworld does not match "
                        "ALBERTA's DIM_OF_WORLD setting." );

#if DUNE_ALBERTA_VERSION < 0x200
    // ALBERTA 1.2 supports only one grid dimension
    // If this check fails, reconfigure with --with-alberta-dim=dim
    dune_static_assert( (dim == DIM),
                        "Template Parameter dim does not match "
                        "ALBERTA's DIM setting." );
#endif
  }


  // AlbertaGrid
  // -----------

  template< int dim, int dimworld >
  inline AlbertaGrid < dim, dimworld >::AlbertaGrid ()
    : mesh_(),
      maxlevel_( 0 ),
      hIndexSet_( *this ),
      idSet_( hIndexSet_ ),
      levelIndexVec_( (size_t)MAXL, 0 ),
      leafIndexSet_ ( 0 ),
      sizeCache_ ( 0 ),
      leafMarkerVector_( dofNumbering_ ),
      levelMarkerVector_( (size_t)MAXL, MarkerVector( dofNumbering_ ) )
  {
    checkAlbertaDimensions< dim, dimworld>();
  }


  template < int dim, int dimworld >
  inline AlbertaGrid< dim, dimworld >
  ::AlbertaGrid ( const std::string &macroGridFileName,
                  const std::string &gridName )
    : mesh_(),
      maxlevel_( 0 ),
      hIndexSet_( *this ),
      idSet_( hIndexSet_ ),
      levelIndexVec_( (size_t)MAXL, 0 ),
      leafIndexSet_ ( 0 ),
      sizeCache_( 0 ),
      leafMarkerVector_( dofNumbering_ ),
      levelMarkerVector_( (size_t)MAXL, MarkerVector( dofNumbering_ ) )
  {
    checkAlbertaDimensions< dim, dimworld >();

    mesh_.create( macroGridFileName, gridName );
    if( !mesh_ )
    {
      DUNE_THROW( AlbertaIOError,
                  "Grid file '" << macroGridFileName
                                << "' is not in ALBERTA macro triangulation format." );
    }

    setup();
    hIndexSet_.create( dofNumbering_ );
    elNewCheck_.initialize( 0 );
    LeafDataType::initLeafDataValues( mesh_, 0 );

    calcExtras();

    std::cout << typeName() << " created from macro grid file '"
              << macroGridFileName << "'." << std::endl;
  }


  template< int dim, int dimworld >
  inline void AlbertaGrid< dim, dimworld >::setup ()
  {
    dofNumbering_.create( mesh_ );

    elNewCheck_.create( dofNumbering_.dofSpace( 0 ), "el_new_check" );
    assert( !elNewCheck_ == false );
    elNewCheck_.template setupInterpolation< ElNewCheckInterpolation >();

#ifndef CALC_COORD
    coordCache_.create( dofNumbering_ );
#endif
  }


  template< int dim, int dimworld >
  inline void AlbertaGrid< dim, dimworld >::removeMesh ()
  {
    for( size_t i = 0; i < levelIndexVec_.size(); ++i )
    {
      if( levelIndexVec_[ i ] != 0 )
        delete levelIndexVec_[ i ];
      levelIndexVec_[ i ] = 0;
    }

    if( leafIndexSet_ != 0 )
      delete leafIndexSet_;
    leafIndexSet_ = 0;

    // release dof vectors
    hIndexSet_.release();
    elNewCheck_.release();
#ifndef CALC_COORD
    coordCache_.release();
#endif
    dofNumbering_.release();

    if( sizeCache_ != 0 )
      delete sizeCache_;
    sizeCache_ = 0;

    mesh_.release();
  }


  template< int dim, int dimworld >
  inline AlbertaGrid< dim, dimworld >::~AlbertaGrid ()
  {
    removeMesh();
  }


  template< int dim, int dimworld >
  template< int codim, PartitionIteratorType pitype >
  inline typename AlbertaGrid< dim, dimworld >::Traits
  ::template Codim< codim >::template Partition<pitype>::LevelIterator
  AlbertaGrid< dim, dimworld >::lbegin ( int level ) const
  {
    typedef AlbertaGridLevelIterator< codim, pitype, const This > LevelIteratorImp;
    assert( level >= 0 );

    if( level > maxlevel_ )
      return lend< codim, pitype >( level );
    MarkerVector &markerVector = levelMarkerVector_[ level ];

    if( (codim > 0) && !markerVector.up2Date() )
      markerVector.template markSubEntities< 1 >( lbegin< 0 >( level ), lend< 0 >( level ) );

    return LevelIteratorImp( *this, &markerVector, level );
  }


  template< int dim, int dimworld >
  template< int codim, PartitionIteratorType pitype >
  inline typename AlbertaGrid< dim, dimworld >::Traits
  ::template Codim< codim >::template Partition< pitype >::LevelIterator
  AlbertaGrid < dim, dimworld >::lend ( int level ) const
  {
    typedef AlbertaGridLevelIterator< codim, pitype, const This > LevelIteratorImp;
    assert( level >= 0 );

    return LevelIteratorImp( *this, level );
  }


  template< int dim, int dimworld >
  template< int codim >
  inline typename AlbertaGrid< dim, dimworld >::Traits
  ::template Codim< codim >::LevelIterator
  AlbertaGrid < dim, dimworld >::lbegin ( int level ) const
  {
    return lbegin< codim, All_Partition >( level );
  }


  template< int dim, int dimworld >
  template< int codim >
  inline typename AlbertaGrid< dim, dimworld >::Traits
  ::template Codim< codim >::LevelIterator
  AlbertaGrid< dim, dimworld >::lend ( int level ) const
  {
    return lend< codim, All_Partition >( level );
  }


  template< int dim, int dimworld >
  template< int codim, PartitionIteratorType pitype >
  inline typename AlbertaGrid< dim, dimworld >::Traits
  ::template Codim< codim >::template Partition< pitype >::LeafIterator
  AlbertaGrid< dim, dimworld >::leafbegin () const
  {
    typedef AlbertaGridLeafIterator< codim, pitype, const This > LeafIteratorImp;

    MarkerVector &markerVector = leafMarkerVector_;
    const int firstMarkedCodim = 2;
    if( (codim >= firstMarkedCodim) && !markerVector.up2Date() )
      markerVector.template markSubEntities< firstMarkedCodim >( leafbegin< 0 >(), leafend< 0 >() );

    return LeafIteratorImp( *this, &markerVector, maxlevel_ );
  }


  template< int dim, int dimworld >
  template< int codim, PartitionIteratorType pitype >
  inline typename AlbertaGrid< dim, dimworld >::Traits
  ::template Codim< codim >::template Partition< pitype >::LeafIterator
  AlbertaGrid< dim, dimworld >::leafend () const
  {
    typedef AlbertaGridLeafIterator< codim, pitype, const This > LeafIteratorImp;
    return LeafIteratorImp( *this, maxlevel_ );
  }


  template< int dim, int dimworld >
  template< int codim >
  inline typename AlbertaGrid<dim,dimworld>::Traits
  ::template Codim< codim >::LeafIterator
  AlbertaGrid< dim, dimworld >::leafbegin () const
  {
    return leafbegin< codim, All_Partition >();
  }


  template< int dim, int dimworld >
  template< int codim >
  inline typename AlbertaGrid< dim, dimworld >::Traits
  ::template Codim< codim >::LeafIterator
  AlbertaGrid < dim, dimworld >::leafend () const
  {
    return leafend< codim, All_Partition >();
  }


  //****************************************
  // getNewEntity methods
  //***************************************

  // default implementation used new and delete
  template< class GridImp, class EntityProvider, int codim >
  struct GetNewEntity
  {
    typedef typename remove_const< GridImp >::type::Traits Traits;
    typedef MakeableInterfaceObject< typename Traits::template Codim< codim >::Entity >
    EntityObject;
    typedef typename EntityObject::ImplementationType EntityImp;

    static EntityObject *
    getNewEntity ( GridImp &grid, EntityProvider &enp )
    {
      return new EntityObject( EntityImp( grid ) );
    }

    static void freeEntity ( EntityProvider &enp, EntityObject *entity )
    {
      delete entity;
    }
  };

  // specialisation for codim 0 uses stack
  template< class GridImp, class EntityProvider >
  struct GetNewEntity< GridImp, EntityProvider, 0 >
  {
    typedef typename remove_const< GridImp >::type::Traits Traits;
    typedef MakeableInterfaceObject< typename Traits::template Codim< 0 >::Entity >
    EntityObject;
    typedef typename EntityObject::ImplementationType EntityImp;

    static EntityObject *
    getNewEntity ( GridImp &grid, EntityProvider &enp )
    {
      return enp.getNewObjectEntity( grid, (EntityImp *)0 );
    }

    static void freeEntity ( EntityProvider &enp , EntityObject *entity )
    {
      enp.freeObjectEntity( entity );
    }
  };


  template< int dim, int dimworld >
  template< int codim >
  inline MakeableInterfaceObject< typename AlbertaGrid< dim, dimworld >::Traits::template Codim< codim >::Entity > *
  AlbertaGrid< dim, dimworld >::getNewEntity () const
  {
    typedef GetNewEntity< const This, EntityProvider, codim > Helper;
    return Helper::getNewEntity( *this, entityProvider_ );
  }

  template< int dim, int dimworld >
  template< int codim >
  inline void AlbertaGrid< dim, dimworld >
  ::freeEntity ( MakeableInterfaceObject< typename Traits::template Codim< codim >::Entity > *entity ) const
  {
    typedef GetNewEntity< const This, EntityProvider, codim > Helper;
    Helper::freeEntity( entityProvider_, entity );
  }



  template< int dim, int dimworld >
  inline bool AlbertaGrid< dim, dimworld >::globalRefine ( int refCount )
  {
    typedef typename Traits::template Codim< 0 >::LeafIterator LeafIterator;

    // only MAXL level allowed
    assert( (refCount >= 0) && (refCount + maxlevel_ < MAXL) );

    bool wasChanged = false;
    for( int i = 0; i < refCount; ++i )
    {
      // mark all interior elements
      const LeafIterator endit = leafend< 0 >();
      for( LeafIterator it = leafbegin< 0 >(); it != endit; ++it )
        mark( 1, *it );

      preAdapt();
      wasChanged |= adapt();
      postAdapt();
    }
    return wasChanged;
  }


  template< int dim, int dimworld >
  template< class DataHandle >
  inline bool AlbertaGrid< dim, dimworld >
  ::globalRefine ( int refCount, AdaptDataHandleInterface< This, DataHandle > &handle )
  {
    typedef typename Traits::template Codim< 0 >::LeafIterator LeafIterator;

    // only MAXL level allowed
    assert( (refCount >= 0) && (refCount + maxlevel_ < MAXL) );

    bool wasChanged = false;
    for( int i = 0; i < refCount; ++i )
    {
      // mark all interior elements
      const LeafIterator endit = leafend< 0 >();
      for( LeafIterator it = leafbegin< 0 >(); it != endit; ++it )
        mark( 1, *it );

      wasChanged |= adapt( handle );
    }
    return wasChanged;
  }


  template< int dim, int dimworld >
  inline bool AlbertaGrid< dim, dimworld >::preAdapt ()
  {
    adaptationState_.preAdapt();
    return adaptationState_.coarsen();
  }


  template < int dim, int dimworld >
  inline void AlbertaGrid < dim, dimworld >::postAdapt()
  {
#ifndef NDEBUG
    for( int codim = 0; codim <= dimension; ++codim )
    {
      if( leafIndexSet_ != 0 )
        assert( leafIndexSet_->size( codim ) == mesh_.size( codim ) );
    }
#endif

#if 0
    typedef Alberta::Mesh Mesh;
    assert( (leafIndexSet_) ? (((Mesh *)mesh_)->n_elements == leafIndexSet_->size(0) ?   1 : 0) : 1);
    assert( (leafIndexSet_) ? (((Mesh *)mesh_)->n_vertices == leafIndexSet_->size(dim) ? 1 : 0) : 1);
  #if DIM == 3
    //assert( (leafIndexSet_ && dim == 3) ? (mesh->n_edges == leafIndexSet_->size(dim-1) ?  1 :0) :1);
    assert( (leafIndexSet_ && dim == 3) ? (((Mesh *)mesh_)->n_faces == leafIndexSet_->size(1) ? 1 : 0) : 1);
  #endif
#endif

    // clear refined marker
    Alberta::abs( elNewCheck_ );

    adaptationState_.postAdapt();
  }


  template< int dim, int dimworld >
  inline bool AlbertaGrid< dim, dimworld >
  ::mark( int refCount, const typename Traits::template Codim< 0 >::Entity &e )
  {
    // if not leaf entity, leave method
    if( !e.isLeaf() )
      return false;

    // take back previous marking
    adaptationState_.unmark( getMark( e ) );

    // set new marking
    adaptationState_.mark( refCount );
    getRealImplementation( e ).elementInfo().setMark( refCount );

    return true;
  }


  template< int dim, int dimworld >
  inline int AlbertaGrid< dim, dimworld >
  ::getMark( const typename Traits::template Codim< 0 >::Entity &e ) const
  {
    return getRealImplementation( e ).elementInfo().getMark();
  }


  template< int dim, int dimworld >
  inline bool AlbertaGrid< dim, dimworld >::adapt ()
  {
    // set all values of elNewCheck positive which means old
    Alberta::abs( elNewCheck_ );

    // adapt mesh
    hIndexSet_.preAdapt();
    const bool refined = mesh_.refine();
    const bool coarsened = (adaptationState_.coarsen() ? mesh_.coarsen() : false);
    adaptationState_.adapt();
    hIndexSet_.postAdapt();

    if( refined || coarsened )
      calcExtras();

    // return true if elements were created
    return refined;
  }


  template< int dim, int dimworld >
  template< class DataHandle >
  inline bool AlbertaGrid < dim, dimworld >
  ::adapt ( AdaptDataHandleInterface< This, DataHandle > &handle )
  {
    // minimum number of elements assumed to be created during adaptation
    const int defaultElementChunk = 100;

#ifndef CALC_COORD
    preAdapt();
    const int refineMarked = adaptationState_.refineMarked();
    handle.preAdapt( std::max( defaultElementChunk, 4*refineMarked ) );

    typedef Alberta::AdaptRestrictProlongHandler
    < This, AdaptDataHandleInterface< This, DataHandle > >
    DataHandler;
    DataHandler dataHandler( *this, handle );

    typedef AdaptationCallback< DataHandler > Callback;
    typename Callback::DofVectorPointer callbackVector;
    callbackVector.create( dofNumbering_.emptyDofSpace(), "Adaptation Callback" );
    callbackVector.template setupInterpolation< Callback >();
    callbackVector.template setupRestriction< Callback >();
    if( Callback::DofVectorPointer::supportsAdaptationData )
      callbackVector.setAdaptationData( dataHandler );
    else
      Alberta::adaptationDataHandler_ = &dataHandler;

    bool refined = adapt();

    if( !Callback::DofVectorPointer::supportsAdaptationData )
      Alberta::adaptationDataHandler_ = 0;
    callbackVector.release();

    handle.postAdapt();
    postAdapt();
    return refined;
#else
    derr << "AlbertaGrid::adapt(rpOp) : CALC_COORD defined, therefore adapt with call back not defined! \n";
    return false;
#endif
  }


  template< int dim, int dimworld >
  template< class DofManager, class RestrictProlongOperator >
  inline bool AlbertaGrid < dim, dimworld >
  ::adapt ( DofManager &dofManager, RestrictProlongOperator &rpOp, bool verbose )
  {
    typedef RestrictProlongWrapper< This, DofManager, RestrictProlongOperator > Wrapper;
    Wrapper rpOpWrapper( dofManager, rpOp );
    return adapt( rpOpWrapper );
  }


  template< int dim, int dimworld >
  inline bool AlbertaGrid< dim, dimworld >
  ::checkElNew ( const Alberta::Element *element ) const
  {
    assert( element != NULL );
    const int *array = (int *)elNewCheck_;
    const int index = dofNumbering_( element, 0, 0 );
    return (array[ index ] < 0);
  }


  template< int dim, int dimworld >
  inline const Alberta::GlobalVector &
  AlbertaGrid< dim, dimworld >
  ::getCoord ( const Alberta::ElementInfo< dimension > &elementInfo,
               int vertex ) const
  {
    assert( (vertex >= 0) && (vertex <= dim) );
#ifdef CALC_COORD
    return elementInfo.coordinate( vx );
#else
    return coordCache_( elementInfo, vertex );
#endif
  }


  template < int dim, int dimworld >
  inline int AlbertaGrid < dim, dimworld >::maxLevel() const
  {
    return maxlevel_;
  }


  template< int dim, int dimworld >
  inline int AlbertaGrid< dim, dimworld >::size ( int level, int codim ) const
  {
    assert( sizeCache_ != 0 );
    return ((level >= 0) && (level <= maxlevel_) ? sizeCache_->size( level, codim ) : 0);
  }


  template< int dim, int dimworld >
  inline int AlbertaGrid< dim, dimworld >::size ( int level, GeometryType type ) const
  {
    return (type.isSimplex() ? size( level, dim - type.dim() ) : 0);
  }


  template< int dim, int dimworld >
  inline int AlbertaGrid< dim, dimworld >::size ( int codim ) const
  {
    assert( sizeCache_ != 0 );
    assert( sizeCache_->size( codim ) == mesh_.size( codim ) );
    return mesh_.size( codim );
  }


  template< int dim, int dimworld >
  inline int AlbertaGrid< dim, dimworld >::size ( GeometryType type ) const
  {
    return (type.isSimplex() ? size( dim - type.dim() ) : 0);
  }


  template < int dim, int dimworld >
  inline const typename AlbertaGrid < dim, dimworld > :: Traits :: LevelIndexSet &
  AlbertaGrid < dim, dimworld > :: levelIndexSet (int level) const
  {
    // assert that given level is in range
    assert( level >= 0 );
    assert( level < (int) levelIndexVec_.size() );

    if(!levelIndexVec_[level]) levelIndexVec_[level] = new LevelIndexSetImp (*this,level);
    return *(levelIndexVec_[level]);
  }

  template < int dim, int dimworld >
  inline const typename AlbertaGrid < dim, dimworld > :: Traits :: LeafIndexSet &
  AlbertaGrid < dim, dimworld > :: leafIndexSet () const
  {
    if(!leafIndexSet_) leafIndexSet_ = new LeafIndexSetImp (*this);
    return *leafIndexSet_;
  }


  template< int dim, int dimworld >
  inline int AlbertaGrid< dim, dimworld >
  ::getLevelOfElement ( const Alberta::Element *element ) const
  {
    assert( element != NULL );

    const int *array = (int *)elNewCheck_;
    const int index = dofNumbering_( element, 0, 0 );
    // return the elements level which is the absolute value of the entry
    return std::abs( array[ index ] );
  }


  template < int dim, int dimworld >
  inline void AlbertaGrid < dim, dimworld >::calcExtras ()
  {
    // determine new maxlevel
    maxlevel_ = Alberta::maxAbs( elNewCheck_ );
    assert( (maxlevel_ >= 0) && (maxlevel_ < MAXL) );

#ifndef NDEBUG
    typedef Alberta::FillFlags< dim > FillFlags;
    CalcMaxLevel calcMaxLevel;
    mesh_.leafTraverse( calcMaxLevel, FillFlags::nothing );
    assert( maxlevel_ == calcMaxLevel.maxLevel() );
#endif

    // unset up2Dat status, if lbegin is called then this status is updated
    for( int l = 0; l < MAXL; ++l )
      levelMarkerVector_[ l ].clear();

    // unset up2Dat status, if leafbegin is called then this status is updated
    leafMarkerVector_.clear();

    if(sizeCache_) delete sizeCache_;
    // first bool says we have simplex, second not cube, third, worryabout
    sizeCache_ = new SizeCacheType (*this,true,false,true);

    // if levelIndexSet exists, then update now
    for(unsigned int i=0; i<levelIndexVec_.size(); ++i)
      if(levelIndexVec_[i]) (*levelIndexVec_[i]).calcNewIndex();

    // create new Leaf Index
    if( leafIndexSet_ ) leafIndexSet_->calcNewIndex();
  }


  template< int dim, int dimworld >
  template< GrapeIOFileFormatType format >
  inline bool AlbertaGrid< dim, dimworld >
  ::writeGrid ( const std::string &filename, ctype time ) const
  {
    switch( format )
    {
    case xdr :
      return writeGridXdr( filename, time );

    case ascii :
      DUNE_THROW( NotImplemented, "AlbertaGrid does not support writeGrid< ascii >." );

    // write leaf grid as macro triangulation
    //int ret = ALBERTA write_macro( mesh_ , filename.c_str() );
    //return (ret == 1) ? true : false;

    default :
      DUNE_THROW( NotImplemented, "AlbertaGrid: Unknown output format: " << format << "." );
    }
    return false;
  }


  template< int dim, int dimworld >
  template< GrapeIOFileFormatType format >
  inline bool AlbertaGrid< dim, dimworld >
  ::readGrid ( const std::string &filename, ctype &time )
  {
    switch( format )
    {
    case xdr :
      return readGridXdr( filename, time );

    case ascii :
      DUNE_THROW( NotImplemented, "AlbertaGrid does not support readGrid< ascii >." );

    //return readGridAscii (filename , time );

    default :
      DUNE_THROW( NotImplemented, "AlbertaGrid: Unknown output format: " << format << "." );
    }
    return false;
  }


  template< int dim, int dimworld >
  inline bool AlbertaGrid< dim, dimworld >
  ::writeGridXdr ( const std::string &filename, ctype time ) const
  {
    if( filename.size() <= 0 )
      DUNE_THROW( AlbertaIOError, "No filename given to writeGridXdr." );
    return (mesh_.write( filename, time ) && hIndexSet_.write( filename ));
  }


  template< int dim, int dimworld >
  inline bool AlbertaGrid< dim, dimworld >
  ::readGridXdr ( const std::string &filename, ctype &time )
  {
    typedef Alberta::FillFlags< dim > FillFlags;

    //removeMesh();

    if( filename.size() <= 0 )
      return false;

    mesh_.read( filename, time );
    if( !mesh_ )
      DUNE_THROW( AlbertaIOError, "Could not read grid file: " << filename << "." );

    setup();

    SetLocalElementLevel setLocalElementLevel( elNewCheck_ );
    mesh_.hierarchicTraverse( setLocalElementLevel, FillFlags::nothing );

    hIndexSet_.read( filename, mesh_ );

    // calc maxlevel and indexOnLevel and so on
    calcExtras();

    return true;
  }


#if 0
  template < int dim, int dimworld >
  inline bool AlbertaGrid< dim, dimworld >
  ::readGridAscii ( const std::string &filename, ctype &time )
  {
    removeMesh(); // delete all objects

    mesh_.create( "AlbertaGrid", filename.c_str() );

    time = 0.0;

    // unset up2Dat status, if lbegin is called then this status is updated
    for( int l = 0; l < MAXL; l++ )
      levelMarkerVector_[ l ].unsetUp2Date();

    // unset up2Dat status, if leafbegin is called then this status is updated
    leafMarkerVector_.unsetUp2Date();

    setup();
    hIndexSet_.create( dofNumbering_ );
    elNewCheck_.initialize( 0 );
    LeafDataType::initLeafDataValues( mesh_, 0 );

    calcExtras();

    return true;
  }
#endif



  // AlbertaGrid::AdaptationCallback
  // -------------------------------

  template< int dim, int dimworld >
  template< class DataHandler >
  struct AlbertaGrid< dim, dimworld >::AdaptationCallback
  {
    typedef Alberta::DofVectorPointer< Alberta::GlobalVector > DofVectorPointer;

    static DataHandler &getDataHandler ( const DofVectorPointer &dofVector )
    {
      DataHandler *dataHandler;
      if( DofVectorPointer::supportsAdaptationData )
        dataHandler = dofVector.getAdaptationData< DataHandler >();
      else
        dataHandler = (DataHandler *)Alberta::adaptationDataHandler_;
      assert( dataHandler != 0 );
      return *dataHandler;
    }

    static void interpolateVector ( const DofVectorPointer &dofVector,
                                    const Alberta::Patch &patch )
    {
      DataHandler &dataHandler = getDataHandler();
      assert( dataHandler != 0 );
      for( int i = 0; i < patch.count(); ++i )
        dataHandler->prolongLocal( patch[ i ] );
    }

    static void restrictVector ( const DofVectorPointer &dofVector,
                                 const Alberta::Patch &patch )
    {
      DataHandler *dataHandler = (DataHandler *)Alberta::adaptationDataHandler_;
      assert( dataHandler != 0 );
      for( int i = 0; i < patch.count(); ++i )
        dataHandler->restrictLocal( patch[ i ] );
    }
  };



  // AlbertaGrid::CalcMaxLevel
  // -------------------------

  template< int dim, int dimworld >
  class AlbertaGrid< dim, dimworld >::CalcMaxLevel
  {
    int maxLevel_;

  public:
    CalcMaxLevel ()
      : maxLevel_( 0 )
    {}

    void operator() ( const Alberta::ElementInfo< dim > &elementInfo )
    {
      maxLevel_ = std::max( maxLevel_, elementInfo.level() );
    }

    int maxLevel () const
    {
      return maxLevel_;
    }
  };



  // AlbertaGrid::SetLocalElementLevel
  // ---------------------------------

  template< int dim, int dimworld >
  class AlbertaGrid< dim, dimworld >::SetLocalElementLevel
  {
    typedef Alberta::DofAccess< dim, 0 > DofAccess;

    Alberta::DofVectorPointer< int > levels_;
    DofAccess dofAccess_;

  public:
    explicit SetLocalElementLevel ( const Alberta::DofVectorPointer< int > &levels )
      : levels_( levels ),
        dofAccess_( levels.dofSpace() )
    {}

    void operator() ( const Alberta::ElementInfo< dim > &elementInfo ) const
    {
      int *const array = (int *)levels_;
      const int dof = dofAccess_( elementInfo.el(), 0 );
      array[ dof ] = elementInfo.level();
    }
  };



  // AlbertaGrid::ElNewCheckInterpolation
  // ------------------------------------

  template< int dim, int dimworld >
  struct AlbertaGrid< dim, dimworld >::ElNewCheckInterpolation
  {
    static const int dimension = dim;
    static const int codimension = 0;

  private:
    typedef Alberta::DofVectorPointer< int > DofVectorPointer;
    typedef Alberta::DofAccess< dimension, codimension > DofAccess;

    DofVectorPointer dofVector_;
    DofAccess dofAccess_;

    explicit ElNewCheckInterpolation ( const DofVectorPointer &dofVector )
      : dofVector_( dofVector ),
        dofAccess_( dofVector.dofSpace() )
    {}

  public:
    void operator() ( const Alberta::Element *father )
    {
      int *array = (int *)dofVector_;
      const int fatherLevel = std::abs( array[ dofAccess_( father, 0 ) ] );
      for( int i = 0; i < 2; ++i )
      {
        const Alberta::Element *child = father->child[ i ];
        array[ dofAccess_( child, 0 ) ] = -(fatherLevel+1);
      }
    }

    static void interpolateVector ( const DofVectorPointer &dofVector,
                                    const Alberta::Patch &patch )
    {
      ElNewCheckInterpolation interpolation( dofVector );
      patch.forEach( interpolation );
    }
  };

} // namespace Dune

#endif

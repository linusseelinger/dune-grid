// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:
#ifndef DUNE_ALU3DGRIDGRID_HH
#define DUNE_ALU3DGRIDGRID_HH

//- System includes
#include <vector>

//- Dune includes
#include <dune/grid/utility/grapedataioformattypes.hh>
#include <dune/grid/common/capabilities.hh>
#include <dune/common/interfaces.hh>
#include <dune/common/bigunsignedint.hh>

#include <dune/grid/common/grid.hh>
#include <dune/grid/common/referenceelements.hh>
#include <dune/grid/common/defaultindexsets.hh>
#include <dune/grid/common/sizecache.hh>
#include <dune/grid/common/intersectioniteratorwrapper.hh>
#include <dune/grid/common/datahandleif.hh>

//- Local includes
#include "alu3dinclude.hh"
#include "topology.hh"
#include "indexsets.hh"
#include "memory.hh"
#include "datahandle.hh"
#include "alu3dutility.hh"

#if ALU3DGRID_PARALLEL
#include <dune/common/mpicollectivecommunication.hh>
#else
#include <dune/common/collectivecommunication.hh>
#endif

namespace Dune {

  // Forward declarations
  template<int cd, int dim, class GridImp>
  class ALU3dGridEntity;
  template<int cd, PartitionIteratorType pitype, class GridImp >
  class ALU3dGridLevelIterator;
  template<int cd, class GridImp >
  class ALU3dGridEntityPointerBase;
  template<int cd, class GridImp >
  class ALU3dGridEntityPointer;
  template<int mydim, int coorddim, class GridImp>
  class ALU3dGridGeometry;
  template<class GridImp>
  class ALU3dGridHierarchicIterator;
  template<class GridImp>
  class ALU3dGridIntersectionIterator;
  template<class GridImp>
  class ALU3dGridLevelIntersectionIterator;
  template<int codim, PartitionIteratorType pitype, class GridImp>
  class ALU3dGridLeafIterator;
  template <int mydim, int coorddim, class GridImp>
  class ALU3dGridMakeableEntity;
  template <class GridImp>
  class ALU3dGridFaceGeometryInfo;
  template<int dim, int dimworld, ALU3dGridElementType elType>
  class ALU3dGridGlobalIdSet;
  template<int dim, int dimworld, ALU3dGridElementType elType>
  class ALU3dGridLocalIdSet;
  template<int dim, int dimworld, ALU3dGridElementType elType>
  class ALU3dGridHierarchicIndexSet;
  template <class EntityImp>
  class ALUMemoryProvider;
  template<int dim, int dimworld, ALU3dGridElementType elType>
  class ALU3dGrid;
  template <class GridImp, int codim>
  struct ALU3dGridEntityFactory;

  //**********************************************************************
  //
  // --ALU3dGrid
  // --Grid
  //
  //**********************************************************************
  template <int dim, int dimworld, ALU3dGridElementType elType>
  struct ALU3dGridFamily
  {
    //! Type of the local id set
    typedef ALU3dGridLocalIdSet<dim,dimworld,elType> LocalIdSetImp;

#if ALU3DGRID_PARALLEL
    //! Type of the global id set
    typedef ALU3dGridGlobalIdSet<dim,dimworld,elType> GlobalIdSetImp;
#else
    typedef ALU3dGridGlobalIdSet<dim,dimworld,elType> GlobalIdSetImp;
    //typedef LocalIdSetImp GlobalIdSetImp;
#endif

    //! Type of the level index set
    typedef DefaultLevelIndexSet<ALU3dGrid < dim, dimworld, elType > >  LevelIndexSetImp;
    //! Type of the leaf index set
    typedef DefaultLeafIndexSet<ALU3dGrid < dim, dimworld, elType > >   LeafIndexSetImp;

    //! type of ALU3dGrids global id
    typedef ALUGridId<ALUMacroKey> GlobalIdType;

    //! type of ALU3dGrids local id
    typedef int LocalIdType;

    typedef ALU3dGrid<dim,dimworld,elType> GridImp;

    struct Traits
    {
      //! type of ALU3dGrids local id
      typedef int LocalIdType;

      //! type of ALU3dGrids global id
      typedef ALUGridId<ALUMacroKey> GlobalIdType;

      typedef ALU3dGrid<dim,dimworld,elType> Grid;

      typedef Dune::IntersectionIterator<const GridImp, LeafIntersectionIteratorWrapper> IntersectionIterator;

      typedef Dune::IntersectionIterator<const GridImp, LeafIntersectionIteratorWrapper > LeafIntersectionIterator;
      typedef Dune::IntersectionIterator<const GridImp, LevelIntersectionIteratorWrapper> LevelIntersectionIterator;

      typedef Dune::HierarchicIterator<const GridImp, ALU3dGridHierarchicIterator> HierarchicIterator;

      template <int cd>
      struct Codim
      {
        // IMPORTANT: Codim<codim>::Geometry == Geometry<dim-codim,dimw>
        typedef Dune::Geometry<dim-cd, dimworld, const GridImp, ALU3dGridGeometry> Geometry;
        typedef Dune::Geometry<dim-cd, dim, const GridImp, ALU3dGridGeometry> LocalGeometry;
        // we could - if needed - introduce an other struct for dimglobal of Geometry
        typedef Dune::Entity<cd, dim, const GridImp, ALU3dGridEntity> Entity;

        typedef Dune::LevelIterator<cd,All_Partition,const GridImp,ALU3dGridLevelIterator> LevelIterator;

        typedef Dune::LeafIterator<cd,All_Partition,const GridImp,ALU3dGridLeafIterator> LeafIterator;

        typedef Dune::EntityPointer<const GridImp,ALU3dGridEntityPointer<cd,const GridImp> > EntityPointer;

        template <PartitionIteratorType pitype>
        struct Partition
        {
          typedef Dune::LevelIterator<cd,pitype,const GridImp,ALU3dGridLevelIterator> LevelIterator;
          typedef Dune::LeafIterator<cd,pitype,const GridImp,ALU3dGridLeafIterator> LeafIterator;
        };

      };

      typedef IndexSet<GridImp,LevelIndexSetImp,DefaultLevelIteratorTypes<GridImp> > LevelIndexSet;
      typedef IndexSet<GridImp,LeafIndexSetImp,DefaultLeafIteratorTypes<GridImp> > LeafIndexSet;
      typedef IdSet<GridImp,LocalIdSetImp,LocalIdType> LocalIdSet;

#if ALU3DGRID_PARALLEL
      typedef IdSet<GridImp,GlobalIdSetImp,GlobalIdType> GlobalIdSet;
      typedef CollectiveCommunication<MPI_Comm> CollectiveCommunication;
#else
      // in serial we use LocalIdSet as GlobalIdSet because it much faster
      // that global id set
      typedef LocalIdSet GlobalIdSet;
      typedef CollectiveCommunication<Grid> CollectiveCommunication;
#endif
    };
  };

  /**
     \brief [<em> provides \ref Dune::Grid </em>]
     \brief 3D grid with support for hexahedrons and tetrahedrons.
     The ALU3dGrid implements the Dune GridInterface for 3d tetrahedral and
     hexahedral meshes. This grid can be locally adapted and used in parallel
     computations using dynamcic load balancing.

     @note
     Adaptive parallel grid supporting dynamic load balancing, written
     mainly by Bernard Schupp. This grid supports hexahedrons and tetrahedrons.

     (see ALUGrid homepage: http://www.mathematik.uni-freiburg.de/IAM/Research/alugrid/)

     Two tools are available for partitioning :
     \li Metis ( version 4.0 and higher, see http://www-users.cs.umn.edu/~karypis/metis/metis/ )
     \li Party Lib ( version 1.1 and higher, see http://wwwcs.upb.de/fachbereich/AG/monien/RESEARCH/PART/party.html)

     @author Robert Kloefkorn
   */
  template <int dim, int dimworld, ALU3dGridElementType elType>
  class ALU3dGrid :
    public GridDefaultImplementation<dim, dimworld, alu3d_ctype, ALU3dGridFamily <dim,dimworld,elType> >,
    public HasObjectStream
  {
    // type of base class
    typedef GridDefaultImplementation<dim, dimworld, alu3d_ctype, ALU3dGridFamily <dim,dimworld,elType> > BaseType;

    CompileTimeChecker<(dim      == 3)> ALU3dGrid_only_implemented_for_3dp;
    CompileTimeChecker<(dimworld == 3)> ALU3dGrid_only_implemented_for_3dw;

    typedef ALU3dGrid<dim,dimworld,elType> MyType;
    typedef ALU3dGrid<dim,dimworld,elType> ThisType;

    friend class ALU3dGridEntity <0,dim,MyType>;
    friend class ALU3dGridEntity <0,dim,const MyType>;
    friend class ALU3dGridEntity <1,dim,const MyType>;
    friend class ALU3dGridEntity <2,dim,const MyType>;
    friend class ALU3dGridEntity <3,dim,const MyType>;
    friend class ALU3dGridIntersectionIterator<MyType>;

    friend class ALU3dGridEntityPointerBase<0,const MyType >;
    friend class ALU3dGridEntityPointerBase<1,const MyType >;
    friend class ALU3dGridEntityPointer<1,const MyType >;
    friend class ALU3dGridEntityPointerBase<2,const MyType >;
    friend class ALU3dGridEntityPointer<2,const MyType >;
    friend class ALU3dGridEntityPointerBase<3,const MyType >;
    friend class ALU3dGridEntityPointer<3,const MyType >;

    friend class ALU3dGridIntersectionIterator<const MyType>;
    friend class ALU3dGridHierarchicIterator<const MyType>;

    friend class ALU3dGridHierarchicIndexSet<dim,dimworld,elType>;
    friend class ALU3dGridGlobalIdSet<dim,dimworld,elType>;
    friend class ALU3dGridLocalIdSet<dim,dimworld,elType>;

    //**********************************************************
    // The Interface Methods
    //**********************************************************
  public:
    static const ALU3dGridElementType elementType = elType;
    typedef typename ALU3DSPACE GatherScatterType::ObjectStreamType ObjectStreamType;

    typedef ALU3dGridFamily<dim,dimworld,elType> GridFamily;

    friend class Conversion< ALU3dGrid<dim,dimworld,elementType> , HasObjectStream > ;
    friend class Conversion< const ALU3dGrid<dim,dimworld,elementType> , HasObjectStream > ;

    //! my Traits class
    typedef typename ALU3dGridFamily < dim , dimworld , elType > :: Traits Traits;

  protected:
    typedef MakeableInterfaceObject<typename Traits::template Codim<0>::Geometry> GeometryObject;
    friend class LocalGeometryStorage< GeometryObject , 8 >;

  public:

    //! Type of the hierarchic index set
    typedef ALU3dGridHierarchicIndexSet<dim,dimworld,elType> HierarchicIndexSet;

    //! Type of the local id set
    typedef typename ALU3dGridFamily < dim , dimworld , elType > :: LocalIdSetImp LocalIdSetImp;

    //! Type of the global id set
    typedef typename ALU3dGridFamily < dim , dimworld , elType > :: GlobalIdSetImp GlobalIdSetImp;

    //! Type of the global id set
    typedef typename Traits :: GlobalIdSet GlobalIdSet;

    //! Type of the local id set
    typedef typename Traits :: LocalIdSet LocalIdSet;

    //! Type of the level index set
    typedef typename GridFamily :: LevelIndexSetImp LevelIndexSetImp;
    //! Type of the leaf index set
    typedef typename GridFamily :: LeafIndexSetImp LeafIndexSetImp;

    typedef typename SelectType<elType == tetra,
        ReferenceSimplex<alu3d_ctype, dim>,
        ReferenceCube   <alu3d_ctype, dim> >::Type ReferenceElementType;

    //! a standard leaf iterator
    typedef ALU3dGridLeafIterator<0, All_Partition, MyType> LeafIteratorImp;
    typedef typename Traits::template Codim<0>::LeafIterator LeafIteratorType;
    typedef typename Traits::template Codim<0>::LeafIterator LeafIterator;

    typedef ALU3dGridHierarchicIterator<MyType> HierarchicIteratorImp;

    typedef typename Traits :: CollectiveCommunication
    CollectiveCommunicationType;

    //! max number of levels
    enum {
      //! \brief maximal number of levels is 64
      MAXL = 64
    };

    //! element chunk for refinement
    enum {
      //! \brief normal default number of new elements for new adapt method
      newElementsChunk_ = 100
    };

    //! upper estimate on number of elements that could be created when a new element is created
    enum {
      /** \brief if one element is refined then it
          causes apporximately not more than
          this number of new elements  */
      refineEstimate_ = 40
    };

  protected:
    //! Constructor which reads an ALU3dGrid Macro Triang file
    //! or given GridFile
#if ALU3DGRID_PARALLEL
    ALU3dGrid(const std::string macroTriangFilename , MPI_Comm mpiComm = MPI_COMM_WORLD );
#else
    ALU3dGrid(const std::string macroTriangFilename , int myrank = -1);
#endif
  public:

    //! \brief Desctructor
    ~ALU3dGrid();

    //! \brief for grid identification
    std::string name () const;

    /** \brief  Return maximum level defined in this grid. Levels are numbered
        maxLevel with 0 the coarsest level.
     */
    int maxLevel() const;

    //! Iterator to first entity of given codim on level
    template<int cd, PartitionIteratorType pitype>
    typename Traits::template Codim<cd>::template Partition<pitype>::LevelIterator
    lbegin (int level) const;

    //! one past the end on this level
    template<int cd, PartitionIteratorType pitype>
    typename Traits::template Codim<cd>::template Partition<pitype>::LevelIterator
    lend (int level) const;

    //! Iterator to first entity of given codim on level
    template<int cd>
    typename Traits::template Codim<cd>::
    template Partition<All_Partition>::LevelIterator
    lbegin (int level) const;

    //! one past the end on this level
    template<int cd>
    typename Traits::template Codim<cd>::
    template Partition<All_Partition>::LevelIterator
    lend (int level) const;

  private:
    //! General definiton for a leaf iterator
    template <int codim, PartitionIteratorType pitype>
    typename Traits::template Codim<codim>::template Partition<pitype>::LeafIterator
    leafbegin(int level) const;

    //! General definition for an end iterator on leaf level
    template <int codim, PartitionIteratorType pitype>
    typename Traits::template Codim<codim>::template Partition<pitype>::LeafIterator
    leafend(int level) const;

    //! General definiton for a leaf iterator
    template <int codim>
    typename Traits::template Codim<codim>::LeafIterator
    leafbegin(int level) const;

    //! General definition for an end iterator on leaf level
    template <int codim>
    typename Traits::template Codim<codim>::LeafIterator
    leafend(int level) const;

    //! Iterator to first entity of codim 0 on leaf level (All_Partition)
    LeafIteratorType leafbegin (int level) const;

    //! one past the end on this leaf level (codim 0 and All_Partition)
    LeafIteratorType leafend (int level) const;

    //! Iterator to first entity of codim 0 on leaf level (All_Partition)
    LeafIteratorType leafbegin () const;

    //! one past the end on this leaf level (codim 0 and All_Partition)
    LeafIteratorType leafend () const;

  public:
    //! General definiton for a leaf iterator
    template <int codim, PartitionIteratorType pitype>
    typename Traits::template Codim<codim>::template Partition<pitype>::LeafIterator
    leafbegin() const;

    //! General definition for an end iterator on leaf level
    template <int codim, PartitionIteratorType pitype>
    typename Traits::template Codim<codim>::template Partition<pitype>::LeafIterator
    leafend() const;

    //! General definiton for a leaf iterator
    template <int codim>
    typename Traits::template Codim<codim>::LeafIterator
    leafbegin() const;

    //! General definition for an end iterator on leaf level
    template <int codim>
    typename Traits::template Codim<codim>::LeafIterator
    leafend() const;

  private:
    //! General definiton for a leaf iterator
    template <int codim, PartitionIteratorType pitype>
    typename Traits::template Codim<codim>::template Partition<pitype>::LeafIterator
    createLeafIteratorBegin (int level) const;

    //! General definition for an end iterator on leaf level
    template <int codim, PartitionIteratorType pitype>
    typename Traits::template Codim<codim>::template Partition<pitype>::LeafIterator
    createLeafIteratorEnd(int level) const;

  public:
    //! number of grid entities per level and codim
    int size (int level, int cd) const;

    //! number of leaf entities per codim in this process
    int size (int codim) const;

    //! number of entities per level and geometry type in this process
    int size (int level, GeometryType type) const;

    //! number of leaf entities per geometry type in this process
    int size (GeometryType type) const;

    //! number of grid entities on all levels for given codim
    int global_size (int cd) const ;

    // (no interface method) number of grid entities in the entire grid for given codim
    int hierSetSize (int cd) const;

    //! get global id set of grid
    const GlobalIdSet & globalIdSet () const {
#if ALU3DGRID_PARALLEL
      if(!globalIdSet_) globalIdSet_ = new GlobalIdSetImp(*this);
      return *globalIdSet_;
#else
      // use local id set in serial
      return localIdSet();
#endif
    }

    //! get global id set of grid
    const LocalIdSet & localIdSet () const { return localIdSet_; }

    //! get leaf index set of the grid
    const typename Traits :: LeafIndexSet & leafIndexSet () const;

    //! get level index set of the grid
    const typename Traits :: LevelIndexSet & levelIndexSet (int level) const;

    //! calculate load of each proc and repartition if neccessary
    bool loadBalance ();

    //! calculate load of each proc and repartition if neccessary
    template <class DataHandleType>
    bool loadBalance (DataHandleType & data);

    /** \brief ghostSize is zero for this grid  */
    int ghostSize (int level, int codim) const;

    /** \brief overlapSize is zero for this grid  */
    int overlapSize (int level, int codim) const { return 0; }

    /** \brief ghostSize is zero for this grid  */
    int ghostSize (int codim) const;

    /** \brief overlapSize is zero for this grid  */
    int overlapSize (int codim) const { return 0; }

    /** level communicate */
    template<class DataHandleImp,class DataType>
    void communicate (CommDataHandleIF<DataHandleImp,DataType> & data, InterfaceType iftype, CommunicationDirection dir, int level) const;

    /** leaf communicate  */
    template<class DataHandleImp,class DataType>
    void communicate (CommDataHandleIF<DataHandleImp,DataType> & data, InterfaceType iftype, CommunicationDirection dir) const;

  private:
    typedef ALU3DSPACE GatherScatter GatherScatterType;
    /** do communication  */
    void doCommunication (
      GatherScatterType & vertexData,
      GatherScatterType & edgeData,
      GatherScatterType & faceData,
      GatherScatterType & elementData,
      InterfaceType iftype, CommunicationDirection dir) const;

  public:
    /** collective communicate object */
    const CollectiveCommunicationType & comm () const { return ccobj_; }

    //! returns if a least one entity was marked for coarsening
    bool preAdapt ( );

    //! clear all entity new markers
    void postAdapt ( );

    /**! refine all positive marked leaf entities,
       return true if a least one entity was refined */
    bool adapt ( );

    //! adapt with DofManager
    template <class DofManagerType, class RestrictProlongOperatorType>
    bool adapt (DofManagerType &, RestrictProlongOperatorType &, bool verbose=false );

    //! uses the interface, mark on entity and refineLocal
    bool globalRefine(int refCount);

    //**********************************************************
    // End of Interface Methods
    //**********************************************************
    /** \brief write Grid to file in specified FileFormatType
     */
    template <GrapeIOFileFormatType ftype>
    bool writeGrid( const std::string filename, alu3d_ctype time ) const ;

    bool writeGrid_Xdr( const std::string filename, alu3d_ctype time ) const ;
    bool writeGrid_Ascii( const std::string filename, alu3d_ctype time ) const ;

    /** \brief read Grid from file filename and store time of mesh in time
     */
    template <GrapeIOFileFormatType ftype>
    bool readGrid( const std::string filename, alu3d_ctype & time );

    // (no interface method) get hierarchic index set of the grid
    const HierarchicIndexSet & hierarchicIndexSet () const { return hIndexSet_; }

    // set max of given mxl and actual maxLevel
    // for loadBalance
    void setMaxLevel (int mxl);

    // no interface method, but has to be public
    void updateStatus ();

    //! mark entities for refinement or coarsening, refCount < 0 will mark
    //! the entity for one coarsen step and refCount > 0 will mark for one
    //! refinement, one refinement will create 8 children per element
    bool mark( int refCount , const typename Traits::template Codim<0>::EntityPointer & ep );
  private:
    bool mark( int refCount , const typename Traits::template Codim<0>::Entity & en );

  public:
    template <class IntersectionInterfaceType>
    const typename BaseType::
    template ReturnImplementationType<IntersectionInterfaceType> :: ImplementationType &
    getRealIntersectionIterator(const IntersectionInterfaceType & it) const
    {
      return this->getRealImplementation(it);
    }

    //! deliver all geometry types used in this grid
    const std::vector<GeometryType>& geomTypes (int codim) const { return geomTypes_[codim]; }

    // return reference to org ALU3dGrid
    // private method, but otherwise we have to friend class all possible
    // types of LevelIterator ==> later
    ALU3DSPACE GitterImplType & myGrid() const;

    //! return reference to Dune reference element according to elType
    const ReferenceElementType & referenceElement() const { return referenceElement_; }

  protected:
    //! Copy constructor should not be used
    ALU3dGrid( const MyType & g );

    //! assignment operator should not be used
    ALU3dGrid<dim,dimworld,elType>&
    operator = (const MyType & g);

    //! reset size and global size, update Level- and LeafIndexSet, if they exist
    void calcExtras();

    //! calculate maxlevel
    void calcMaxlevel();

    //! make grid walkthrough and calc global size
    void recalcGlobalSize();

    //! check whether macro grid format is of our type
    void checkMacroGridFile (const std::string filename);

    //! check whether macro grid has the right element type
    void checkMacroGrid ();

    // the real ALU grid
    mutable ALU3DSPACE GitterImplType * mygrid_;
#if ALU3DGRID_PARALLEL
    ALU3DSPACE MpAccessMPI mpAccess_;
#endif

    // collective comm, same as mpAccess_, only Peters "generic" (haha)version
    CollectiveCommunicationType ccobj_;

  public:
    // number of links to other processors, for internal use only
    int nlinks () const {
#if ALU3DGRID_PARALLEL
      return mpAccess_.nlinks();
#endif
      return 1;
    }

  private:
    // max level of grid
    int maxlevel_;

    // count how much elements where marked
    mutable int coarsenMarked_;
    mutable int refineMarked_;

    // at the moment the number of different geom types is 1
    enum { numberOfGeomTypes = 1 };
    std::vector< std::vector<GeometryType> > geomTypes_;

    // create GeomTypes
    void makeGeomTypes ();

    // our hierarchic index set
    HierarchicIndexSet hIndexSet_;

    // out global id set
    mutable GlobalIdSetImp * globalIdSet_;

    // out global id set
    LocalIdSetImp localIdSet_;

    // the level index set ( default type )
    mutable std::vector < LevelIndexSetImp * > levelIndexVec_;

    // the leaf index set
    mutable LeafIndexSetImp * leafIndexSet_;

    // the entity codim 0
  public:
    typedef MakeableInterfaceObject<typename Traits::template Codim<0>::Entity> EntityObject;
  private:
    typedef ALUMemoryProvider< EntityObject > EntityProvider;

    template <int codim>
    inline MakeableInterfaceObject<typename Traits::template Codim<codim>::Entity> *
    getNewEntity ( int level ) const
    {
      return ALU3dGridEntityFactory<MyType,codim>::getNewEntity(*this,entityProvider_,level);
    }

    template <int codim>
    inline void freeEntity (MakeableInterfaceObject<typename Traits::template Codim<codim>::Entity> * en) const
    {
      ALU3dGridEntityFactory<MyType,codim>::freeEntity(entityProvider_, en);
    }

    mutable EntityProvider entityProvider_;

    // the reference element
    ReferenceElementType referenceElement_;

    typedef ALU3dGridVertexList VertexListType;
    mutable VertexListType vertexList_[MAXL];

    mutable ALU3dGridItemListType ghostLeafList_[dim];
    mutable ALU3dGridItemListType ghostLevelList_[dim][MAXL];

    mutable ALU3dGridItemListType levelEdgeList_[MAXL];

    typedef ALU3dGridLeafVertexList LeafVertexListType;
    mutable LeafVertexListType leafVertexList_;
  public:
    LeafVertexListType & getLeafVertexList() const
    {
      if( !leafVertexList_.up2Date() ) leafVertexList_.setupVxList(*this);
      return leafVertexList_;
    }

    int getLevelOfLeafVertex(const ALU3DSPACE VertexType & vertex) const
    {
      assert( leafVertexList_.up2Date() );
      return leafVertexList_.getLevel(vertex);
    }

    VertexListType & getVertexList(int level) const
    {
      assert( level >= 0 );
      assert( level <= maxLevel() );
      VertexListType & vxList = vertexList_[level];
      if(!vxList.up2Date()) vxList.setupVxList(*this,level);
      return vxList;
    }

    ALU3dGridItemListType & getGhostLeafList(int codim) const
    {
      assert( codim >= 1 );
      assert( codim <= 3 );
      return ghostLeafList_[codim-1];
    }

    ALU3dGridItemListType & getGhostLevelList(int codim, int level) const
    {
      assert( codim >= 1 );
      assert( codim <= 3 );

      assert( level >= 0 );
      assert( level <= maxLevel() );
      return ghostLevelList_[codim-1][level];
    }

    ALU3dGridItemListType & getEdgeList(int level) const
    {
      assert( level >= 0 );
      assert( level <= maxLevel() );
      return levelEdgeList_[level];
    }
  private:

    // the type of our size cache
    typedef SingleTypeSizeCache<MyType> SizeCacheType;
    SizeCacheType * sizeCache_;

    // actual number of ghost elements
    int ghostElements_;

    // new intersection iterator is a wrapper which get itersectioniteratoimp as pointers
  public:
    typedef ALU3dGridIntersectionIterator<const ThisType>
    IntersectionIteratorImp;
    typedef ALU3dGridIntersectionIterator<const ThisType>
    LeafIntersectionIteratorImp;
    typedef ALU3dGridLevelIntersectionIterator<const ThisType>
    LevelIntersectionIteratorImp;

    typedef ALUMemoryProvider< LeafIntersectionIteratorImp > LeafIntersectionIteratorProviderType;
    typedef ALUMemoryProvider< LevelIntersectionIteratorImp >   LevelIntersectionIteratorProviderType;
  private:
    friend class LeafIntersectionIteratorWrapper < const ThisType > ;
    friend class LevelIntersectionIteratorWrapper< const ThisType > ;
    // return reference to intersectioniterator storage
    LeafIntersectionIteratorProviderType & leafIntersetionIteratorProvider() const { return leafInterItProvider_; }
    mutable LeafIntersectionIteratorProviderType leafInterItProvider_;
    LevelIntersectionIteratorProviderType & levelIntersetionIteratorProvider() const { return levelInterItProvider_; }
    mutable LevelIntersectionIteratorProviderType levelInterItProvider_;

  }; // end class ALU3dGrid


  template <class GridImp, int codim>
  struct ALU3dGridEntityFactory
  {
    typedef typename GridImp :: template Codim<codim> :: Entity Entity;
    typedef MakeableInterfaceObject<Entity> EntityObject;
    typedef typename EntityObject :: ImplementationType EntityImp;

    template <class EntityProviderType>
    static EntityObject *
    getNewEntity (const GridImp & grid, EntityProviderType & ep, int level)
    {
      return new EntityObject(EntityImp( grid, level ));
    }

    template <class EntityProviderType>
    static void freeEntity( EntityProviderType & ep, EntityObject * e )
    {
      delete e;
    }
  };

  template <class GridImp>
  struct ALU3dGridEntityFactory<GridImp,0>
  {
    enum { codim = 0 };
    typedef typename GridImp :: template Codim<codim> :: Entity Entity;
    typedef MakeableInterfaceObject<Entity> EntityObject;
    typedef typename EntityObject :: ImplementationType EntityImp;

    template <class EntityProviderType>
    inline static EntityObject *
    getNewEntity (const GridImp & grid, EntityProviderType & ep, int level)
    {
      return ep.template getEntityObject( grid, level, (EntityImp *) 0);
    }

    template <class EntityProviderType>
    inline static void freeEntity( EntityProviderType & ep, EntityObject * e )
    {
      ep.freeObject( e );
    }
  };


  bool checkMacroGrid ( ALU3dGridElementType elType ,
                        const std::string filename );
  const char* elType2Name( ALU3dGridElementType elType );

  namespace Capabilities {

    template<int dim,int dimw, Dune::ALU3dGridElementType elType>
    struct hasLeafIterator< Dune::ALU3dGrid<dim, dimw, elType> >
    {
      static const bool v = true;
    };

    template<int dim, int dimw, Dune::ALU3dGridElementType elType, int cdim>
    struct hasEntity<Dune::ALU3dGrid<dim, dimw, elType>, cdim >
    {
      static const bool v = true;
    };

    template <int dim, int dimw, ALU3dGridElementType elType>
    struct isParallel<const ALU3dGrid<dim, dimw, elType> > {
      static const bool v = true;
    };

    template<int dim, int dimw, Dune::ALU3dGridElementType elType>
    struct isLevelwiseConforming< ALU3dGrid<dim,dimw,elType> >
    {
      static const bool v = true;
    };

    template<int dim, int dimw, Dune::ALU3dGridElementType elType>
    struct hasHangingNodes< ALU3dGrid<dim,dimw,elType> >
    {
      static const bool v = true;
    };

    template<int dim, int dimw, Dune::ALU3dGridElementType elType>
    struct hasBackupRestoreFacilities< ALU3dGrid<dim,dimw,elType> >
    {
      static const bool v = true;
    };

  } // end namespace Capabilities

} // end namespace Dune

#include "grid_imp.cc"
#endif

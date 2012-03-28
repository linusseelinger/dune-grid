// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:
#ifndef DUNE_GEOGRID_CACHEDCOORDFUNCTION_HH
#define DUNE_GEOGRID_CACHEDCOORDFUNCTION_HH

#include <cassert>
#include <memory>

#include <dune/common/typetraits.hh>

#include <dune/grid/common/gridenums.hh>

#include <dune/grid/geometrygrid/capabilities.hh>
#include <dune/grid/geometrygrid/coordfunctioncaller.hh>
#include <dune/grid/utility/persistentcontainer.hh>

namespace Dune
{

  // Internal Forward Declarations
  // -----------------------------

  template< class HostGrid, class CoordFunction, class Allocator >
  class CachedCoordFunction;



  // GeoGrid::CoordCache
  // -------------------

  namespace GeoGrid
  {

    template< class HostGrid, class Coordinate, class Allocator = std::allocator< Coordinate > >
    class CoordCache
    {
      typedef CoordCache< HostGrid, Coordinate, Allocator > This;

      static const unsigned int dimension = HostGrid::dimension;

      typedef typename HostGrid::template Codim< dimension >::Entity Vertex;

      typedef PersistentContainer< HostGrid, Coordinate, Allocator > DataCache;

    public:
      explicit CoordCache ( const HostGrid &hostGrid, const Allocator &allocator = Allocator() )
        : data_( hostGrid, dimension, allocator )
      {}

      template< class Entity >
      const Coordinate &operator() ( const Entity &entity, unsigned int corner ) const
      {
        return data_( entity, corner );
      }

      const Coordinate &operator() ( const Vertex &vertex, unsigned int corner ) const
      {
        assert( corner == 0 );
        return data_[ vertex ];
      }

      template< class Entity >
      Coordinate &operator() ( const Entity &entity, unsigned int corner )
      {
        return data_( entity,corner) ;
      }

      Coordinate &operator() ( const Vertex &vertex, unsigned int corner )
      {
        assert( corner == 0 );
        return data_[ vertex ];
      }

      void adapt ()
      {
        data_.update();
      }

    private:
      CoordCache ( const This & );
      This &operator= ( const This & );

      mutable DataCache data_;
    };

  } // namespace GeoGrid



  // CachedCoordFunction
  // -------------------

  template< class HostGrid, class CoordFunction, class Allocator = std::allocator< void > >
  class CachedCoordFunction
    : public DiscreteCoordFunction< typename CoordFunction::ctype, CoordFunction::dimRange, CachedCoordFunction< HostGrid, CoordFunction > >
  {
    typedef CachedCoordFunction< HostGrid, CoordFunction > This;
    typedef DiscreteCoordFunction< typename CoordFunction::ctype, CoordFunction::dimRange, This > Base;

  public:
    typedef typename Base::ctype ctype;

    typedef typename Base::RangeVector RangeVector;

  private:
    typedef GeoGrid::CoordCache< HostGrid, RangeVector, typename Allocator::template rebind< RangeVector >::other > Cache;

  public:
    explicit
    CachedCoordFunction ( const HostGrid &hostGrid,
                          const CoordFunction &coordFunction = CoordFunction(),
                          const Allocator &allocator = Allocator() )
      : hostGrid_( hostGrid ),
        coordFunction_( coordFunction ),
        cache_( hostGrid, allocator )
    {
      buildCache();
    }

    void adapt ()
    {
      cache_.adapt();
      buildCache();
    }

    void buildCache ();

    template< class HostEntity >
    void insertEntity ( const HostEntity &hostEntity );

    template< class HostEntity >
    void evaluate ( const HostEntity &hostEntity, unsigned int corner, RangeVector &y ) const
    {
      y = cache_( hostEntity, corner );
#ifndef NDEBUG
      typedef GeoGrid::CoordFunctionCaller< HostEntity, typename CoordFunction::Interface >
      CoordFunctionCaller;

      RangeVector z;
      CoordFunctionCaller coordFunctionCaller( hostEntity, coordFunction_ );
      coordFunctionCaller.evaluate( corner, z );
      assert( ((y - z).two_norm() < 1e-6) );
#endif
    }

  private:
    const HostGrid &hostGrid_;
    const CoordFunction &coordFunction_;
    Cache cache_;
  };



  // Implementation of CachedCoordFunction
  // -------------------------------------

  template< class HostGrid, class CoordFunction, class Allocator >
  inline void CachedCoordFunction< HostGrid, CoordFunction, Allocator >::buildCache ()
  {
    typedef typename HostGrid::template Codim< 0 >::Entity Element;
    typedef typename HostGrid::LevelGridView MacroView;
    typedef typename HostGrid::HierarchicIterator HierarchicIterator;

    typedef typename MacroView::template Codim< 0 >::template Partition< All_Partition >::Iterator MacroIterator;

    const MacroView macroView = hostGrid_.levelView( 0 );
    const int maxLevel = hostGrid_.maxLevel();

    const MacroIterator mend = macroView.template end< 0, All_Partition >();
    for( MacroIterator mit = macroView.template begin< 0, All_Partition >(); mit != mend; ++mit )
    {
      const Element &macroElement = *mit;
      insertEntity( macroElement );

      const HierarchicIterator hend = macroElement.hend( maxLevel );
      for( HierarchicIterator hit = macroElement.hbegin( maxLevel ); hit != hend; ++hit )
        insertEntity( *hit );
    }
  }


  template< class HostGrid, class CoordFunction, class Allocator >
  template< class HostEntity >
  inline void CachedCoordFunction< HostGrid, CoordFunction, Allocator >
  ::insertEntity ( const HostEntity &hostEntity )
  {
    typedef GeoGrid::CoordFunctionCaller< HostEntity, typename CoordFunction::Interface >
    CoordFunctionCaller;

    CoordFunctionCaller coordFunctionCaller( hostEntity, coordFunction_ );
    const GenericReferenceElement< ctype, HostEntity::dimension > &refElement
      = GenericReferenceElements< ctype, HostEntity::dimension >::general( hostEntity.type() );

    const unsigned int numCorners = refElement.size( HostEntity::dimension );
    for( unsigned int i = 0; i < numCorners; ++i )
      coordFunctionCaller.evaluate( i, cache_( hostEntity, i ) );
  }

} // namespace Dune

#endif // #ifndef DUNE_GEOGRID_CACHEDCOORDFUNCTION_HH

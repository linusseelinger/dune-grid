// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=4 sw=2 sts=2:
#ifndef DUNE_ALBERTA_DOFVECTOR_HH
#define DUNE_ALBERTA_DOFVECTOR_HH

#include <cstdlib>
#include <limits>

#include <dune/grid/albertagrid/misc.hh>
#include <dune/grid/albertagrid/elementinfo.hh>
#include <dune/grid/albertagrid/refinement.hh>

#if HAVE_ALBERTA

namespace Dune
{

  namespace Alberta
  {

    // External Forward Declarations
    // -----------------------------

    template< int dim >
    class MeshPointer;



    // DofVectorProvider
    // -----------------

    template< class Dof >
    struct DofVectorProvider;

    template<>
    struct DofVectorProvider< int >
    {
      typedef ALBERTA DOF_INT_VEC DofVector;

      static DofVector *get ( const DofSpace *dofSpace, const std::string &name )
      {
        return ALBERTA get_dof_int_vec( name.c_str(), dofSpace );
      }

      static void free ( DofVector *dofVector )
      {
        ALBERTA free_dof_int_vec( dofVector );
      }

      static DofVector *read ( const std::string &filename, Mesh *mesh, DofSpace *dofSpace )
      {
        return ALBERTA read_dof_int_vec_xdr( filename.c_str(), mesh, dofSpace );
      }

      static bool write ( const DofVector *dofVector, const std::string &filename )
      {
        int success = ALBERTA write_dof_int_vec_xdr( dofVector, filename.c_str() );
        return (success == 0);
      }
    };

    template<>
    struct DofVectorProvider< signed char >
    {
      typedef ALBERTA DOF_SCHAR_VEC DofVector;

      static DofVector *get ( const DofSpace *dofSpace, const std::string &name )
      {
        return ALBERTA get_dof_schar_vec( name.c_str(), dofSpace );
      }

      static void free ( DofVector *dofVector )
      {
        ALBERTA free_dof_schar_vec( dofVector );
      }

      static DofVector *read ( const std::string &filename, Mesh *mesh, DofSpace *dofSpace )
      {
        return ALBERTA read_dof_schar_vec_xdr( filename.c_str(), mesh, dofSpace );
      }

      static bool write ( const DofVector *dofVector, const std::string &filename )
      {
        int success = ALBERTA write_dof_schar_vec_xdr( dofVector, filename.c_str() );
        return (success == 0);
      }
    };

    template<>
    struct DofVectorProvider< unsigned char >
    {
      typedef ALBERTA DOF_UCHAR_VEC DofVector;

      static DofVector *get ( const DofSpace *dofSpace, const std::string &name )
      {
        return ALBERTA get_dof_uchar_vec( name.c_str(), dofSpace );
      }

      static void free ( DofVector *dofVector )
      {
        ALBERTA free_dof_uchar_vec( dofVector );
      }

      static DofVector *read ( const std::string &filename, Mesh *mesh, DofSpace *dofSpace )
      {
        return ALBERTA read_dof_uchar_vec_xdr( filename.c_str(), mesh, dofSpace );
      }

      static bool write ( const DofVector *dofVector, const std::string &filename )
      {
        int success = ALBERTA write_dof_uchar_vec_xdr( dofVector, filename.c_str() );
        return (success == 0);
      }
    };

    template<>
    struct DofVectorProvider< Real >
    {
      typedef ALBERTA DOF_REAL_VEC DofVector;

      static DofVector *get ( const DofSpace *dofSpace, const std::string &name )
      {
        return ALBERTA get_dof_real_vec( name.c_str(), dofSpace );
      }

      static void free ( DofVector *dofVector )
      {
        ALBERTA free_dof_real_vec( dofVector );
      }

      static DofVector *read ( const std::string &filename, Mesh *mesh, DofSpace *dofSpace )
      {
        return ALBERTA read_dof_real_vec_xdr( filename.c_str(), mesh, dofSpace );
      }

      static bool write ( const DofVector *dofVector, const std::string &filename )
      {
        int success = ALBERTA write_dof_real_vec_xdr( dofVector, filename.c_str() );
        return (success == 0);
      }
    };

    template<>
    struct DofVectorProvider< GlobalVector >
    {
      typedef ALBERTA DOF_REAL_D_VEC DofVector;

      static DofVector *get ( const DofSpace *dofSpace, const std::string &name )
      {
        return ALBERTA get_dof_real_d_vec( name.c_str(), dofSpace );
      }

      static void free ( DofVector *dofVector )
      {
        ALBERTA free_dof_real_d_vec( dofVector );
      }

      static DofVector *read ( const std::string &filename, Mesh *mesh, DofSpace *dofSpace )
      {
        return ALBERTA read_dof_real_d_vec_xdr( filename.c_str(), mesh, dofSpace );
      }

      static bool write ( const DofVector *dofVector, const std::string &filename )
      {
        int success = ALBERTA write_dof_real_d_vec_xdr( dofVector, filename.c_str() );
        return (success == 0);
      }
    };



    // DofVectorPointer
    // ----------------

    template< class Dof >
    class DofVectorPointer
    {
      typedef DofVectorPointer< Dof > This;

      typedef Alberta::DofVectorProvider< Dof > DofVectorProvider;

    public:
      typedef typename DofVectorProvider::DofVector DofVector;

      static const bool supportsAdaptationData = (DUNE_ALBERTA_VERSION >= 0x201);

    private:
      DofVector *dofVector_;

    public:
      DofVectorPointer ()
        : dofVector_( NULL )
      {}

      explicit DofVectorPointer ( const DofSpace *dofSpace,
                                  const std::string &name = "" )
        : dofVector_ ( DofVectorProvider::get( dofSpace, name ) )
      {}

      explicit DofVectorPointer ( DofVector *dofVector )
        : dofVector_( dofVector )
      {}

      operator DofVector * () const
      {
        return dofVector_;
      }

      operator Dof * () const
      {
        Dof *ptr = NULL;
        GET_DOF_VEC( ptr, dofVector_ );
        return ptr;
      }

      bool operator! () const
      {
        return (dofVector_ == NULL);
      }

      const DofSpace *dofSpace () const
      {
        return dofVector_->fe_space;
      }

      std::string name () const
      {
        if( dofVector_ != NULL )
          return dofVector_->name;
        else
          return std::string();
      }

      void create ( const DofSpace *dofSpace, const std::string &name = "" )
      {
        release();
        dofVector_ = DofVectorProvider::get( dofSpace, name );
      }

      template< int dim >
      void read ( const std::string &filename, const MeshPointer< dim > &meshPointer )
      {
        release();
        dofVector_ = DofVectorProvider::read( filename, meshPointer, NULL );
      }

      bool write ( const std::string &filename ) const
      {
        return DofVectorProvider::write( dofVector_, filename );
      }

      void release ()
      {
        if( dofVector_ != NULL )
        {
          DofVectorProvider::free( dofVector_ );
          dofVector_ = NULL;
        }
      }

      template< class Functor >
      void forEach ( Functor &functor )
      {
        Dof *array = (Dof *)(*this);
        FOR_ALL_DOFS( dofSpace()->admin, functor( array[ dof ] ) );
      }

      void initialize ( const Dof &value )
      {
        Dof *array = (Dof *)(*this);
        FOR_ALL_DOFS( dofSpace()->admin, array[ dof ] = value );
      }

      template< class AdaptationData >
      AdaptationData *getAdaptationData () const
      {
        assert( dofVector_ != NULL );
#if DUNE_ALBERTA_VERSION >= 0x201
        assert( dofVector_->user_data != NULL );
        return static_cast< AdaptationData * >( dofVector_->user_data );
#else
        return 0;
#endif
      }

      template< class AdaptationData >
      void setAdaptationData ( AdaptationData *adaptationData )
      {
        assert( dofVector_ != NULL );
#if DUNE_ALBERTA_VERSION >= 0x201
        dofVector_->user_data = adaptationData;
#endif // #if DUNE_ALBERTA_VERSION >= 0x201
      }

      template< class Interpolation >
      void setupInterpolation ()
      {
        assert( dofVector_ != NULL );
        dofVector_->refine_interpol = &refineInterpolate< Interpolation >;
      }

      template< class Restriction >
      void setupRestriction ()
      {
        assert( dofVector_ != NULL );
        dofVector_->coarse_restrict = &coarsenRestrict< Restriction >;
      }

    private:
      template< class Interpolation >
      static void refineInterpolate ( DofVector *dofVector, RC_LIST_EL *list, int n )
      {
        const This dofVectorPointer( dofVector );
        const Patch patch( list, n );
        Interpolation::interpolateVector( dofVectorPointer, patch );
      }

      template< class Restriction >
      static void coarsenRestrict ( DofVector *dofVector, RC_LIST_EL *list, int n )
      {
        const This dofVectorPointer( dofVector );
        const Patch patch( list, n );
        Restriction::restrictVector( dofVectorPointer, patch );
      }
    };



    // Auxilliary Functions
    // --------------------

    inline void abs ( const DofVectorPointer< int > &dofVector )
    {
      assert( !dofVector == false );
      int *array = (int *)dofVector;
      FOR_ALL_DOFS( dofVector.dofSpace()->admin,
                    array[ dof ] = std::abs( array[ dof ] ) );
    }


    inline int max ( const DofVectorPointer< int > &dofVector )
    {
      assert( !dofVector == false );
      int *array = (int *)dofVector;
      int result = std::numeric_limits< int >::min();
      FOR_ALL_DOFS( dofVector.dofSpace()->admin,
                    result = std::max( result, array[ dof ] ) );
      return result;
    }


    inline int min ( const DofVectorPointer< int > &dofVector )
    {
      assert( !dofVector == false );
      int *array = (int *)dofVector;
      int result = std::numeric_limits< int >::max();
      FOR_ALL_DOFS( dofVector.dofSpace()->admin,
                    result = std::min( result, array[ dof ] ) );
      return result;
    }


    inline int maxAbs ( const DofVectorPointer< int > &dofVector )
    {
      assert( !dofVector == false );
      int *array = (int *)dofVector;
      int result = 0;
      FOR_ALL_DOFS( dofVector.dofSpace()->admin,
                    result = std::max( result, std::abs( array[ dof ] ) ) );
      return result;
    }

  }

}

#endif // #if HAVE_ALBERTA

#endif

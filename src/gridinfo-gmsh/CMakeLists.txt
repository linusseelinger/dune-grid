if(ALBERTA_2D_LIB_FOUND AND ENABLE_GRID_EXTRA_UTILS)
  add_executable(dune-gridinfo-gmsh-alberta-2d gridinfo-gmsh-alberta-2d.cc)
  add_dune_alberta_flags(dune-gridinfo-gmsh-alberta-2d GRIDDIM 2)
  set(PROGRAMS ${PROGRAMS} dune-gridinfo-gmsh-alberta-2d)
endif(ALBERTA_2D_LIB_FOUND AND ENABLE_GRID_EXTRA_UTILS)

if(ALBERTA_3D_LIB_FOUND AND ENABLE_GRID_EXTRA_UTILS)
  add_executable(dune-gridinfo-gmsh-alberta-3d gridinfo-gmsh-alberta-3d.cc)
  add_dune_alberta_flags(dune-gridinfo-gmsh-alberta-3d GRIDDIM 3)
  set(PROGRAMS ${PROGRAMS} dune-gridinfo-gmsh-alberta-3d)
endif(ALBERTA_3D_LIB_FOUND AND ENABLE_GRID_EXTRA_UTILS)

if(ALUGRID_FOUND AND ENABLE_GRID_EXTRA_UTILS)
  add_executable(dune-gridinfo-gmsh-alucube-3d gridinfo-gmsh-alucube-3d.cc)
  add_dune_alugrid_flags(dune-gridinfo-gmsh-alucube-3d)
  add_executable(dune-gridinfo-gmsh-alusimplex-3d gridinfo-gmsh-alusimplex-3d.cc)
  add_dune_alugrid_flags(dune-gridinfo-gmsh-alusimplex-3d)
  add_executable(dune-gridinfo-gmsh-alusimplex-2d gridinfo-gmsh-alusimplex-2d.cc)
  add_dune_alugrid_flags(dune-gridinfo-gmsh-alusimplex-2d)
  set(PROGRAMS ${PROGRAMS} dune-gridinfo-gmsh-alusimplex-2d dune-gridinfo-gmsh-alusimplex-3d dune-gridinfo-gmsh-alucube-3d)
endif(ALUGRID_FOUND AND ENABLE_GRID_EXTRA_UTILS)

if(UG_FOUND AND ENABLE_GRID_EXTRA_UTILS)
  add_executable(dune-gridinfo-gmsh-ug-2d gridinfo-gmsh-ug-2d.cc)
  add_dune_ug_flags(dune-gridinfo-gmsh-ug-2d)
  add_executable(dune-gridinfo-gmsh-ug-3d gridinfo-gmsh-ug-3d.cc)
  add_dune_ug_flags(dune-gridinfo-gmsh-ug-3d)
  set(PROGRAMS ${PROGRAMS} dune-gridinfo-gmsh-ug-2d dune-gridinfo-gmsh-ug-3d)
endif(UG_FOUND AND ENABLE_GRID_EXTRA_UTILS)

if(PROGRAMS)
  install(TARGETS ${PROGRAMS} DESTINATION ${CMAKE_INSTALL_BINDIR})
endif(PROGRAMS)

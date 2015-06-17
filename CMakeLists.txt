cmake_minimum_required(VERSION 2.8.9)
cmake_policy(VERSION 2.8.9)

set(LOCAL_PROJECT_NAME DTI-Reg)
set(PRIMARY_PROJECT_NAME DTI-Reg)#for External_DCMTK.cmake
project(${LOCAL_PROJECT_NAME})

#-----------------------------------------------------------------------------
# Superbuild Option - Enabled by default
#                   Phase I:  ${LOCAL_PROJECT_NAME}_SUPERBUILD is set to ON, and the
#                             supporting packages defined in "SuperBuild.cmake"
#                             are built.  The last package in "SuperBuild.cmake"
#                             to be built is a recursive call to this
#                             file with ${LOCAL_PROJECT_NAME}_SUPERBUILD explicitly
#                             set to "OFF" to initiate Phase II
#
#                   Phase II: Build the ${LOCAL_PROJECT_NAME}, referencing the support
#                             packages built in Phase I.
#-----------------------------------------------------------------------------
option(${LOCAL_PROJECT_NAME}_SUPERBUILD "Build ${LOCAL_PROJECT_NAME} and the projects it depends on via SuperBuild.cmake." ON)
mark_as_advanced(${LOCAL_PROJECT_NAME}_SUPERBUILD)

option(STATIC_DTI-Reg "Build static DTI-Reg" ON)

if( DTI-Reg_BUILD_SLICER_EXTENSION )
  #-----------------------------------------------------------------------------
  # Extension meta-information
  set(EXTENSION_HOMEPAGE "http://www.slicer.org/slicerWiki/index.php/Documentation/Nightly/Extensions/DTI-Reg")
  set(EXTENSION_CATEGORY "Diffusion")
  set(EXTENSION_CONTRIBUTORS "Francois Budin (UNC), Clement Vachet (SCI), Martin Styner (UNC)")
  set(EXTENSION_DESCRIPTION "DTI-Reg is an application that performs pair-wise DTI registration, using scalar FA (or MD) map to drive the registration. Individual steps of the pair-wise registration pipeline are performed via external applications - some of them being 3D Slicer modules. Starting with two input DTI images, scalar FA maps are generated via dtiprocess. Registration is then performed between these FA maps, via BRAINSFit/BRAINSDemonWarp or ANTS -Advanced Normalization Tools-, which provide different registration schemes: rigid, affine, BSpline, diffeomorphic, logDemons. The final deformation is then applied to the source DTI image via ResampleDTIlogEuclidean.")
  set(EXTENSION_ICONURL "https://raw.githubusercontent.com/NIRALUser/DTI-Reg/master/DTI-Reg-icon.png")
  set(EXTENSION_SCREENSHOTURLS "http://www.slicer.org/slicerWiki/images/c/c5/DTI-Reg_-_Misaligned_DTIs.png http://www.slicer.org/slicerWiki/images/1/12/DTI-Reg_-_Aligned_DTIs_-_Coronal.png")
  set(EXTENSION_DEPENDS "DTIProcess ResampleDTIlogEuclidean") # Specified as a space separated list or 'NA' if any
  set(EXTENSION_BUILD_SUBDIRECTORY DTI-Reg-build)
  set(SUPERBUILD_TOPLEVEL_PROJECT DTI-Reg)
endif()

#-----------------------------------------------------------------------------
# Superbuild script
#-----------------------------------------------------------------------------
if(${LOCAL_PROJECT_NAME}_SUPERBUILD)
  include("${CMAKE_CURRENT_SOURCE_DIR}/SuperBuild.cmake")
  return()
else()
  include("${CMAKE_CURRENT_SOURCE_DIR}/${LOCAL_PROJECT_NAME}.cmake")
  return()
endif()

message(FATAL_ERROR "You should never reach this point !")


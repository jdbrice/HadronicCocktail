#!python
import subprocess
import os

SConscript('color_SConscript')
Import( 'env' )

ROOTCFLAGS    	= subprocess.check_output( ['root-config',  '--cflags'] ).rstrip().split( " " )
ROOTLDFLAGS    	= subprocess.check_output( ["root-config",  "--ldflags"] )
ROOTLIBS      	= subprocess.check_output( ["root-config",  "--libs"] )
ROOTGLIBS     	= subprocess.check_output( ["root-config",  "--glibs"] )
ROOTLIBPATH 	= subprocess.check_output( ["root-config", "--libdir" ] )
ROOT_SYS 		= os.environ[ "ROOTSYS" ]
JDB_LIB			= os.environ[ "JDB_LIB" ]
JDB_LIB_NAME 	= 'libRooBarb.a'

cppDefines 		= {}
cppFlags 		= ['-Wall' ]#, '-Werror']
cxxFlags 		= ['-std=c++11' ]
cxxFlags.extend( ROOTCFLAGS )

paths 			= [ '.', 			# dont really like this but ended up needing for root dict to work ok
					'include', 
					'include/UnitTests',
					JDB_LIB + "/include"
					]

exe = "hadronicCocktail.app"

########################## Project Target #####################################

common_env = env.Clone()
# common_env =  Environment()
# common_env.Append( ENV  = {'LD_LIBRARY_PATH' : LD_LIBRARY_PATH} )

common_env.Append(CPPDEFINES 	= cppDefines)
common_env.Append(CPPFLAGS 		= cppFlags)
common_env.Append(CXXFLAGS 		= cxxFlags)
common_env.Append(LINKFLAGS 	= cxxFlags ) #ROOTLIBS + " " + JDB_LIB + "/lib/libJDB.a"
common_env.Append(CPPPATH		= paths)
common_env.Append(LIBS 			= [ "libRooBarbCore.a", "libRooBarbConfig.a", "libRooBarbTasks.a", "libRooBarbRootAna.a", "libRooBarbUnitTest.a", "libRooBarbExtra.a" ] )
common_env.Append(LIBPATH 		= [ JDB_LIB + "/lib/" ] )

# Add ROOT libraries and flags
common_env[ "_LIBFLAGS" ] = common_env[ "_LIBFLAGS" ] + " " + ROOTLIBS + " "


# RooBarb Log level
jdb_log_level = ARGUMENTS.get( "ll", 60 )
common_env.Append(CXXFLAGS 		= "-DJDB_LOG_LEVEL=" + str(jdb_log_level) )

# Target build command
target = common_env.Program( target='bin/' + exe, source=[Glob( "src/*.cpp" ), Glob( "src/*/*.cpp" )] )

# helps during debug of JDB LIB
Depends( target, Glob( JDB_LIB + "/include/jdb/*" ) )

# set as the default target
Default( target )



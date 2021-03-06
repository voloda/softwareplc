#!/bin/sh
# Type --help for usage of this script file
#

# build dependent directories - filled with configure
# skript
SKELETON_DIRECTORY="/usr/${prefix}/share/plc/skeleton"
PLC_INCLUDE_DIR="/usr/${prefix}/include/plc"

# reset important variables used in script
OUTPUT=""
PROJECT_NAME=""
PROJECT_TARGET=""

function help()
{
	echo "Software PLC for RT-Linux"
	echo ""
	echo "  --help              Shows this text"
	echo "  --create pname      Creates new PLC project in current directory or in"
	echo "                      directory specified by --target-directory-option."
	echo "                      Project name will be pname (name of RT module will"
	echo "                      be plc_pname.o"
	echo "  --target-dir dir    Project will be created in directory dir"
	echo "  --cppflags          Flags for C++ compiler"

}

# proceed supported options
while ( test $# -ge 1 ); do
	case "$1" in
		--help)
			help
		;;
		--create)
			shift 
			PROJECT_NAME=$1
		;;
		--target-dir)
			shift
			PROJECT_TARGET=$1
		;;
		--cppflags)
			OUTPUT="$OUTPUT -I$PLC_INCLUDE_DIR"
		;;
		*)
			echo "Unknown parameter $1. Try --help to show help"
			exit 1
		;;
	esac
	shift 
done

if ( test -n "$PROJECT_NAME" ); then
	echo "Creating project: $PROJECT_NAME from skeleton: $SKELETON_DIRECTORY"
	echo "(Note: you must have CONFIGURATION block with name: $PROJECT_NAME"
	echo " in your project!)"
	echo -n "Target directory: "

	if ( test -z "$PROJECT_TARGET" ); then
		PROJECT_TARGET="."
		echo "current directory"
	else
		echo "$PROJECT_TARGET"
	fi

	# create project target directory, if it doesn't exist
	if ( test ! -d "$PROJECT_TARGET" ); then
		echo "Directory doesn't exist. Creating..."
		mkdir -p "$PROJECT_TARGET"
	fi

	# copy directory structure from skeleton directory into
	# project directory
	for a in `find "$SKELETON_DIRECTORY" -type d -name '*'`; do
		dname=${a:${#SKELETON_DIRECTORY}}
		mkdir -p "$PROJECT_TARGET/$dname"
	done

	# copy all files from skeleton directory into
	# project directory
	for a in `find "$SKELETON_DIRECTORY" -type f -name '*'`; do
		fname=${a:${#SKELETON_DIRECTORY}}
		cp -R "$a" "$PROJECT_TARGET/$fname"
	done

	# generate RT-Linux configuration makefile
	rtl-config --mk > "$PROJECT_TARGET/sources/rtl.mk"

	echo "Now edit Makefile:"
	echo "   $PROJECT_TARGET/Makefile"
	echo "and add names of PLC source files as described in file"
	
	sed "s/@PROJECT@/$PROJECT_NAME/g" "$PROJECT_TARGET/Makefile.in" > "$PROJECT_TARGET/Makefile"

	mkdir -p "$PROJECT_TARGET/deps"
fi

if ( test -n "$OUTPUT" ); then
	echo $OUTPUT
fi

exit 0


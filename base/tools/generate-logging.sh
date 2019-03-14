#! /bin/sh

prepare_doxyfile()
{
	cat <<__END
DOXYFILE_ENCODING = UTF-8
RECURSIVE = YES
INPUT_ENCODING = UTF-8
BRIEF_MEMBER_DESC = NO
REPEAT_BRIEF = NO
GENERATE_TODOLIST = NO
GENERATE_TESTLIST = NO
GENERATE_BUGLIST = NO
GENERATE_DEPRECATEDLIST = NO
STRIP_CODE_COMMENTS = YES
XML_PROGRAMLISTING = NO
GENERATE_HTML = NO
GENERATE_LATEX = NO
GENERATE_XML = YES
CLASS_GRAPH = YES
COLLABORATION_GRAPH = NO
GROUP_GRAPHS = NO
INCLUDE_GRAPH = NO
INCLUDED_BY_GRAPH = NO
CALL_GRAPH = NO
CALLER_GRAPH = NO
GRAPHICAL_HIERARCHY = NO
DIRECTORY_GRAPH = YES
__END
}

ORIG_PWD=`pwd`
SELF_LOC=`realpath $0`
SELF_DIR=`dirname $SELF_LOC`

init()
{
	TDIR="$(mktemp -d)"
	pushd "${TDIR}" >&2
}

cleanup()
{
	popd >&2
	rm -rf "${TDIR}"
}

die()
{
	cleanup
	echo "${@}" >&2
	exit 1
}

init
prepare_doxyfile > Doxyfile

if [ -n "$1" ]; then
	echo "INPUT = $1" >> Doxyfile
	shift
else
	echo "INPUT = $ORIG_PWD/src" >> Doxyfile
fi

while [ -n "$1" ]; do
	echo "INPUT += $1" >> Doxyfile
	shift
done

doxygen >&2 || die "doxygen has failed"

cd xml \
&& xsltproc "$SELF_DIR/generate-logging.xsl" classBeeeOn_1_1Loggable.xml \
|| die "xsltproc has failed"

cleanup

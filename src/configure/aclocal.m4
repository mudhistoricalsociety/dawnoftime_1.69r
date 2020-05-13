dnl ##################################################################
dnl This macro came from aclocal.m4 in the sample code associated
dnl with "UNIX Network Programming: Volume 1", Second Edition,
dnl by W. Richard Stevens.  That sample code is available via
dnl ftp://ftp.kohala.com/pub/rstevens/unpv12e.tar.gz.  
dnl
dnl We cannot use the AC_CHECK_TYPE macros because AC_CHECK_TYPE
dnl #includes only <sys/types.h>, <stdlib.h>, and <stddef.h>.
dnl Unfortunately, many implementations today hide typedefs in wierd
dnl locations: Solaris 2.5.1 has uint8_t and uint32_t in <pthread.h>.
dnl SunOS 4.1.x has int8_t in <sys/bitypes.h>.
dnl So we define our own macro AC_DAWN_CHECK_TYPE that includes more,
dnl then looks for the typedef.
dnl
dnl This macro should be invoked after all the header checks have been
dnl performed, since we #include "confdefs.h" below, and then use the
dnl HAVE_foo_H values that it can #define.
dnl
AC_DEFUN(AC_DAWN_CHECK_TYPE,
	[AC_MSG_CHECKING(if $1 defined)
	AC_CACHE_VAL(ac_cv_type_$1,
		AC_TRY_COMPILE(
[
#include	"confdefs.h"	/* the header built by configure so far */
#ifdef	HAVE_SYS_TYPES_H
#  include	<sys/types.h>
#endif
#ifdef	HAVE_SYS_SOCKET_H
#  include	<sys/socket.h>
#endif
#ifdef	HAVE_SYS_TIME_H
#  include    <sys/time.h>
#endif
#ifdef	HAVE_NETINET_IN_H
#  include    <netinet/in.h>
#endif
#ifdef	HAVE_ARPA_INET_H
#  include    <arpa/inet.h>
#endif
#ifdef	HAVE_ERRNO_H
#  include    <errno.h>
#endif
#ifdef	HAVE_FCNTL_H
#  include    <fcntl.h>
#endif
#ifdef	HAVE_NETDB_H
#  include	<netdb.h>
#endif
#ifdef	HAVE_SIGNAL_H
#  include	<signal.h>
#endif
#ifdef	HAVE_STDIO_H
#  include	<stdio.h>
#endif
#ifdef	HAVE_STDLIB_H
#  include	<stdlib.h>
#endif
#ifdef	HAVE_STRING_H
#  include	<string.h>
#endif
#ifdef	HAVE_SYS_STAT_H
#  include	<sys/stat.h>
#endif
#ifdef	HAVE_SYS_UIO_H
#  include	<sys/uio.h>
#endif
#ifdef	HAVE_UNISTD_H
#  include	<unistd.h>
#endif
#ifdef	HAVE_SYS_WAIT_H
#  include	<sys/wait.h>
#endif
#ifdef	HAVE_SYS_UN_H
#  include	<sys/un.h>
#endif
#ifdef	HAVE_SYS_SELECT_H
# include   <sys/select.h>
#endif
#ifdef	HAVE_STRINGS_H
# include   <strings.h>
#endif
#ifdef	HAVE_SYS_IOCTL_H
# include   <sys/ioctl.h>
#endif
#ifdef	HAVE_SYS_FILIO_H
# include   <sys/filio.h>
#endif
#ifdef	HAVE_SYS_SOCKIO_H
# include   <sys/sockio.h>
#endif
#ifdef	HAVE_PTHREAD_H
#  include	<pthread.h>
#endif],
		[ $1 foo ],
		ac_cv_type_$1=yes,
		ac_cv_type_$1=no))
	AC_MSG_RESULT($ac_cv_type_$1)
	if test $ac_cv_type_$1 = no ; then
		AC_DEFINE($1, $2)
	fi
])

AC_DEFUN(
        [CHECK_GNU_MAKE], [ AC_CACHE_CHECK( for GNU make,_cv_gnu_make_command,
                _cv_gnu_make_command='' ;
dnl Search all the common names for GNU make
                for a in "$MAKE" make ; do
                        if test -z "$a" ; then continue ; fi ;
                        if  ( sh -c "$a --version" 2> /dev/null | grep GNU  2>&1 > /dev/null ) ;  then
                                _cv_gnu_make_command=$a ;
                                break;
                        fi
                done ;
        ) ;
dnl If there was a GNU version, then set @ifGNUmake@ to the empty string, '#' otherwise
dnl ifNOGNUmake is set to the opposite of ifGNUmake
        if test  "x$_cv_gnu_make_command" != "x"  ; then
                ifGNUmake='' ;
                ifBSDmake='#' ;
                ifSTDmake='#' ;
                AC_MSG_RESULT(Found);
        else
                ifGNUmake='#' ;
                AC_MSG_RESULT(Not found);
        fi
        AC_SUBST(ifGNUmake)
        AC_SUBST(ifBSDmake)
        AC_SUBST(ifSTDmake)

] )


AC_DEFUN(
        [CHECK_BSD_MAKE], [ AC_CACHE_CHECK( for BSD make,_cv_bsd_make_command,
                _cv_bsd_make_command='' ;
dnl Search all the common names for BSD make
                for a in "$MAKE" make ; do
                        if test -z "$a" ; then continue ; fi ;
                        if  ( sh -c "$a -d A -f nosuchmakefilename " 2>&1 | grep "lobal.*\=" | grep BSD 2>&1 > /dev/null ) ;  then
                        	if  ( sh -c "$a -d A -f nosuchmakefilename " 2>&1 | grep "\.CURDIR" 2>&1 > /dev/null ) ;  then
                               	 	_cv_bsd_make_command=$a ;
                                	break;
				fi
                        fi
                done ;
        ) ;
dnl If there was a BSD version, then set variables accordingly
        if test  "x$_cv_bsd_make_command" != "x"  ; then
                ifBSDmake='' ;
                ifSTDmake='#' ;
                AC_MSG_RESULT(Found);
        else
                ifBSDmake='#' ;
                AC_MSG_RESULT(Not found);
        fi
        AC_SUBST(ifGNUmake)
        AC_SUBST(ifBSDmake)
        AC_SUBST(ifSTDmake)
] )



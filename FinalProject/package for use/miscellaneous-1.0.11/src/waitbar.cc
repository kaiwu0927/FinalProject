/**************************************************************************
 *  Waitbar function -- displays progress of lengthy calculations
 *  -------------------------------------------------------------
 *  Copyright (c) 2002  Quentin Spencer <qspencer@ieee.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, see <http://www.gnu.org/licenses/>.
 *
 *************************************************************************/

#include <octave/oct.h>

// Note the extern "C" is need for mingw with a version of termcap.h 
// without the extern "C" explicitly included. Doing it twice should be
// harmless.
extern "C" {
#if defined (HAVE_TERM_H)
#  include <term.h>
#elif defined (HAVE_TERMCAP_H)
#  include <termcap.h>
#endif
};

#define BUF_SIZE	256
#define MAX_LEN		240
#define DEFAULT_LEN	50
#define	BAR_CHAR	'#'
#define SPACING		3

static bool no_terminal=false;

DEFUN_DLD(waitbar, args, nargout,
"waitbar(...);\n\
 WAITBAR displays a text-based wait bar. This function\n\
 is similar to the Matlab waitbar command, but it is\n\
 a text, rather than graphical function.\n\n\
 A typical usage of WAITBAR in a lengthy computation\n\
 (inside a FOR loop, for example) is as follows:\n\n\
 for i=1:1000\n\
     ## computation\n\
     waitbar(i/1000);\n\
 end\n\n\
 WAITBAR(X,TITLE), where 0 <= X <= 1, sets the position of\n\
 the waitbar to the fractional length X. Values of X exactly\n\
 equal to 0 or 1 clear the waitbar. The optional second\n\
 argument TITLE sets the waitbar caption to TITLE.\n\n\
 If Octave is running in a smart terminal, the width is\n\
 automatically detected, and the title is displayed in the\n\
 waitbar (and truncated if it is too long). Otherwise, the\n\
 title is not displayed and the width is initialized to a\n\
 default of 50 characters, or it can be set to N characters\n\
 with WAITBAR(0,N). If no terminal is detected (such as when\n\
 Octave is run in batch mode and output is redirected), no\n\
 output is generated.\n\n\
 For compatibility with the Matlab version of this function\n\
 (which is graphical rather than text-based), additional\n\
 arguments are ignored, but there are no guarantees of perfect\n\
 compatibility.")
{
  static char print_buf[BUF_SIZE];
  static int n_chars_old;
  static int pct_int_old;
  static int length;
#if defined(USE_TERM)
  static char term_buffer[1024];
  static char *begin_rv, *end_rv;
  static int brvlen, ervlen, pct_pos;
  static bool smart_term;
  static bool newtitle = false;
  static charMatrix title;
  int j;
#endif
  static char *term;
  static bool init;

  double pct;
  int i;

  octave_value_list retval;
  retval(0) = Matrix(0,0);
  int nargin = args.length();
  if (nargin < 1) {
    print_usage ();
    return retval;
  }

  if(no_terminal)
    return retval;

  pct	= args(0).double_value();
  if(pct>1.0)	pct	= 1.0;		// to prevent overflow

#if defined(USE_TERM)
  if(nargin==2 && args(1).is_string())
    {
      newtitle = true;
      title = args(1).string_value();
    }
  if(nargin==3)
    {
      newtitle = true;
      title = args(2).string_value();
    }
#endif

  if(pct==0.0 || pct==1.0)
    {
      init = true;
      term = getenv("TERM");
      if(!term)
	{
	  no_terminal	= true;
	  return retval;
	}
#if defined (USE_TERM)
      i = tgetnum("co");
      smart_term = i ? true : false;
      if(nargin==1 || args(1).is_string())
	length = smart_term ? i-1 : DEFAULT_LEN;
#else
      if(nargin==1)
        length = DEFAULT_LEN;
#endif
      else
	if(nargin==2 && !(args(1).is_string()))
	{
	  length	= args(1).int_value();
	  if(length>MAX_LEN)	length	= MAX_LEN;
	  if(length<=0)		length	= DEFAULT_LEN;
	}
#if defined (USE_TERM)
      pct_pos = length/2-2;
      if(smart_term)
	{
	  // get terminal strings ("rv"="reverse video")
	  char* buf_ptr	= term_buffer;
	  begin_rv	= tgetstr("so", &buf_ptr);
	  end_rv	= tgetstr("se", &buf_ptr);
	  
	  // Display a progress bar, but only if the current terminal has a
	  // standout mode
	  if (begin_rv && end_rv)
	    {
	      brvlen = 0;	
	      buf_ptr = begin_rv;
	      while(buf_ptr[++brvlen]);
	      ervlen = 0;	buf_ptr = end_rv;
	      while(buf_ptr[++ervlen]);
	    }
	 	  
	  // initialize print buffer
	  for(i=0; i<BUF_SIZE; ++i)
	    print_buf[i]	= ' ';
	  print_buf[length+brvlen+ervlen+1] = '\r';
	  print_buf[length+brvlen+ervlen+2] = '\0';
	  for(i=0; i<brvlen; ++i)
	    print_buf[i]	= begin_rv[i];
	  for(i=0; i<ervlen; ++i)
	    print_buf[i+brvlen]	= end_rv[i];
	  fputs(print_buf,stdout);
	  if(title.length())
	    newtitle	= true;
	}
      else
	{
#endif
	  for(i=0; i<BUF_SIZE; ++i)
	    print_buf[i]	= ' ';
	  print_buf[length+8]	= '\r';
	  print_buf[length+9]	= '\0';
	  fputs(print_buf,stdout);
	  print_buf[0]		= '[';
	  print_buf[length+1]	= ']';
#if defined (USE_TERM)
	}
#endif
      n_chars_old	= 0;
      fflush(stdout);
      return retval;
    }
  else
    {
      // calculate position
      int n_chars=(int)(pct*length+0.5);
      int pct_int=(int)(pct*100.0+0.5);

      // check to see if we got this far without initialization
      if(init==false)
	{
	  Fwaitbar(octave_value(0.0),0);
	  fputs(print_buf,stdout);
	  fflush(stdout);
	}

      // check to see of output needs to be updated
#if !defined (USE_TERM)
      if(n_chars!=n_chars_old || pct_int!=pct_int_old)
	{
#else
      if(n_chars!=n_chars_old || pct_int!=pct_int_old || newtitle)
	{
	  if(smart_term)
	    {
	      static char pct_str[5];
	      sprintf(pct_str,"%3i%%",pct_int);

	      // Insert the title
	      if(newtitle)
		{
		  pct_pos	= length-SPACING*2;
		  for(i=SPACING+brvlen; i<pct_pos+brvlen-SPACING;  ++i)
		    print_buf[ i>=n_chars_old+brvlen ? i+ervlen : i ] = ' ';
		  for(i=SPACING+brvlen, j=0; j<title.length(); ++i, ++j)
		    if(i<pct_pos+brvlen-SPACING)
		      print_buf[ i>=n_chars_old ? i+ervlen : i ] = title(0,j);
		  newtitle	= false;
		}

	      // Insert the percentage string
	      for(i=pct_pos+brvlen, j=0; j<4; ++i, ++j)
		print_buf[ i>=n_chars_old+brvlen ? i+ervlen : i ] = pct_str[j];

	      // Move print_buf characters
	      if(n_chars_old<n_chars)
		for(i=n_chars_old+brvlen; i<n_chars+brvlen; ++i)
		  print_buf[i]	= print_buf[i+ervlen];
	      else
		for(i=n_chars_old+brvlen-1; i>=n_chars+brvlen; --i)
		  print_buf[i+ervlen]	= print_buf[i];

	      // Insert end of reverse video
	      for(i=n_chars+brvlen, j=0; j<ervlen; ++i, ++j)
		print_buf[i]	= end_rv[j];
	    }
	  else
	    {
#endif
	      if(n_chars>=n_chars_old)
		for(int i=n_chars_old+1; i<=n_chars; ++i)
		  print_buf[i]	= BAR_CHAR;
	      else
		for(int i=n_chars+1; i<=n_chars_old; ++i)
		  print_buf[i]	= ' ';
	      sprintf(&(print_buf[length+3])," %3i%%\r",pct_int);
#if defined (USE_TERM)
	    }
#endif
	  fputs(print_buf,stdout);
	  fflush(stdout);
	  n_chars_old	= n_chars;
	  pct_int_old	= pct_int;
	}
    }
  return retval;
}

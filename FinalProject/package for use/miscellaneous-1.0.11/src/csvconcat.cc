/*
 * Copyright (C) 2004 Laurent Mazet
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; If not, see <http://www.gnu.org/licenses/>.
 */

/* 2004-04-11
 *   initial release
 */

#include <octave/oct.h>
#include <octave/Cell.h>

DEFUN_DLD (csvconcat, args, nargout,
           "-*- texinfo -*-\n"
           "@deftypefn {Function File} {@var{str} = } csvconcat (@var{c})\n"
	   "@deftypefnx {Function File} {@var{str} = } csvconcat (@var{c}, @var{sep})\n"
	   "@deftypefnx {Function File} {@var{str} = } csvconcat (@var{c}, @var{sep}, @var{prot})\n"
	   "\n"
	   "Concatenate a cell into a CSV string or array of strings. "
	   "@var{sep} changes the character used to separate two fields. By "
	   "default, two fields are expected to be separated by a coma "
	   "(@code{,}). @var{prot} changes the character used to protect a string. "
	   "By default it's a double quote (@code{\"}).\n"
           "@end deftypefn") {

  /* Check argument */
  if ((args.length() < 1) || (args.length() > 3)) {
    print_usage ();
    return octave_value();
  }

  /* Get arguments */
  Cell c = args(0).cell_value();

  std::string sep = (args.length() > 1) ? args(1).string_value() : ",";
  if (sep.length() != 1) {
    error("Only on charactere need as separator\n");
    return octave_value();
  }

  std::string prot = (args.length() > 2) ? args(2).string_value() : "\"";
  if (prot.length() != 1) {
    error("Only on charactere need as protector\n");
    return octave_value();
  }
  
  /* Concat a cell into a string */
  string_vector vec(c.rows());
  std::string word;

  /* For each element */
  for (int i=0, il=c.rows(); i<il; i++) {
    word = "";
    for (int j=0, jl=c.columns(); j<jl; j++) {
      
      /* Add separator */
      if (j != 0)
	word += sep;

      if (c(i, j).is_real_scalar()) {

	/* Output real value */
	char tmp[20];
	sprintf(tmp, "%g", c(i, j).double_value());
	word += tmp;
      }

      else if (c(i, j).is_string()) {
	/* Output string value */
	std::string str = c(i, j).string_value();
	if (str.find(sep) != str.npos) {
	  unsigned int pos = 0;
	  while ((pos=str.find(prot, pos)) != str.npos) {
	    str.replace(pos, 1, prot+prot);
	    pos += 2;
	  }
	  str = prot + str + prot;
	}
	word += str;
      }

      else {
	/* Output NaN value */
	warning ("not a real or a string\n");
	word += "NaN";
      }
    }
    vec(i) = word;
  }
  
  return octave_value(vec);
}

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

#include <fstream>

#include <octave/oct.h>
#include <octave/Cell.h>

DEFUN_DLD (cell2csv, args, nargout,
           "-*- texinfo -*-\n"
           "@deftypefn {Function File} {} cell2csv (@var{file}, @var{c})\n"
	   "@deftypefnx {Function File} {} cell2csv (@var{file}, @var{c}, @var{sep})\n"
	   "@deftypefnx {Function File} {} cell2csv (@var{file}, @var{c}, @var{sep}, @var{prot})\n"
	   "\n"
	   "Create a CSV file from a cell. "
	   "@var{sep} changes the character used to separate two fields. By "
	   "default, two fields are expected to be separated by a coma "
	   "(@code{,}). @var{prot} changes the character used to protect a string. "
	   "By default it's a double quote (@code{\"}).\n"
           "@end deftypefn") {

  /* Check argument */
  if ((args.length() < 2) || (args.length() > 4)) {
    print_usage ();
    return octave_value();
  }

  /* Get arguments */
  std::string file = args(0).string_value();

  Cell c = args(1).cell_value();

  std::string sep = (args.length() > 2) ? args(2).string_value() : ",";
  if (sep.length() != 1) {
    error("Only on charactere need as separator\n");
    return octave_value();
  }

  std::string prot = (args.length() > 3) ? args(3).string_value() : "\"";
  if (prot.length() != 1) {
    error("Only on charactere need as protector\n");
    return octave_value();
  }

  /* Open file */
  std::ofstream fd(file.c_str());
  if (!fd.is_open()) {
    error("cannot write %s\n", file.c_str());
    return octave_value();
  }

  /* Concat a cell into a string */
  std::string word;

  /* For each element */
  for (int i=0, il=c.rows(); i<il; i++) {
   
    for (int j=0, jl=c.columns(); j<jl; j++) {
      word = "";
      
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
	  size_t pos = 0;
	  while ((pos=str.find(prot, pos)) != str.npos) {
	    str.replace(pos, 1, prot+prot);
	    pos += 2;
	  }
	  str = prot + str + prot;
	}
	word += str;
      }

      else if (!c(i, j).is_empty()) {
	/* Output NaN value */
	warning ("not a real or a string\n");
	word += "NaN";
      }

      fd << word;
    }

    /* Add end of line */
    fd << std::endl;
  }

  /* Close file */
  fd.close();

  return octave_value();
}

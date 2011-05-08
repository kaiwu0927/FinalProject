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

/* 2004-04-08
 *   initial release
 */

#include <octave/oct.h>
#include <octave/Cell.h>

DEFUN_DLD (csvexplode, args, nargout,
           "-*- texinfo -*-\n"
           "@deftypefn {Function File} {@var{c} = } csvexplode (@var{str})\n"
	   "@deftypefnx {Function File} {@var{c} = } csvexplode (@var{str}, @var{sep})\n"
	   "@deftypefnx {Function File} {@var{c} = } csvexplode (@var{str}, @var{sep}, @var{prot})\n"
	   "\n"
	   "Explode a CSV string into a cell. "
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
  if (!args(0).is_string()) {
    if (args(0).is_cell())
      return octave_value(args(0));
    else
      return octave_value(Cell(args(0)));
  }
  std::string str = args(0).string_value();

  std::string _sep = (args.length() > 1) ? args(1).string_value() : ",";
  if (_sep.length() != 1) {
    error("Only on charactere need as separator\n");
    return octave_value();
  }
  char sep = _sep[0];

  std::string _prot = (args.length() > 2) ? args(2).string_value() : "\"";
  if (_prot.length() != 1) {
    error("Only on charactere need as protector\n");
    return octave_value();
  }
  char prot = _prot[0];

  /* Explode a line into a cell */
  Cell retval;
  std::string word;
  bool inside = false;
  for (int i=0, len=str.length(); i<=len; i++) {
    if (((i==len) || (str[i] == sep)) && (!inside)) {

      /* Extand cell */
      retval.resize(dim_vector(1, retval.columns()+1));

      /* Check if scalar */
      const char *word_str=word.c_str();
      char *err;
      double val = strtod (word_str, &err);

      /* Store into the cell */
      retval(0, retval.columns()-1) =
	((word == "") || (err != word_str+word.length())) ?
	octave_value(word) : octave_value(val);

      word = "";
    }
    else if ((inside) && (str[i]==prot) && ((i<len) && (str[i+1]==prot))) {

      /* Insisde a string */
      word += prot;
      ++i;
    }
    else if (str[i] == prot)
      /* Changing */
      inside = !inside;
    else
      word += str[i];
  }

  return octave_value(retval);
}

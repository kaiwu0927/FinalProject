/*

Copyright (C) 2010 Olaf Till

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or (at
your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.

*/

#include <octave/oct.h>

DEFUN_DLD (cell2cell, args, , 
  "-*- texinfo -*-\n\
@deftypefn {Loadable Function} {} cell2cell (@var{c}, @var{dim})\n\
Return a one-dimensional cell array, extending along dimension @var{dim}, which contains the slices of cell array @var{c} vertical to dimension @var{dim}.\n\
@end deftypefn")
{
  std::string fname ("cell2cell");

  if (args.length () != 2)
    {
      print_usage ();
      return octave_value_list ();
    }

  Cell c = args(0).cell_value ();
  if (error_state)
    {
      error ("%s: first argument must be a cell array", fname.c_str ());
      return octave_value_list ();
    }

  octave_idx_type dim = args(1).int_value ();
  if (error_state)
    {
      error ("%s: second argument must be an integer",
	     fname.c_str ());
      return octave_value_list ();
    }

  octave_idx_type i, j;

  dim_vector cdims (c.dims ());

  octave_idx_type n_cdims = cdims.length ();

  dim_vector rdims, tdims (cdims);

  octave_idx_type nr = 1;

  if (n_cdims >= dim && cdims(dim - 1) > 1)
    {
      if (dim == 1)
	{
	  rdims.resize (2);

	  rdims(1) = 1;
	}
      else
	rdims.resize (dim);

      for (i = 0; i < dim - 1; i++)
	rdims(i) = 1;

      rdims(dim - 1) = (nr = cdims(dim - 1));

      tdims(dim - 1) = 1;

      tdims.chop_trailing_singletons ();
    }
  else
    {
      rdims.resize (2);

      rdims(0) = 1;
      rdims(1) = 1;
    }

  Cell retval (rdims);

  octave_idx_type nt = tdims.numel ();

  octave_idx_type base = 0, origin, cursor;

  octave_idx_type skip_count = 1; // when to skip

  octave_idx_type min_tp = n_cdims < dim - 1 ? n_cdims : dim - 1;

  for (i = 0; i < min_tp; i++)
    skip_count *= cdims(i);

  octave_idx_type skip = skip_count; // how much to skip

  if (n_cdims >= dim)
    skip *= cdims(dim - 1);

  for (i = 0; i < nr; i++)
    {
      Cell t (tdims);

      origin = base;

      cursor = 0;

      for (j = 0; j < nt; j++)
	{
	  t(j) = c(origin + cursor++);

	  if (cursor == skip_count)
	    {
	      cursor = 0;

	      origin += skip;
	    }
	}

      retval(i) = t;

      base += skip_count;
    }

  return octave_value (retval);
}

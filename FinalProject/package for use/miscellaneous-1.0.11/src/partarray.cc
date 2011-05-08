/*

Copyright (C) 2010, 2011 Olaf Till

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
#include <octave/ov-struct.h>

template<class NDA>
static Cell
do_partarray (const NDA& a, const std::string& fname,
	      const octave_value_list& args)
{
  // FIXME: some type-independent code should probably go into an
  // extra function (no template) to avoid its repetitive instanciation

  int nidc = args.length () - 1;

  dim_vector dv = a.dims ();

  int ndims = dv.length ();

  int maxdims = ndims > nidc ? ndims : nidc;

  Array<octave_idx_type> alldims (dim_vector (maxdims, 1));
  for (int i = 0; i < maxdims; alldims(i) = i < ndims ? dv(i) : 1, i++);

  int nc = 1;
  Array<int> nidx (dim_vector (maxdims, 1));
  // Octave-3.2.4 reports "missing symbol" with Array<Array< > >,
  // though 3.3.54+ does not
  Array<octave_idx_type> bidc [maxdims], eidc [maxdims];
  //
  Array<octave_idx_type> step (dim_vector (maxdims, 1));
  step(0) = 1;
  for (int i = 0; i < maxdims; i++)
    {
      int cnidx;
      octave_value arg;
      if (i >= nidc || (arg = args(i + 1)).is_empty ())
	{
	  Array<octave_idx_type> bidx (dim_vector (1, 1), 1);
	  Array<octave_idx_type> eidx (dim_vector (1, 1), alldims(i));
	  bidc[i] = bidx;
	  eidc[i] = eidx;
	  cnidx = 1;
	}
      else
	{
	  dim_vector argdims = arg.dims ();
	  if (argdims.length () > 2 || argdims(1) != 2)
	    {
	      error ("%s: argument %i: wrong dimensions",
		     fname.c_str (), i + 2);
	      return Cell ();
	    }
	  cnidx =
	    (bidc[i] = octave_value (arg.matrix_value ().column (0)).
	     octave_idx_type_vector_value ()).length ();
	  eidc[i] = octave_value (arg.matrix_value ().column (1)).
	    octave_idx_type_vector_value ();
	  for (int j = 0; j < cnidx; j++)
	    {
	      if (bidc[i](j) < 1 || eidc[i](j) > alldims(i))
		error ("%s: index given by row %i of argument %i exceeds array dimensions",
		       fname.c_str (), j, i + 2);
	      if (bidc[i](j) > eidc[i](j))
		// so later length calculations yield 0
		eidc[i](j) = bidc[i](j) - 1;
	    }
	}
      nc *= (nidx(i) = cnidx);
      if (i < maxdims - 1)
	step(i + 1) = step(i) * alldims(i);
    }
  if (error_state)
    return Cell ();

  dim_vector rdv (nc, 1);

  Cell retval (rdv);

  // preparations are done, start now; take care, indices are
  // one-based, since given by user

  // go through all combinations of indices into user indices
  Array<int> cidx (dim_vector (maxdims, 1), 0); // current combination
  for (int i = 0; i < nc; i++)
    {
      // set cursor to start of subarray and calculate some lengths
      octave_idx_type n = 1, cursor = 0;
      Array<octave_idx_type> lengths (dim_vector (maxdims, 1));
      for (int j = 0; j < maxdims; j++)
	{
	  octave_idx_type begin = bidc[j](cidx(j));
	  octave_idx_type end = eidc[j](cidx(j));
	  octave_idx_type length = end + 1 - begin;
	  lengths(j) = length;
	  n *= length;
	  cursor += (begin - 1) * step(j);
	}
      Array<octave_idx_type> starts (dim_vector (maxdims - 1, 1), cursor);
      Array<octave_idx_type> idx_cursors (dim_vector (maxdims, 1), 0);
      // copy the subarray
      dim_vector subdv;
      subdv.resize (maxdims);
      for (octave_idx_type k = 0; k < maxdims; k++)
	subdv(k) = lengths(k);
      NDA asub (subdv);
      for (octave_idx_type k = 0; k < n; k++)
	{
	  asub(k) = a(cursor++);
	  idx_cursors(0)++;
	  for (int l = 0; l < maxdims - 1; l++)
	    {
	      if (idx_cursors(l) == lengths(l))
		{
		  idx_cursors(l) = 0;
		  idx_cursors(l + 1)++;
		  cursor = starts(l) + step(l + 1);
		  for (int m = 0; m <= l; starts(m) = cursor, m++);
		}
	    }
	}
      // insert subarray into cell
      retval(i, 0) = octave_value (asub);
      // prepare next combination
      cidx(0)++;
      for (int j = 0; j < maxdims - 1; j++)
	if (cidx(j) >= nidx(j))
	  {
	    cidx(j) = 0;
	    cidx(j + 1)++;
	  }
    }

  return retval;
}

DEFUN_DLD (partarray, args, ,
  "-*- texinfo -*-\n\
@deftypefn {Loadable Function} {} partarray (@var{a}, @var{idx}, ...)\n\
Return a column cell array with subarrays of the @var{a}. Start and end indices of subarrays are given in the rows of @var{idx}. @var{idx} can be given for each dimension, empty @var{idx} means no partitioning in the respective dimension. Order of returned subarrays: rows of @var{idx} of some dimension are completely used before using the next row of @var{idx} of the higher dimension.\n\
\n\
Does not work with structure arrays at the moment.\n\
@end deftypefn")
{
  std::string fname ("partarray");

  if (args.length () == 0)
    {
      print_usage ();
      return octave_value_list ();
    }

  octave_value array = args(0);
  octave_value retval;

  // The idea to use a function template was taken from Octaves
  // num2cell in src/DLD-FUNCTIONS/cellfun.cc. The following code,
  // which distinguishes the applicable types for the template, was
  // cut and pasted from there and adapted.
  if (array.is_bool_type ())
    retval = do_partarray (array.bool_array_value (), fname, args);
  else if (array.is_char_matrix ())
    retval = do_partarray (array.char_array_value (), fname, args);
  else if (array.is_numeric_type ())
    {
      if (array.is_integer_type ())
	{
	  if (array.is_int8_type ())
	    retval = do_partarray (array.int8_array_value (), fname, args);
	  else if (array.is_int16_type ())
	    retval = do_partarray (array.int16_array_value (), fname, args);
	  else if (array.is_int32_type ())
	    retval = do_partarray (array.int32_array_value (), fname, args);
	  else if (array.is_int64_type ())
	    retval = do_partarray (array.int64_array_value (), fname, args);
	  else if (array.is_uint8_type ())
	    retval = do_partarray (array.uint8_array_value (), fname, args);
	  else if (array.is_uint16_type ())
	    retval = do_partarray (array.uint16_array_value (), fname, args);
	  else if (array.is_uint32_type ())
	    retval = do_partarray (array.uint32_array_value (), fname, args);
	  else if (array.is_uint64_type ())
	    retval = do_partarray (array.uint64_array_value (), fname, args);
	}
      else if (array.is_complex_type ())
	{
	  if (array.is_single_type ())
	    retval = do_partarray (array.float_complex_array_value (),
				   fname, args);
	  else
	    retval = do_partarray (array.complex_array_value (), fname, args);
	}
      else
	{
	  if (array.is_single_type ())
	    retval = do_partarray (array.float_array_value (), fname, args);
	  else
	    retval = do_partarray (array.array_value (), fname, args);
	}
    }
  // FIXME: This seems to work in cellfun.cc, but not here ...
  /*
  else if (array.is_map ())
    retval = do_partarray (array.map_value (), fname, args);
  */
  else if (array.is_cell ())
    retval = do_partarray (array.cell_value (), fname, args);
  else
    gripe_wrong_type_arg (fname, array);

  return octave_value (retval);
}

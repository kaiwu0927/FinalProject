## Copyright (C) 2008 Bill Denney
##
## This software is free software; you can redistribute it and/or modify it
## under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 3 of the License, or (at
## your option) any later version.
##
## This software is distributed in the hope that it will be useful, but
## WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this software; see the file COPYING.  If not, see
## <http://www.gnu.org/licenses/>.

## -*- texinfo -*-
## @deftypefn {Function File} {hhv} = hhigh (data)
## @deftypefnx {Function File} {hhv} = hhigh (data, nperiods)
## @deftypefnx {Function File} {hhv} = hhigh (data, nperiods, dim)
##
## Compute the highest high value of @var{data} for the past
## @var{nperiods} (default: 14) across the dimension, @var{dim}
## (default: 1).
##
## @seealso{llow}
## @end deftypefn

## Author: Bill Denney <bill@denney.ws>
## Created: 24 Feb 2008

function hhv = hhigh (data, nperiods, dim)

  if nargin < 1 || nargin > 3
    print_usage ();
  elseif ~ isvector (data)
    ## FIXME
    error ("cannot yet handle more than one dimensional data")
  endif
  if nargin < 2
    nperiods = 14;
  endif
  if nargin < 3
    ## Use the first non-singleton dimension
    dim = find (size (data) > 1, 1);
  endif

  if dim > ndims (data)
    error ("dim cannot be greater than the number of dimensions in data");
  endif

  sz = size (data);
  hhv = data;
  for i = 1:sz(dim)
    hhv(i) = max (data(max (i-nperiods+1, 1):i));
  endfor

endfunction

## Tests
%!shared c, h
%! c = [22.44 22.61 22.67 22.88 23.36 23.23 23.08 22.86 23.17 23.69 23.77 23.84 24.32 24.8 24.16 24.1 23.37 23.61 23.21 25];
%! h = [22.44 22.61 22.67 22.88 23.36 23.36 23.36 23.36 23.36 23.69 23.77 23.84 24.32 24.8 24.8 24.8 24.8 24.8 24.8 25];
%!assert(hhigh(c), h)
%!assert(hhigh(c'), h')

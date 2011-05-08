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
## @deftypefn {Function File} {indices =} datefind (subset, superset, tol)
##
## Find any instances of the @code{subset} in the @code{superset} with
## the @code{tol}erance.  @code{tol} is 0 by default.
##
## @seealso{date, datenum}
## @end deftypefn

## Author: Bill Denney <bill@denney.ws>
## Created: 21 Jan 2008

function idx = datefind (subset, superset, tol=0)

  if (nargin < 2 || nargin > 3)
    print_usage ();
  elseif ! isscalar (tol)
    error ("datefind: tol must be a scalar")
  endif

  idx = [];
  for i = 1:numel (superset)
    if any (subset(:) - tol <= superset(i) & superset(i) <= subset(:) + tol)
      idx(end+1, 1) = i;
    endif
  endfor

endfunction

## Tests
%!assert (datefind (datenum (1999, 7, [10;20]), datenum (1999, 7, 1:31)), [10;20])
%!assert (datefind (datenum (1999, 7, [10;20]), datenum (1999, 7, 1:31), 1), [9;10;11;19;20;21])

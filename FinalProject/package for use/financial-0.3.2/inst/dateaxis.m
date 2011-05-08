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
## @deftypefn {Function File} {} dateaxis ()
## @deftypefnx {Function File} {} dateaxis (ax)
## @deftypefnx {Function File} {} dateaxis (ax, dateform)
## @deftypefnx {Function File} {} dateaxis (ax, dateform, startdate)
## @deftypefnx {Function File} {} dateaxis (h, ...)
##
## Convert the current axis tick labels (or the axis handle @var{h}) to
## a date format.  The axis given by @var{ax} ("x", "y", or "z") will be
## changed; the default is "x".  The date format, @var{dateform}, used
## will be either auto-determined or an integer corresponding to the
## date formats in datestr. If @var{startdate} is given, then the first
## tick value on the given axis is assumed to be that date.
##
## @seealso{bolling, candle, highlow, movavg, pointfig}
## @end deftypefn

## Author: Bill Denney <bill@denney.ws>
## Created: 2 Feb 2008

function dateaxis (varargin)

  ## defaults
  h = gca ();
  ax = "x";
  dateform = [];
  startdate = [];

  ## check inputs
  if nargin > 0
    if ishandle(varargin{1})
      h = varargin{1};
      varargin(1) = [];
    endif
    if length(varargin) > 0
      ax = varargin{1};
    endif
    if length(varargin) > 1
      dateform = varargin{2};
    endif
    if length(varargin) > 2
      startdate = varargin{3};
      if ischar(startdate)
        startdate = datenum(startdate);
      elseif ~isnumeric(startdate)
        error ("startdate must be either a datenum or numeric")
      endif
    endif
    if length(varargin) > 3
      print_usage ();
    endif
  endif

  if isempty(dateform)
    r = range(get(h, [ax "lim"]));
    if r < 10/60/24
      ## minutes and seconds
      dateform = 13;
    elseif r < 2
      ## hours
      dateform = 15;
    elseif r < 15
      ## days
      dateform = 8;
    elseif r < 365
      ## months
      dateform = 6;
    elseif r < 90*12
      ## quarters
      dateform = 27;
    else
      ## years
      dateform = 10;
    endif
  endif

  ticks = get (h, [ax "tick"]);
  ticks = ticks - ticks(1) + startdate;
  ticks = datestr(ticks, dateform);
  ticks = mat2cell(ticks, ones(size(ticks,1),1), size(ticks,2));
  keyboard
  set (h, [ax "ticklabel"], ticks);

endfunction

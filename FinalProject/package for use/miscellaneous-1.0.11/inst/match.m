## Copyright (C) 2007 Muthiah Annamalai
##
## This program is free software; you can redistribute it and/or
## modify it under the terms of the GNU General Public
## License as published by the Free Software Foundation;
## either version 2, or (at your option) any later version.
##
## This software is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied
## warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
## PURPOSE.  See the GNU General Public License for more
## details.
##
## You should have received a copy of the GNU General Public License
## along with this program; if not, see <http://www.gnu.org/licenses/>.

## -*- texinfo -*-
## @deftypefn {Function File}  @var{result} = {} match ( @var{fun_handle}, @var{iterable} )
## match is filter, like Lisp's ( & numerous other language's ) function for
## Python has a built-in filter function which takes two arguments,
## a function and a list, and returns a list. 'match' performs the same
## operation like filter in Python. The match applies the
## function to each of the element in the @var{iterable} and collects
## that the result of a function applied to each of the data structure's
## elements in turn, and the return values are collected as a list of 
## input arguments, whenever the function-result is 'true' in Octave
## sense. Anything (1,true,?) evaluating to true, the argument is
## saved into the return value.
##
## @var{fun_handle} can either be a function name string or a
## function handle (recommended).
## 
## Typically you can use it as,
## @example
## match(@@(x) ( x >= 1 ), [-1 0 1 2])
## [1, 2]
## @end example
## @end deftypefn
 
## Last Modified by Muthiah Annamalai

function rval = match (fun_handle,data)
  
  if (nargin >= 1)

    try
      if ( ischar(fun_handle) )
	fun_handle=eval(strcat("@",fun_handle));
      end
      fstr=typeinfo(fun_handle);
    catch
      error('Error: Cannot find function handle, or funtion doesnt exist')
    end
  end

  if (nargin<2)
    error("match: incorrect number of arguments; expecting at least two.");
  end
  
  LD=length(data);
  rval=[];
  for idx=1:LD
    if fun_handle(data(idx)) %anything thats true
      rval=[rval, data(idx)];
    end
  end
  return
endfunction
%!
%!assert(match(@(x) mod(x,2),1:10),[1:2:10],0)
%!assert(match(@sin,1:10),[1:10],0)
%!assert(match(@(x) strcmp('Octave',x),{'Matlab','Octave'}),{'Octave'},0)
%!assert(match(@(x) (x>0), [-10:+10]),[1:10],0)
%!

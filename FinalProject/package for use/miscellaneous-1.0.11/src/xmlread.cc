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

/* 2004-01-30
 *   initial release
 */

#include <octave/oct.h>
#include <octave/lo-ieee.h>
#include <octave/oct-map.h>
#include <octave/Cell.h>

extern "C" {
#include "xmltree.h"
#include "xmltree_read.h"
}

octave_value get_element (element *root) {
  int length, rows, columns;

  string_vector tmp_array;
  ComplexMatrix tmp_matrix;
  Octave_map tmp_structure;
  Cell tmp_cell;
  octave_value_list tmp_list;

  octave_value retval;

  if (!root)
    return retval;

  switch (root->def_value) {
  case value_scalar:
    switch (root->const_value) {
    case const_true: retval = octave_value(true); break;
    case const_false: retval = octave_value(false); break;
    case const_inf: retval = octave_value(octave_Inf); break;
    case const_neginf: retval = octave_value(-octave_Inf); break;
    case const_nan:  retval = octave_value(octave_NaN); break;
    case const_na:  retval = octave_value(octave_NA); break;
    case const_undef: retval = octave_value(root->scalar_value); break;
    }
    break;

  case value_complex:
    retval = 
      octave_value(Complex(get_element(root->child).double_value(),
			   get_element(root->child->next).double_value()));
    break;

  case value_string:
    retval = octave_value(root->string_value);
    break;

  case value_array:
    rows = root->rows;
    root = root->child;
    tmp_array = string_vector(rows);
    for (int k=0; (k<rows) && (root); k++, root = root->next)
      tmp_array(k) = get_element(root).string_value();
    retval = octave_value(tmp_array);
    break;

  case value_matrix:
    rows = root->rows;
    columns = root->columns;
    root = root->child;
    tmp_matrix = ComplexMatrix(rows, columns);
    for (int k=0; (k<rows) && (root); k++)
      for (int l=0; (l<columns) && (root); l++, root = root->next)
	tmp_matrix(k, l) = get_element(root).complex_value();
    if (tmp_matrix.all_elements_are_real())
      retval = octave_value(real(tmp_matrix));
    else
      retval = octave_value(tmp_matrix);
    break;

  case value_structure:
    root = root->child;
    for (int k=0; root; root = root->next)
      if (root->name)
	tmp_structure.assign(root->name, get_element(root));
      else {
	char *name = new char[7];
	sprintf (name, "__%04d", k++);
	warning ("no field name in structure.");
	tmp_structure.assign(name, get_element(root));
	delete[] name;
      }
    retval = octave_value(tmp_structure);
    break;

  case value_list:
    length = root->length;
    root = root->child;
    //tmp_list = octave_value_list(length);
    for (int k=0; (k<length) && (root); k++, root = root->next)
      tmp_list(k) = get_element(root);
    retval = octave_value(tmp_list);
    break;
    
  case value_cell:
    rows = root->rows;
    columns = root->columns;
    root = root->child;
    tmp_cell = Cell(rows, columns);
    for (int k=0; (k<rows) && (root); k++)
      for (int l=0; (l<columns) && (root); l++, root = root->next)
	tmp_cell(k, l) = get_element(root);
    retval = octave_value(tmp_cell);
    break;

  default:
    warning ("unknown type.\n");
  }

  return retval;
}

DEFUN_DLD (xmlread, args, nargout,
	   "-*- texinfo -*-\n"
           "@deftypefn {Function File} {@var{value}} xmlread(@var{filename})\n"
	   "\n"
	   "Read a @var{value} from @var{filename} as an XML file\n"
	   "@end deftypefn") {

  /* Check argument */
  if (args.length() != 1) {
    print_usage ();
    return octave_value_list();
  }

  /* Read file */
  std::string filename = args(0).string_value();
  element *root = read_xmltree(filename.c_str());
  if (!root)
    return octave_value_list();
  
  /* step down into the element tree */
  octave_value retval = get_element (root->child);
  free_element (root);

  return retval;
}

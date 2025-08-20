#include "LABSoft_GUI_LABChecker_Digital_Input_Table.h"

#include <cmath>
#include <cstdio>
#include <string>
#include <iostream>

#include <FL/Enumerations.H>
#include <FL/fl_draw.H>

LABSoft_GUI_LABChecker_Digital_Input_Table:: 
LABSoft_GUI_LABChecker_Digital_Input_Table (int         X,
                                            int         Y,
                                            int         W,
                                            int         H,
                                            const char* label)
  : Fl_Table  (X, Y, W, H, label),
    m_inputs  (m_output_count, std::vector<char>(m_input_bits, '0')),
    m_outputs (m_output_count, std::vector<char>(m_output_bits, '0'))
{
  when                      (FL_WHEN_NOT_CHANGED | when ());
  callback                  (&event_callback, static_cast<void*>(this));  
  create_input_widget       (W, H);

  end ();

  init_table_dimensions     ();
  recalculate_rows_and_cols ();
  resize                    (m_number_of_rows, m_number_of_cols);
}

LABSoft_GUI_LABChecker_Digital_Input_Table:: 
~LABSoft_GUI_LABChecker_Digital_Input_Table ()
{

}

void LABSoft_GUI_LABChecker_Digital_Input_Table:: 
init_table_dimensions ()
{
  row_header        (1);
  row_header_width  (COL_WIDTH);
  row_resize        (0);
  row_height_all    (ROW_HEIGHT);

  col_header        (1);
  col_header_height (ROW_HEIGHT);
  col_resize        (0);
  col_width_all     (COL_WIDTH);

  set_selection     (0, 0, 0, 0);
  // tab_cell_nav      (1);
  

  tooltip ("Use keyboard to navigate cells:\nArrow keys or Tab/Shift-Tab");
}

void LABSoft_GUI_LABChecker_Digital_Input_Table:: 
create_input_widget (unsigned W,
                     unsigned H)
{
  m_input = new Fl_Input (W / 2, H / 2, 0, 0);

  m_input->hide         ();
  m_input->callback     (input_cb, static_cast<void*>(this));
  m_input->when         (FL_WHEN_ENTER_KEY_ALWAYS); // callback triggered when user hits Enter
  m_input->maximum_size (1);
  m_input->color        (COLOR_SELECTED_CELL);
}

void LABSoft_GUI_LABChecker_Digital_Input_Table:: 
event_callback2 ()
{
  int R = callback_row (); 
  int C = callback_col ();

  LABSoft_GUI_LABChecker_Digital_Input_Table::TableContext context = callback_context ();

  switch (context)
  {
    case (LABSoft_GUI_LABChecker_Digital_Input_Table::TableContext::CONTEXT_CELL):
    {
      switch (Fl::event ())
      {
        case (FL_PUSH):
        {
          done_editing  ();
          start_editing (R, C);

          break;
        }

        case (FL_KEYBOARD):
        {
          if (Fl::event_key () == FL_Escape)
          {
            // capture to avoid quitting the program
          }

          switch (Fl::e_text[0])
          {
            case ('0'):
            case ('1'):
            case ('X'):
            case ('x'):
            case ('~'):
            case ('3'):
            {
              start_editing   (R, C);
              m_input->handle (Fl::event ());

              break;
            }

            case ('\r'):
            case ('\n'):
            {
              start_editing (R, C);

              break;
            }

          }
        }
      }

      break;
    }

    case (LABSoft_GUI_LABChecker_Digital_Input_Table::TableContext::CONTEXT_TABLE):
    case (LABSoft_GUI_LABChecker_Digital_Input_Table::TableContext::CONTEXT_ROW_HEADER):
    case (LABSoft_GUI_LABChecker_Digital_Input_Table::TableContext::CONTEXT_COL_HEADER):
    {
      done_editing ();
      
      break;
    }

    default:
    {
      break;
    }
  }
}

void LABSoft_GUI_LABChecker_Digital_Input_Table:: 
input_cb2 ()
{
  set_value_hide ();

  if (Fl::event_key () == FL_Enter)
  {
    if (m_col_edit < (cols () - 1))
    {
      start_editing (m_row_edit, m_col_edit + 1);
    }
    else 
    {
      if (m_row_edit < (rows () - 1))
      {
        start_editing (m_row_edit + 1, 0);
      }
    }
  }
}

void LABSoft_GUI_LABChecker_Digital_Input_Table:: 
recalculate_rows_and_cols ()
{
  m_number_of_rows = m_output_count;
  m_number_of_cols = m_input_bits + m_output_bits;
}

void LABSoft_GUI_LABChecker_Digital_Input_Table:: 
recalculate_and_resize ()
{
  recalculate_rows_and_cols ();
  do_resize                 ();
}

char LABSoft_GUI_LABChecker_Digital_Input_Table:: 
get_vector_value (unsigned R, 
                  unsigned C) const
{
  if (C < m_input_bits)
  {
    return (m_inputs[R][(m_input_bits - 1) - C]);
  }
  else 
  {
    return (m_outputs[R][(m_number_of_cols - C) - 1]);
  } 
}

void LABSoft_GUI_LABChecker_Digital_Input_Table:: 
set_vector_value (char      c, 
                  unsigned  R, 
                  unsigned  C)
{
  if (C < m_input_bits)
  {
    m_inputs[R][(m_input_bits - 1) - C] = c;
  }
  else 
  {
    m_outputs[R][(m_number_of_cols - C) - 1] = c;
  } 
}

void LABSoft_GUI_LABChecker_Digital_Input_Table::
event_callback (Fl_Widget*  w, 
                void*       v)
{
  (static_cast<LABSoft_GUI_LABChecker_Digital_Input_Table*>(v))->event_callback2 ();
}

void LABSoft_GUI_LABChecker_Digital_Input_Table::
input_cb (Fl_Widget*  w,
          void*       v) 
{	
  (static_cast<LABSoft_GUI_LABChecker_Digital_Input_Table*>(v))->input_cb2 ();
}

void LABSoft_GUI_LABChecker_Digital_Input_Table:: 
do_resize ()
{
  // 1. resize 2D vector
  //m_values.resize (m_number_of_rows, std::vector<char>(m_number_of_cols));

  m_inputs.resize (m_number_of_rows);

  for (auto& row : m_inputs)
  {
    row.resize (m_input_bits, '0');
  }

  m_outputs.resize (m_number_of_rows);

  for (auto& row : m_outputs)
  {
    row.resize (m_output_bits, '0');
  }

  // 2. resize table 
  cols (m_number_of_cols);
  rows (m_number_of_rows);
}

void LABSoft_GUI_LABChecker_Digital_Input_Table:: 
set_value_hide ()
{
  std::string str (m_input->value ());
 
  if (str == "0" || str == "1" || str == "X" || str == "x" || str == "`" || str == "3")
  {
    if (str == "`")
    {
      str = "X";
    }

    if (str == "3")
    {
      str = "0";
    }

    if (str == "x")
    {
      str = "X";
    }

    set_vector_value (str[0], m_row_edit, m_col_edit);
  }

  m_input->hide ();

  // window()->cursor(FL_CURSOR_DEFAULT);		// XXX: if we don't do this, cursor can disappear!
}

// Start editing a new cell: move the Fl_Int_Input widget to specified row/column
// Preload the widget with the cell's current value,
// and make the widget 'appear' at the cell's location.
void LABSoft_GUI_LABChecker_Digital_Input_Table:: 
start_editing (int R, int C)
{
  m_row_edit = R;
  m_col_edit = C;

  set_selection (R, C, R, C);

  int X, Y, W, H;

  find_cell (CONTEXT_CELL, R, C, X, Y, W, H); // Find X/Y/W/H of cell

  m_input->resize (X, Y, W, H); // Move Fl_Input widget to position

  char s[30];

  std::snprintf (s, sizeof (s), "%c", get_vector_value (R, C));

  m_input->value      (s);
  m_input->position   (0, strlen (s));
  m_input->show       ();
  m_input->take_focus ();
}

// Tell the input widget it's done editing, and to hide
void LABSoft_GUI_LABChecker_Digital_Input_Table:: 
done_editing ()
{
  if (m_input->visible ())  // input widget visible, i.e. edit in progress?
  {
    set_value_hide ();      // transfer its current contents to cell and hide
  }
}

void LABSoft_GUI_LABChecker_Digital_Input_Table:: 
draw_cell (LABSoft_GUI_LABChecker_Digital_Input_Table::TableContext context,
           int R,
           int C,
           int X,
           int Y,
           int W,
           int H)
{
  static char s[30];

  switch (context)
  {
    // 1. STARTPAGE - when table, or parts of the table, are about to be redrawn
    case (LABSoft_GUI_LABChecker_Digital_Input_Table::TableContext::CONTEXT_STARTPAGE):
    {
      break;
    }

    // 2. ENDPAGE - when table has completed being redrawn
    case (LABSoft_GUI_LABChecker_Digital_Input_Table::TableContext::CONTEXT_ENDPAGE):
    {
      break;
    }

    // 3. ROW_HEADER - whenever a row header cell needs to be drawn
    case (LABSoft_GUI_LABChecker_Digital_Input_Table::TableContext::CONTEXT_ROW_HEADER):
    {
      fl_push_clip  (X, Y, W, H);
      {
        fl_draw_box   (FL_FLAT_BOX, X, Y, W, H, 53);
        fl_color      (FL_BLACK);
        fl_font       (fl_font () | FL_BOLD, fl_size ());

        std::sprintf  (s, "%d", R + 1);

        fl_draw       (s, X, Y, W, H, FL_ALIGN_CENTER);
      }
      fl_pop_clip   ();
      

      break;
    }

    // 4. COL_HEADER - whenever a column header cell needs to be drawn
    case (LABSoft_GUI_LABChecker_Digital_Input_Table::TableContext::CONTEXT_COL_HEADER):
    {
      fl_push_clip  (X, Y, W, H);
      {
        if (C < m_input_bits)
        {
          fl_draw_box   (FL_FLAT_BOX, X, Y, W, H, COLOR_HEADER_ROW_INPUT);
          std::sprintf  (s, "S%d", (m_input_bits - 1) - C);
        }
        else 
        {
          fl_draw_box   (FL_FLAT_BOX, X, Y, W, H, COLOR_HEADER_ROW_OUTPUT);
          fl_color      (FL_BLACK);
          std::sprintf  (s, "L%d", (m_number_of_cols - C) - 1);
        }

        fl_color  (FL_BLACK);
        fl_font   (FL_HELVETICA_BOLD, fl_size ());
        fl_draw   (s, X, Y, W, H, FL_ALIGN_CENTER);
      }
      fl_pop_clip   ();

      break;
    }

    // 5. CONTEXT_CELL - whenever a data cell in the table needs to be drawn
    case (LABSoft_GUI_LABChecker_Digital_Input_Table::TableContext::CONTEXT_CELL):
    {
      if (R == m_row_edit && C == m_col_edit && m_input->visible ())
      {
        return;
      }

      std::snprintf (s, sizeof (s), "%c", get_vector_value (R, C));

      fl_push_clip  (X, Y, W, H);
      {
        fl_font     (FL_HELVETICA, fl_size ());
        fl_draw_box (FL_THIN_DOWN_BOX, X, Y, W, H, is_selected (R, C) ? COLOR_SELECTED_CELL : FL_WHITE);
        fl_color    (FL_BLACK);   
        fl_draw     (s, X, Y, W, H, FL_ALIGN_INSIDE | FL_ALIGN_CENTER);
      }
      fl_pop_clip   ();

      break;
    }

    // 6. RC_RESIZE - whenever table or row/column is resized or scrolled,
    //                either interactively or via col_width () or row_height ()
    case (LABSoft_GUI_LABChecker_Digital_Input_Table::TableContext::CONTEXT_RC_RESIZE):
    {
      if (m_input->visible ())
      {
        find_cell       (LABSoft_GUI_LABChecker_Digital_Input_Table::TableContext::CONTEXT_TABLE,
                          m_row_edit, m_col_edit, X, Y, W, H);
                       
        m_input->resize (X, Y, W, H);
        init_sizes      ();
      }

      break;
    }

    default:
    {
      break;
    }
  }
}

void LABSoft_GUI_LABChecker_Digital_Input_Table:: 
resize (unsigned new_number_of_rows, 
        unsigned new_number_of_cols)
{
  m_number_of_rows = new_number_of_rows;
  m_number_of_cols = new_number_of_cols;

  do_resize ();
}

void LABSoft_GUI_LABChecker_Digital_Input_Table::
input_bits (unsigned value)
{
  m_input_bits  = value;
  m_max_output_count = std::pow (2, value);

  if (m_output_count > m_max_output_count)
  {
    output_count (m_max_output_count);
  }
  else 
  {
    recalculate_and_resize ();
  }
}

void LABSoft_GUI_LABChecker_Digital_Input_Table::
output_bits (unsigned value)
{
  m_output_bits = value;

  recalculate_and_resize ();
}

void LABSoft_GUI_LABChecker_Digital_Input_Table::
output_count (unsigned value)
{
  if (value >= 1)
  {
    m_output_count = (value > m_max_output_count) ? m_max_output_count : value;

    recalculate_and_resize ();
  }
}

unsigned LABSoft_GUI_LABChecker_Digital_Input_Table:: 
input_bits () const 
{
  return (m_input_bits);
}

unsigned LABSoft_GUI_LABChecker_Digital_Input_Table:: 
output_count () const
{
  return (m_output_count);
}

unsigned LABSoft_GUI_LABChecker_Digital_Input_Table:: 
max_output_count () const
{
  return (m_max_output_count);
}

std::vector<std::vector<char>>& LABSoft_GUI_LABChecker_Digital_Input_Table:: 
inputs ()
{
  return (m_inputs);
}

std::vector<std::vector<char>>& LABSoft_GUI_LABChecker_Digital_Input_Table:: 
outputs ()
{
  return (m_outputs);
}

// EOF 
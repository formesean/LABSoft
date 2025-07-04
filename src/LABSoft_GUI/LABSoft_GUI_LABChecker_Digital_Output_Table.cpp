#include "LABSoft_GUI_LABChecker_Digital_Output_Table.h"

#include <cmath>
#include <cstdio>
#include <string>
#include <iostream>

#include <FL/Enumerations.H>
#include <FL/fl_draw.H>

LABSoft_GUI_LABChecker_Digital_Output_Table:: 
LABSoft_GUI_LABChecker_Digital_Output_Table (int         X,
                                    int         Y,
                                    int         W,
                                    int         H,
                                    const char* label)
  : Fl_Table  (X, Y, W, H, label),
    m_inputs  (m_output_count, std::vector<char>(m_input_bits, '0')),
    m_outputs (m_output_count, std::vector<char>(m_output_bits, '0'))
{
  end ();

  init_table_dimensions ();
  reset                 ();
}

LABSoft_GUI_LABChecker_Digital_Output_Table:: 
~LABSoft_GUI_LABChecker_Digital_Output_Table ()
{

}

void LABSoft_GUI_LABChecker_Digital_Output_Table:: 
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
}

void LABSoft_GUI_LABChecker_Digital_Output_Table:: 
recalculate_rows_and_cols ()
{
  m_number_of_rows = m_output_count;
  m_number_of_cols = m_input_bits + (m_output_bits * 2);
}

void LABSoft_GUI_LABChecker_Digital_Output_Table:: 
recalculate_and_resize ()
{
  recalculate_rows_and_cols ();
  do_resize                 ();
}

char LABSoft_GUI_LABChecker_Digital_Output_Table:: 
get_vector_value (unsigned R, 
                  unsigned C) const
{
  if (C < m_input_bits)
  {
    return (m_inputs[R][(m_input_bits - 1) - C]);
  }
  else 
  {
    unsigned outputs_col = C - m_input_bits;

    if (outputs_col < m_output_bits)
    {
      return (m_outputs[R][(m_output_bits - 1) - outputs_col]);
    }
    else 
    {
      return (m_actual_outputs[R][((m_output_bits - 1) - outputs_col) + m_output_bits]);
    }
  }
}

void LABSoft_GUI_LABChecker_Digital_Output_Table:: 
clear_vectors ()
{
  m_input_vals          .clear ();
  m_output_vals         .clear ();
  m_actual_output_vals  .clear ();
  m_inputs              .clear ();
  m_outputs             .clear ();
  m_actual_outputs      .clear ();
}

void LABSoft_GUI_LABChecker_Digital_Output_Table:: 
reset_vars ()
{
  m_number_of_rows  = 0;
  m_number_of_cols  = 0;
  m_input_bits      = 0;
  m_output_bits     = 0;
  m_output_count    = 0;
}

void LABSoft_GUI_LABChecker_Digital_Output_Table:: 
do_resize ()
{ 
  cols (m_number_of_cols);
  rows (m_number_of_rows);
}

void LABSoft_GUI_LABChecker_Digital_Output_Table:: 
draw_cell (LABSoft_GUI_LABChecker_Digital_Output_Table::TableContext context,
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
    case (LABSoft_GUI_LABChecker_Digital_Output_Table::TableContext::CONTEXT_STARTPAGE):
    {
      break;
    }

    // 2. ENDPAGE - when table has completed being redrawn
    case (LABSoft_GUI_LABChecker_Digital_Output_Table::TableContext::CONTEXT_ENDPAGE):
    {
      break;
    }

    // 3. ROW_HEADER - whenever a row header cell needs to be drawn
    case (LABSoft_GUI_LABChecker_Digital_Output_Table::TableContext::CONTEXT_ROW_HEADER):
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
    case (LABSoft_GUI_LABChecker_Digital_Output_Table::TableContext::CONTEXT_COL_HEADER):
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
          unsigned output_col = C - m_input_bits;

          if (output_col < m_output_bits)
          {
            fl_draw_box   (FL_FLAT_BOX, X, Y, W, H, COLOR_HEADER_ROW_OUTPUT);
            fl_color      (FL_BLACK);
            std::sprintf  (s, "L%d", (m_output_bits - 1) - output_col);
          }
          else 
          {
            fl_draw_box   (FL_FLAT_BOX, X, Y, W, H, COLOR_HEADER_ROW_ACTUAL_OUTPUT);
            fl_color      (FL_BLACK);
            std::sprintf  (s, "A%d", ((m_output_bits - 1) - output_col) + m_output_bits);
          } 
        }

        fl_color  (FL_BLACK);
        fl_font   (FL_HELVETICA_BOLD, fl_size ());
        fl_draw   (s, X, Y, W, H, FL_ALIGN_CENTER);
      }
      fl_pop_clip   ();

      break;
    }

    // 5. CONTEXT_CELL - whenever a data cell in the table needs to be drawn
    case (LABSoft_GUI_LABChecker_Digital_Output_Table::TableContext::CONTEXT_CELL):
    {
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
    case (LABSoft_GUI_LABChecker_Digital_Output_Table::TableContext::CONTEXT_RC_RESIZE):
    {
      init_sizes ();
      
      break;
    }

    default:
    {
      break;
    }
  }
}

void LABSoft_GUI_LABChecker_Digital_Output_Table:: 
resize (unsigned new_number_of_rows, 
        unsigned new_number_of_cols)
{
  m_number_of_rows = new_number_of_rows;
  m_number_of_cols = new_number_of_cols;

  do_resize ();
}

void LABSoft_GUI_LABChecker_Digital_Output_Table:: 
display_results (LAB_Digital_Circuit_Checker::ScoreData score_data)
{
  load_data (score_data);

  recalculate_and_resize ();
}

void LABSoft_GUI_LABChecker_Digital_Output_Table:: 
load_data (LAB_Digital_Circuit_Checker::ScoreData score_data)
{
  m_input_bits          = score_data.input_bits;
  m_output_bits         = score_data.output_bits;
  m_output_count        = score_data.char_inputs.size ();

  m_input_vals          = score_data.inputs;
  m_output_vals         = score_data.outputs;
  m_actual_output_vals  = score_data.actual_outputs;

  m_inputs              = score_data.char_inputs;
  m_outputs             = score_data.char_outputs;
  m_actual_outputs      = score_data.char_actual_outputs;
}

void LABSoft_GUI_LABChecker_Digital_Output_Table:: 
reset ()
{
  clear_vectors           ();
  reset_vars              ();
  recalculate_and_resize  ();
}

// EOF 
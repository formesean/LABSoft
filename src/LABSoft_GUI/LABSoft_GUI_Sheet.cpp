#include "LABSoft_GUI_Sheet.h"

#include <stdexcept>

// delete soon
#include <iostream>

#include <FL/fl_draw.H>
#include <FL/Enumerations.H>

LABSoft_GUI_Sheet:: 
LABSoft_GUI_Sheet (int X, 
                         int Y, 
                         int W, 
                         int H, 
                         const char* label)
  : Fl_Table (X, Y, W, H, label),
    m_data (ROWS, std::vector<Cell_Data> (COLS))
{
  create_input_widget   (W, H);
  end                   ();

  when                  (FL_WHEN_NOT_CHANGED | when ());
  callback              (&event_cb_static, static_cast<void*>(this));
  init_table_dimensions ();
  set_selection         (0, 0, 0, 0);

  // tab_cell_nav  (1);
}

LABSoft_GUI_Sheet:: 
~LABSoft_GUI_Sheet ()
{

}

void LABSoft_GUI_Sheet::
event_cb_static (Fl_Widget* w, 
                 void*      data)
{
  (static_cast<LABSoft_GUI_Sheet*>(w))->event_cb ();
}

void LABSoft_GUI_Sheet::
event_cb ()
{
  int R = callback_row ();
  int C = callback_col ();

  LABSoft_GUI_Sheet::TableContext context = callback_context ();

  switch (context)
  {
    case (LABSoft_GUI_Sheet::TableContext::CONTEXT_CELL):
    {     
      done_editing_cell   ();
      start_editing_cell  (R, C);

      break;
    }

    case (LABSoft_GUI_Sheet::TableContext::CONTEXT_TABLE):
    case (LABSoft_GUI_Sheet::TableContext::CONTEXT_ROW_HEADER):
    case (LABSoft_GUI_Sheet::TableContext::CONTEXT_COL_HEADER):
    {
      done_editing_cell ();

      break;
    }

    default:
    {
      break;
    }
  }
}

void LABSoft_GUI_Sheet:: 
input_cb_static (Fl_Widget* w,
                 void*      data)
{
  (static_cast<LABSoft_GUI_Sheet*>(w))->input_cb (data);
}

void LABSoft_GUI_Sheet:: 
input_cb (void* data)
{
  LABSoft_GUI_Sheet& sheet = *(static_cast<LABSoft_GUI_Sheet*>(data));

  sheet.set_value_hide ();

  if (Fl::event_key () == FL_Enter)
  {
    if (sheet.m_row_edit < (sheet.rows () - 1))
    {
      sheet.start_editing_cell (sheet.m_row_edit + 1, sheet.m_col_edit);
    }
    else 
    {
      sheet.start_editing_cell (sheet.m_row_edit, sheet.m_col_edit);
    }
  }
}

void LABSoft_GUI_Sheet:: 
create_input_widget (unsigned W,
                     unsigned H)
{
  m_input = new Fl_Input (W / 2, H / 2, 0, 0);

  m_input->hide     ();
  m_input->callback (input_cb_static, static_cast<void*>(this));
  // m_input->when     (FL_WHEN_CHANGED | FL_WHEN_ENTER_KEY_ALWAYS); // callback triggered when user hits Enter
  m_input->when     (FL_WHEN_ENTER_KEY_ALWAYS);
  m_input->color    (m_selected_cell_bg_color);
}

void LABSoft_GUI_Sheet::
init_table_dimensions ()
{
  row_header        (0);
  row_header_width  (COL_WIDTH);
  row_resize        (0);
  row_height_all    (ROW_HEIGHT);

  col_header        (0);
  col_header_height (ROW_HEIGHT);
  col_resize        (0);
  col_width_all     (COL_WIDTH);

  rows              (ROWS);
  cols              (COLS);
}

void LABSoft_GUI_Sheet:: 
draw_cell (LABSoft_GUI_Sheet::TableContext context,
           int R,
           int C,
           int X,
           int Y,
           int W,
           int H)
{
  // Full information:
  // https://www.fltk.org/doc-1.3/classFl__Table.html#a3f1f1ccc017d9159c1c98f6f7268bf6d

  switch (context)
  {
    // 1. CONTEXT_STARTPAGE - when table, or parts of table, are about to be redrawn
    //                      - used to initialize static data, such as font selections
    case (LABSoft_GUI_Sheet::TableContext::CONTEXT_STARTPAGE):
    {
      break;
    }

    // 2. CONTEXT_ENDPAGE - when table has completed being redrawn
    case (LABSoft_GUI_Sheet::TableContext::CONTEXT_ENDPAGE):
    {
      break;
    }

    // 3. CONTEXT_ROW_HEADER  - whenever a row header cell needs to be drawn
    case (LABSoft_GUI_Sheet::TableContext::CONTEXT_ROW_HEADER):
    {
      draw_cell_contents (R + 1, 0, X, Y, W, H);

      break;
    }

    // 4. CONTEXT_COL_HEADER  - whenever a column header cell needs to be drawn
    case (LABSoft_GUI_Sheet::TableContext::CONTEXT_COL_HEADER):
    {
      draw_cell_contents (0, C + 1, X, Y, W, H);

      break;
    }

    // 5. CONTEXT_CELL  - whenever a data cell in the table needs to be drawn
    case (LABSoft_GUI_Sheet::TableContext::CONTEXT_CELL):
    {
      if (R == m_row_edit && C == m_col_edit && m_input->visible ())
      {
        return;
      }
      else 
      {
        draw_cell_contents (R + 1, C + 1, X, Y, W, H);
      }

      break;
    }

    // 6. CONTEXT_RC_RESIZE - whenever table or row/column is resized or scrolled
    //                        either interactively or via col_width () or row_height ()
    case (LABSoft_GUI_Sheet::TableContext::CONTEXT_RC_RESIZE):
    {
      if (m_input->visible ())
      {
        find_cell (LABSoft_GUI_Sheet::TableContext::CONTEXT_TABLE,
          m_row_edit, m_col_edit, X, Y, W, H);

        m_input->resize (X, Y, W, H);

        init_sizes ();
      }

      break;
    }

    default:
    {
      break;
    }
  }
}

void LABSoft_GUI_Sheet:: 
draw_cell_contents (int R, 
                    int C, 
                    int X, 
                    int Y, 
                    int W, 
                    int H)
{
  Cell_Data ci = m_data[R][C];

  fl_push_clip (X, Y, W, H);

  fl_draw_box (FL_BORDER_BOX, X, Y, W, H, ci.bg_color);
  fl_color    (ci.font_color);
  fl_font     (ci.font, ci.font_size);

  fl_draw (
    ci.text.c_str (),
    X + (m_cell_padding_left),
    Y + (m_cell_padding_top),
    W - (m_cell_padding_left + m_cell_padding_right),
    H - (m_cell_padding_top + m_cell_padding_bottom),
    ci.font_align
  );

  fl_pop_clip ();
}

// Start editing a new cell: 
// move the Fl_Input widget to specific row & column.
// Preload the widget with the cell's current value,
// and make the widget "appear" at the cell's location.
void LABSoft_GUI_Sheet:: 
start_editing_cell (unsigned R, 
                    unsigned C)
{
  m_row_edit = R;
  m_col_edit = C;

  set_selection (R, C, R, C);

  int X, Y, W, H;

  find_cell (CONTEXT_CELL, R, C, X, Y, W, H); // find X/Y/W/H of cell

  LABSoft_GUI_Sheet::Cell_Data& data = m_data[R + 1][C + 1];

  m_input->resize     (X, Y, W, H);
  m_input->value      (data.text.c_str ());
  m_input->position   (0, data.text.size ());
  m_input->textfont   (data.font);
  m_input->textsize   (data.font_size);
  m_input->show       ();
  m_input->take_focus ();
}

void LABSoft_GUI_Sheet:: 
done_editing_cell ()
{
  if (m_input->visible ())  // input widget visible, i.e., edit in progress?
  { 
    set_value_hide ();      // transfer its current contents to cell and hide
  }
}

void LABSoft_GUI_Sheet:: 
set_value_hide ()
{
  m_data[m_row_edit + 1][m_col_edit + 1].text = std::string (m_input->value ());

  m_input->hide ();
}

void LABSoft_GUI_Sheet:: 
dimensions (unsigned rows,
            unsigned cols)
{
  m_data.resize (rows + 1);

  for (auto& row : m_data)
  {
    row.resize (cols + 1);
  }

  this->rows (rows);
  this->cols (cols);
}

LABSoft_GUI_Sheet::Range LABSoft_GUI_Sheet:: 
range (unsigned row,
       unsigned col)
{
  LABSoft_GUI_Sheet::Range range (*this, row, col);

  return (range);
}

const std::vector<std::vector<LABSoft_GUI_Sheet::Cell_Data>>& LABSoft_GUI_Sheet:: 
data () const 
{
  return (m_data);
}

// Range

LABSoft_GUI_Sheet::Range::
Range (LABSoft_GUI_Sheet& sheet,
       unsigned           row,
       unsigned           col)
  : m_sheet     (sheet),
    m_start_row (row),
    m_start_col (col),
    m_width     (1),
    m_height    (1)
{

}

void LABSoft_GUI_Sheet::Range::
value (std::string str)
{
  if (m_width == 1 && m_height == 1)
  {
    m_sheet.m_data[m_start_row][m_start_col].text = str;

    m_sheet.redraw ();
  }
  else 
  {
    throw (std::runtime_error ("Invalid input value size."));
  }
}

std::string LABSoft_GUI_Sheet::Range:: 
value () const
{
  if (m_width == 1 && m_height == 1)
  {
    return (m_sheet.m_data[m_start_row][m_start_col].text);
  }
  else 
  {
    throw (std::runtime_error ("Invalid call on value (), range contains multiple cells."));
  }
}
#include "LABSoft_GUI_Exporter_Data_Table.h"

#include <FL/Enumerations.H>
#include <FL/fl_draw.H>

LABSoft_GUI_Exporter_Data_Table:: 
LABSoft_GUI_Exporter_Data_Table (int         X,
                                 int         Y,
                                 int         W,
                                 int         H,
                                 const char* label)
  : Fl_Table  (X, Y, W, H, label)
{
  end ();
}

LABSoft_GUI_Exporter_Data_Table:: 
~LABSoft_GUI_Exporter_Data_Table ()
{

}

void LABSoft_GUI_Exporter_Data_Table:: 
draw_cell (LABSoft_GUI_Exporter_Data_Table::TableContext context,
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
    case (LABSoft_GUI_Exporter_Data_Table::TableContext::CONTEXT_STARTPAGE):
    {
      break;
    }

    // 2. ENDPAGE - when table has completed being redrawn
    case (LABSoft_GUI_Exporter_Data_Table::TableContext::CONTEXT_ENDPAGE):
    {
      break;
    }

    // 3. ROW_HEADER - whenever a row header cell needs to be drawn
    case (LABSoft_GUI_Exporter_Data_Table::TableContext::CONTEXT_ROW_HEADER):
    {
      break;
    }

    // 4. COL_HEADER - whenever a column header cell needs to be drawn
    case (LABSoft_GUI_Exporter_Data_Table::TableContext::CONTEXT_COL_HEADER):
    {
      break;
    }

    // 5. CONTEXT_CELL - whenever a data cell in the table needs to be drawn
    case (LABSoft_GUI_Exporter_Data_Table::TableContext::CONTEXT_CELL):
    {
       break;
    }

    // 6. RC_RESIZE - whenever table or row/column is resized or scrolled,
    //                either interactively or via col_width () or row_height ()
    case (LABSoft_GUI_Exporter_Data_Table::TableContext::CONTEXT_RC_RESIZE):
    {
      break;
    }

    default:
    {
      break;
    }
  }
}



// EOF 
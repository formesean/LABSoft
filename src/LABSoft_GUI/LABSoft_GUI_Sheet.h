#ifndef LABSOFT_GUI_SHEET_H
#define LABSOFT_GUI_SHEET_H

#include <string>
#include <vector>

#include <FL/Fl_Input.H>
#include <FL/Fl_Table.H>

class LABSoft_GUI_Sheet : public Fl_Table
{
  private:
    class Range
    {
      private:
        LABSoft_GUI_Sheet& m_sheet;

        unsigned m_start_row  = 0;
        unsigned m_start_col  = 0;
        unsigned m_width      = 0;
        unsigned m_height     = 0;

      public:
        Range (LABSoft_GUI_Sheet& sheet,
               unsigned           row,
               unsigned           col);

        void        value (std::string str);
        std::string value () const;
    };

    struct Cell_Data
    {
      std::string text        = "";
      Fl_Font     font        = FL_HELVETICA;
      Fl_Fontsize font_size   = 12;
      Fl_Color    font_color  = FL_BLACK;
      Fl_Align    font_align  = FL_ALIGN_INSIDE | FL_ALIGN_LEFT;
      Fl_Color    bg_color    = FL_WHITE;
    };

  private:
    const unsigned  ROWS       = 1;
    const unsigned  COLS       = 1;
    const unsigned  ROW_HEIGHT = 30;
    const unsigned  COL_WIDTH  = 30;

  private:
    int       m_row_edit                = 0;
    int       m_col_edit                = 0;
    int       m_cell_padding_top        = 5;
    int       m_cell_padding_bottom     = 5;
    int       m_cell_padding_left       = 5;
    int       m_cell_padding_right      = 5;
    Fl_Color  m_selected_cell_bg_color  = 247;

    Fl_Input*                           m_input;
    std::vector<std::vector<Cell_Data>> m_data;

  private:
    void  draw_cell (TableContext context, 
                     int R = 0, 
                     int C = 0, 
                     int X = 0, 
                     int Y = 0, 
                     int W = 0, 
                     int H = 0);
    
    void create_input_widget    (unsigned W, unsigned H);
    void init_table_dimensions  ();
    void draw_cell_contents     (int R, int C, int X, int Y, int W, int H);
    void event_cb               ();
    void input_cb               (void* data);
    void start_editing_cell     (unsigned R, unsigned C);
    void done_editing_cell      ();
    void set_value_hide         ();

    static void event_cb_static (Fl_Widget* w, void* data);
    static void input_cb_static (Fl_Widget* w, void* data);

  public:
    LABSoft_GUI_Sheet (int X, int Y, int W, int H, const char* label = 0);
   ~LABSoft_GUI_Sheet ();

    void  dimensions  (unsigned rows, unsigned cols);
    Range range       (unsigned row, unsigned col);

    // delete soon
    const std::vector<std::vector<Cell_Data>>& data () const;
};

#endif
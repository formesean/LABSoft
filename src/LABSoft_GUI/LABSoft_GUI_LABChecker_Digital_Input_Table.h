#ifndef LABSOFT_GUI_LABCHECKER_DIGITAL_INPUT_TABLE
#define LABSOFT_GUI_LABCHECKER_DIGITAL_INPUT_TABLE

#include <vector>

#include <FL/Fl_Input.H>
#include <FL/Fl_Table.H>

class LABSoft_GUI_LABChecker_Digital_Input_Table : public Fl_Table
{
  private:
    int       COLOR_HEADER_ROW_INPUT  = 95;
    int       COLOR_HEADER_ROW_OUTPUT = 79;
    int       COLOR_SELECTED_CELL     = 247;
    unsigned  ROW_HEIGHT              = 25;
    unsigned  COL_WIDTH               = 30;

  private:
    Fl_Input* m_input;
    unsigned  m_number_of_rows    = 1;
    unsigned  m_number_of_cols    = 2;
    unsigned  m_row_edit          = 0;
    unsigned  m_col_edit          = 0;
    unsigned  m_input_bits        = 1;
    unsigned  m_output_bits       = 1;
    unsigned  m_output_count      = 2;
    unsigned  m_max_output_count  = 2;

    std::vector<std::vector<char>> m_inputs;
    std::vector<std::vector<char>> m_outputs;

  private:
    void  draw_cell                 (TableContext context, 
                                      int R = 0, 
                                      int C = 0, 
                                      int X = 0, 
                                      int Y = 0, 
                                      int W = 0, 
                                      int H = 0);

    void  init_table_dimensions     ();
    void  do_resize                 ();
    void  set_value_hide            ();
    void  start_editing             (int R, int C);
    void  done_editing              ();
    void  create_input_widget       (unsigned W, unsigned H);
    void  event_callback2           ();
    void  input_cb2                 ();
    void  recalculate_rows_and_cols ();
    void  recalculate_and_resize    ();
    char  get_vector_value          (unsigned R, unsigned C) const;
    void  set_vector_value          (char c, unsigned R, unsigned C);

    static void event_callback  (Fl_Widget* w, void *v);
    static void input_cb        (Fl_Widget*,void* v);

  public:
    LABSoft_GUI_LABChecker_Digital_Input_Table (int X, int Y, int W, int H, const char* label = 0);
   ~LABSoft_GUI_LABChecker_Digital_Input_Table ();

    // Setter
    void resize       (unsigned new_number_of_rows, unsigned new_number_of_cols);
    void input_bits   (unsigned value);
    void output_bits  (unsigned value);
    void output_count (unsigned value);

    // Getter
    unsigned                        input_bits        () const;
    unsigned                        output_count      () const;
    unsigned                        max_output_count  () const;
    std::vector<std::vector<char>>& inputs            ();
    std::vector<std::vector<char>>& outputs           ();
};

#endif
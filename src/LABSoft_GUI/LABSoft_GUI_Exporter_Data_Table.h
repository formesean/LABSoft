#ifndef LABSOFT_GUI_EXPORTER_DATA_TABLE
#define LABSOFT_GUI_EXPORTER_DATA_TABLE

#include <vector>

#include <FL/Fl_Table.H>

class LABSoft_GUI_Exporter_Data_Table : public Fl_Table
{
  private:
    std::vector<std::vector<char>> m_outputs;

  private:
    void  draw_cell                 (TableContext context, 
                                      int R = 0, 
                                      int C = 0, 
                                      int X = 0, 
                                      int Y = 0, 
                                      int W = 0, 
                                      int H = 0);

  public:
    LABSoft_GUI_Exporter_Data_Table (int X, int Y, int W, int H, const char* label = 0);
   ~LABSoft_GUI_Exporter_Data_Table ();
};

#endif
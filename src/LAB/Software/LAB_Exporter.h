#ifndef LAB_EXPORTER_H
#define LAB_EXPORTER_H

#include <string>
#include <vector>

#include "../LAB_Module.h"
#include "../../Utility/LAB_Enumerations.h"

class LAB_Exporter : public LAB_Module
{
  private:
    LABE::LAB::INSTRUMENT                 m_instrument;
    std::vector<std::vector<std::string>> m_data_table;

    // state
    struct state
    {
      bool comments = false;
      bool headers  = false;
      bool labels   = false;
    } m_state;

  private:

  public:
    LAB_Exporter                  (LAB& _LAB);

    void export_data_to_csv_file  (const std::string& save_path);
    void update_data_table        (); 
    void instrument               (LABE::LAB::INSTRUMENT instrument);

    // state
    void comments (bool state);
    void headers  (bool state);
    void labels   (bool state);
    bool comments () const;
};

#endif
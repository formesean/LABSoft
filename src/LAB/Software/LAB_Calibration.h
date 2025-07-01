#ifndef LAB_CALIBRATION_H
#define LAB_CALIBRATION_H

#include <string>
#include <unordered_map>

#include "../LAB_Module.h"

using LAB_Calibration_Data = std::unordered_map<std::string, double>;

class LAB_Calibration : public LAB_Module
{
  private:
    LAB_Calibration_Data m_factory_calibration_data;
    LAB_Calibration_Data m_calibration_data_incoming;

  private:
    void load_factory_calibration_file          (const std::string& file_path);
    void update_actual_calibration_data_map     ();
    void apply_calibration_data_to_instruments  (const LAB_Calibration_Data& data);
    
    LAB_Calibration_Data parse_calibration_file   (const std::string& file_path);
  
  public:
    LAB_Calibration (LAB& _LAB);

    void load_calibration_file            (const std::string& file_path);
    void load_calibration_data            (const LAB_Calibration_Data& data);
    void load_factory_calibration_data    ();
    void apply_calibration_data           ();
    void discard_calibration_data_changes ();
    void save_calibration_data_to_file    (const std::string& file_path);
    void save_calibration_data_to_default ();

    const LAB_Calibration_Data  current_calibration_data  () const;
    const LAB_Calibration_Data& calibration_data_incoming () const;
};

#endif
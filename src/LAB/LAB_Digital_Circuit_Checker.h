#ifndef LAB_DIGITAL_CIRCUIT_CHECKER_H
#define LAB_DIGITAL_CIRCUIT_CHECKER_H

#include <string>
#include <vector>
#include <cstdint>

#include "LAB_Module.h"
#include "../Utility/pugixml.hpp"
#include "../LAB_Libraries/LAB_MCP23S17.h"

class LAB_Digital_Circuit_Checker : public LAB_Module
{
  public:
    struct ScoreData
    {
      unsigned  input_bits    = 0;
      unsigned  output_bits   = 0;

      unsigned  score_current = 0;
      unsigned  score_total   = 0;
      double    score_percent = 0.0;

      const std::vector<uint8_t>& inputs;
      const std::vector<uint8_t>& outputs;
      const std::vector<uint8_t>& actual_outputs;

      const std::vector<std::vector<char>>& char_inputs;
      const std::vector<std::vector<char>>& char_outputs;
      const std::vector<std::vector<char>>& char_actual_outputs;
    };

  private:
    LAB_MCP23S17                    m_hw_expander;
    bool                            m_is_running = false;

    // file
    bool                            m_is_file_loaded  = false;
    std::string                     m_file_path;
    pugi::xml_document              m_xml_doc;

    // data
    unsigned                        m_input_bits;
    unsigned                        m_output_bits;
    std::vector<uint8_t>            m_inputs;
    std::vector<uint8_t>            m_outputs;
    std::vector<uint8_t>            m_actual_outputs;
    std::vector<std::vector<char>>  m_char_inputs;
    std::vector<std::vector<char>>  m_char_outputs;
    std::vector<std::vector<char>>  m_char_actual_outputs;

    // score
    bool                            m_are_results_ready = false;
    unsigned                        m_score_current     = 0;
    unsigned                        m_score_total       = 0;
    double                          m_score_percent     = 0.0;

  private:
    void        init_gpio_pins                    ();
    void        init_hw_expander                  (); 
    bool        acquire_file_lock                 (const std::string& path);
    bool        release_file_lock                 (const std::string& path);
    void        clear_vectors                     ();
    void        load_metadata                     ();
    void        load_data_pairs                   ();
    void        load_data_from_file               ();
    void        perform_check                     ();
    void        calculate_scores                  ();
    void        generate_char_actual_outputs_vec  ();
    void        buffer_enable_output              (bool state);

  public:
    LAB_Digital_Circuit_Checker (LAB& _LAB);
   ~LAB_Digital_Circuit_Checker ();

    void      load_file    (const std::string& path);
    void      unload_file  ();
    void      run_checker  ();

    // Getter
    ScoreData get_score_data    () const;
    bool      are_results_ready () const;
    bool      is_running        () const;
};

#endif 

// EOF 
#include "LAB_Exporter.h"

#include <fstream>
#include <sstream>

// delete soon
#include <iostream>

#include "../LAB.h"

LAB_Exporter:: 
LAB_Exporter (LAB& _LAB)
  : LAB_Module (_LAB)
{
  
}

void LAB_Exporter:: 
export_data_to_csv_file (const std::string& save_path)
{
  std::ofstream file (save_path);

  if (!file.is_open ())
  {
    throw (std::runtime_error (
      "LAB_Exporter::export_data_to_csv_file (): Failed to create CSV file."
    ));

    return;
  }

  for (const std::vector<std::string>& row : m_data_table) 
  {
    for (size_t i = 0; i < row.size(); ++i) 
    {
      file << row[i];

      if (i < row.size () - 1) 
      {
        file << ",";
      }
    }

    file << "\n";
  }

  file.close ();
}

void LAB_Exporter:: 
update_data_table ()
{
  int row_count = m_state.comments ? 2 : 0;
  int col_count = 0;
    
  switch (m_instrument)
  {
    case (LABE::LAB::INSTRUMENT::OSCILLOSCOPE):
    {     
      row_count += lab ().m_Oscilloscope.samples ();
      col_count += 1 + lab ().m_Oscilloscope.channels ();

      // 1. resize
      m_data_table.resize (row_count, std::vector<std::string> (col_count, ""));

      //2. build from state
      if (m_state.comments)
      {
        m_data_table[0][0] = "Lab in a Box v1.0";
      }

      // 3. add data
      int row_start = m_state.comments ? 2 : 0;

      for (int row = 0; row < lab ().m_Oscilloscope.samples (); row++)
      {
        m_data_table[row + row_start][0] = std::to_string (row + 1);

        for (int chan = 0; chan < lab ().m_Oscilloscope.channels (); chan++)
        {
          m_data_table[row + row_start][chan + 1] = std::to_string (
            lab ().m_Oscilloscope.chan_samples (chan)[row]
          );
        }
      }
      
      break;
    }

    case (LABE::LAB::INSTRUMENT::VOLTMETER):
    {
      break;
    }

    case (LABE::LAB::INSTRUMENT::LOGIC_ANALYZER):
    {
      break;
    }
  }
}

void LAB_Exporter:: 
instrument (LABE::LAB::INSTRUMENT instrument)
{
  m_instrument = instrument;
}

void LAB_Exporter:: 
comments (bool state)
{
  m_state.comments = state;
}

void LAB_Exporter:: 
headers (bool state)
{
  m_state.headers = state;
}

void LAB_Exporter:: 
labels (bool state)
{
  m_state.labels = state;
}

bool LAB_Exporter:: 
comments () const
{
  return (m_state.comments);
}
#ifndef LAB_LABCHECKER_ANALOG_H
#define LAB_LABCHECKER_ANALOG_H

#include <array>
#include <vector>

#include "../LAB_Module.h"
#include "../../Utility/LAB_Constants.h"

//bea: try to delete this.
/*struct OsciDAta;
struct CircuitCheckerData;

class LAB_LABChecker_Analog : public Fl_Widget{
  private: 

  OsciData m_Osci_Data;
  bool m_osci_data_available;

  //std::vector<??> m_circuit_element;
  DisplayMode m_current_mode;

public: 

  LAB_LABChecker_Analog(int X, int Y, int W, int H, const char* label = 0)
  virtual ~LAB_LABChecker_Analog();

  void update_osci_data(const CircuitCheckerData&  data);
  void render_osci_overlay();

    //void add_circuit_element(const CircuitElement& element);
    //void clear_circuit_elements();

  void set_display_mode(DisplayMode mode);

  void draw()override;
  int handle(int event) override;

  private: 

    void draw_circuit_elements();
    void draw_grid();
    void draw_labels();
}; */


class LAB;
class LAB_Oscilloscope;

class LAB_LABChecker_Analog : public LAB_Module
{
  private:
    using PixelPoints = std::array<std::vector<std::array<int, 2>>, 
                      LABC::OSC_DISPLAY::NUMBER_OF_CHANNELS>;

    using ChanDoubles = std::array<double, LABC::OSC::NUMBER_OF_CHANNELS>;

    enum class DRAW_MODE
    {
      STRETCH,
      SHRINK,
      FIT
    };

  private:
    LAB_Oscilloscope& m_osc;

    // cached display values
    unsigned    m_height                          = 0;
    unsigned    m_width                           = 0; 
    unsigned    m_rows                            = 0;
    unsigned    m_columns                         = 0;
    double      m_column_width                    = 0.0;
    double      m_display_height_midline          = 0.0;  

    //
    double      m_time_per_division_delta_scaler  = 0.0;
    DRAW_MODE   m_draw_mode                       = DRAW_MODE::FIT;
    unsigned    m_samples_to_display              = 0;
    unsigned    m_graphing_area_width             = 0.0;
    bool        m_mark_samples                    = false;

    // 
    double      m_x_coord_scaling                 = 0;
    int         m_x_coord_start_offset            = 0;
    int         m_horizontal_offset_start_offset  = 0;
    int         m_mid_sample_to_center_offset     = 0;

    ChanDoubles m_sample_y_scaler = {0.0};
    ChanDoubles m_vertical_offset = {0.0};
    
    // data
    PixelPoints m_pixel_points;
  
  private: 
    double      calc_time_per_division_delta_scaler ()                                                          const;
    DRAW_MODE   calc_draw_mode                      (double tpd_ds)                                             const;
    unsigned    calc_samples_to_display             ()                                                          const;
    unsigned    calc_graphing_area_width            (double tpd_ds, unsigned display_width)                     const;
    double      calc_x_coord_scaling                (unsigned graphing_area_width, unsigned number_of_samples)  const;
    int         calc_x_coord_start_offset           (unsigned graphing_area_width, unsigned display_width)      const;
    int         calc_horizontal_offset_start_offset (unsigned display_width)                                    const;
    int         calc_mid_sample_to_center_offset    ()                                                          const;
    ChanDoubles calc_sample_y_scaler                ()                                                          const;
    bool        calc_mark_samples                   (double tpd_ds, unsigned samples_to_display)                const;
    int         calc_sample_x_coord                 (unsigned index)                                            const;
    int         calc_sample_y_coord                 (double sample, unsigned channel)                           const;
    ChanDoubles calc_vertical_offset                ()                                                          const;
    void        resize_pixel_points                 (PixelPoints& pixel_points, unsigned size);
    void        update_cached_display_values        ();
    void        debug                               () const;

  public:
    LAB_LABChecker_Analog (LAB& _LAB, LAB_Oscilloscope& _LAB_Oscilloscope);

          void          display_parameters    (unsigned width, unsigned height, unsigned rows, unsigned columns);
          void          update_cached_values  ();    
          void          update_pixel_points   (); 
    const PixelPoints&  pixel_points          () const;
          bool          mark_samples          () const;
};

#endif
#include "LAB_Voltmeter.h"

#include "LAB.h"

LAB_Voltmeter::
LAB_Voltmeter (LAB& _LAB)
  : LAB_Module (_LAB)
{
  m_saved_osc_state.is_valid = false;
}

void LAB_Voltmeter::
save_oscilloscope_state ()
{
  LAB_Oscilloscope& osc = lab ().m_Oscilloscope;

  OscilloscopeStateSnapshot snapshot;

  snapshot.do_measurements = osc.parent_data ().do_measurements;
  snapshot.sampling_rate   = osc.sampling_rate ();
  snapshot.samples         = osc.samples ();
  snapshot.trigger_mode    = osc.trigger_mode ();

  const unsigned num_channels = std::min<unsigned>(
    osc.channels (),
    LABC::VOLTMETER::NUMBER_OF_CHANNELS
  );

  for (unsigned ch = 0; ch < num_channels; ++ch)
  {
    auto& dst = snapshot.channels[ch];

    dst.coupling          = osc.coupling (ch);
    dst.scaling           = osc.scaling (ch);
    dst.vertical_offset   = osc.vertical_offset (ch);
    dst.voltage_per_div   = osc.voltage_per_division (ch);
    dst.scaling_corrector_to_actual =
      osc.calibration ().scaling_corrector_to_actual (ch, dst.scaling);
  }

  snapshot.is_valid = true;
  m_saved_osc_state = snapshot;
}

void LAB_Voltmeter::
restore_oscilloscope_state ()
{
  if (!m_saved_osc_state.is_valid)
  {
    return;
  }

  LAB_Oscilloscope& osc = lab ().m_Oscilloscope;
  const OscilloscopeStateSnapshot& s = m_saved_osc_state;

  osc.do_measurements (s.do_measurements);
  osc.sampling_rate   (s.sampling_rate);
  osc.samples         (s.samples);
  osc.trigger_mode    (s.trigger_mode);

  const unsigned num_channels = std::min<unsigned>(
    osc.channels (),
    LABC::VOLTMETER::NUMBER_OF_CHANNELS
  );

  for (unsigned ch = 0; ch < num_channels; ++ch)
  {
    const auto& src = s.channels[ch];

    osc.coupling        (ch, src.coupling);
    osc.scaling         (ch, src.scaling);
    osc.vertical_offset (ch, src.vertical_offset);
    osc.voltage_per_division (ch, src.voltage_per_div);

    osc.calibration ().scaling_corrector_to_actual (
      ch,
      src.scaling,
      src.scaling_corrector_to_actual
    );
  }

  // Do not reuse this snapshot again until we explicitly save it.
  m_saved_osc_state.is_valid = false;
}

void LAB_Voltmeter::
load_oscilloscope_voltmeter_mode_settings ()
{
  lab ().m_Oscilloscope.trigger_mode    (LABE::OSC::TRIG::MODE::NONE);
  lab ().m_Oscilloscope.samples         (LABC::OSC::MAX_SAMPLES);
  lab ().m_Oscilloscope.sampling_rate   (LABC::VOLTMETER::SAMPLING_RATE);
  lab ().m_Oscilloscope.do_measurements (true);

  for (int a = 0; a < LABC::VOLTMETER::NUMBER_OF_CHANNELS; a++)
  {
    lab ().m_Oscilloscope.coupling  (a, LABE::OSC::COUPLING::DC);
    lab ().m_Oscilloscope.scaling   (a, LABC::VOLTMETER::SCALING);
    lab ().m_Oscilloscope.calibration ().scaling_corrector_to_actual (a, LABC::VOLTMETER::SCALING, 1.0);
  }

  lab ().m_Oscilloscope.do_measurements (true);
}

bool LAB_Voltmeter::
is_frontend_running () const
{
  return (m_parent_data.is_frontend_running);
}

bool LAB_Voltmeter::
is_backend_running () const
{
  return (m_parent_data.is_backend_running);
}

void LAB_Voltmeter::
run ()
{
  lab ().m_Oscilloscope .stop ();
  lab ().m_Voltmeter    .stop ();
  lab ().m_Ohmmeter     .stop ();

  // Save current oscilloscope configuration so we can restore it later.
  save_oscilloscope_state ();

  //

  frontend_run_stop (true);
  backend_run_stop  (true);

  load_oscilloscope_voltmeter_mode_settings ();
}

void LAB_Voltmeter::
stop ()
{
  frontend_run_stop (false);
  backend_run_stop  (false);

  // Restore oscilloscope configuration that was active before Voltmeter run().
  restore_oscilloscope_state ();
}

void LAB_Voltmeter::
frontend_run_stop (bool value)
{
  m_parent_data.is_frontend_running = value;
}

void LAB_Voltmeter::
backend_run_stop (bool value)
{
  lab ().m_Oscilloscope.backend_run_stop (value);

  if (value) load_oscilloscope_voltmeter_mode_settings ();

  m_parent_data.is_backend_running = value;
}

double LAB_Voltmeter::
get_reading (unsigned chan)
{
  return (lab ().m_Oscilloscope.measurements ().avg (chan));
}

#ifndef __VFO_TUNER_H__
#define __VFO_TUNER_H__

class VFO_Tuner
{
public:
    // constructor
    VFO_Tuner();

    // destructor
    // ~VFO_Tuner();

    void frequency_up(int steps);

    void frequency_down(int steps);

    void band_up(int steps);

    void band_down(int steps);


    // methods to get and set the frequency, step, band, and form
    // float get_frequency() const;
    // void setFrequency(int frequency);

    // int getStep() const;
    // void setStep(int step);

    // int getBand() const;
    // void setBand(int band);

    // int getForm() const;
    // void setForm(int form);
// derived from mode_handler
};

#endif // __VFO_TUNER_H__

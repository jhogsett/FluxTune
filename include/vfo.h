#ifndef __VFO_H__
#define __VFO_H__

// need to define a set of bands

class VFO
{
public:
    // constructor
    VFO(float frequency, float step, int band);

    // destructor
    // ~VFO();

    // methods to get and set the frequency, step, band, and form
    // float get_frequency() const;
    // void setFrequency(int frequency);

    // int getStep() const;
    // void setStep(int step);

    // int getBand() const;
    // void setBand(int band);

    // int getForm() const;
    // void setForm(int form);

    float _frequency;
    float _step;
    int _band;

private:
};


// derived from mode


// has: current frequency, current step, current band, current form, 

#endif // __VFO_H__

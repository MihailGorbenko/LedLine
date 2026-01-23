#ifndef ROTARY_ENCODER_HPP
#define ROTARY_ENCODER_HPP
#include <Arduino.h>

// Пины энкодера задаются здесь (дефайнами внутри файла)
#define ROTARY_CLK_PIN 4
#define ROTARY_DT_PIN 3
#define ROTARY_SW_PIN 5

class RotaryEncoder {
public:
    enum Event { PRESS_START, PRESS_END, INCREMENT, DECREMENT };

    // Интерфейс подписчика
    class IEncoderListener {
    public:
        virtual void onEvent(Event ev, int value) = 0;
    };

    RotaryEncoder();
    void init();           
    void update();         
    void attachListener(IEncoderListener* l);
    void detachListener(IEncoderListener* l);
    void setSteps(int steps);
    void setValue(int v);
    void setBoundaries(int minV, int maxV, bool wrap);

    // added: configure acceleration/velocity behaviour
    void setAccelParams(unsigned long med_ms, unsigned long fast_ms, int med_mult, int fast_mult, float filterAlpha = 0.3f);

private:
    uint8_t _clkPin, _dtPin, _swPin;
    int _steps;
    int _value;
    int _minV, _maxV;
    bool _wrap;

    static const int MAX_LISTENERS = 4;
    IEncoderListener* _listeners[MAX_LISTENERS];
    int _listenerCount;

    int _lastClk;
    unsigned long _lastSwMillis;
    int _swState;
    bool _btnDown;

    const unsigned long DEBOUNCE_MS = 50;

    void notify(Event ev, int value);

    // added: per-instance quadrature/debounce state (supports multiple encoders)
    uint8_t _lastState;
    int8_t  _accum;
    uint8_t _lastRawSw;

    // added: velocity/acceleration state and params
    unsigned long _lastStepMillis;
    float _vel;                  // smoothed instantaneous velocity (steps/sec)
    float _velFilterAlpha;       // EMA alpha
    unsigned long _accelMedMs;   // threshold for medium accel (ms)
    unsigned long _accelFastMs;  // threshold for fast accel (ms)
    int _accelMedMult;           // medium multiplier
    int _accelFastMult;          // fast multiplier
};

#endif // ROTARY_ENCODER_HPP


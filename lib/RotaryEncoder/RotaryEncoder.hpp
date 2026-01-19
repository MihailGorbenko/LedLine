#ifndef ROTARY_ENCODER_HPP
#define ROTARY_ENCODER_HPP
#include <Arduino.h>

// Пины энкодера задаются здесь (дефайнами внутри файла)
#define ROTARY_CLK_PIN 4
#define ROTARY_DT_PIN 3
#define ROTARY_SW_PIN 5

class RotaryEncoder {
public:
    enum Event { PRESS, LONG_PRESS, INCREMENT, DECREMENT };

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
    void setValue(int v);
    void setBoundaries(int minV, int maxV, bool wrap);
    void setSteps(int steps);

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
    unsigned long _btnPressedMillis;

    const unsigned long DEBOUNCE_MS = 50;
    const unsigned long LONGPRESS_MS = 1000;

    void notify(Event ev, int value);
};

#endif // ROTARY_ENCODER_HPP


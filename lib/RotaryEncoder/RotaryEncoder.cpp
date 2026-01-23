#include "RotaryEncoder.hpp"
#include <stdint.h>

// изменённый конструктор: steps по умолчанию = 1
RotaryEncoder::RotaryEncoder()
    : _clkPin(ROTARY_CLK_PIN), _dtPin(ROTARY_DT_PIN), _swPin(ROTARY_SW_PIN),
      _steps(1),
      _value(0), _minV(0), _maxV(127), _wrap(false),
      _listenerCount(0), _lastClk(HIGH), _lastSwMillis(0),
      _swState(HIGH), _btnDown(false) {
    for (int i = 0; i < MAX_LISTENERS; ++i) _listeners[i] = nullptr;
}

// file-scope state for the state-machine (per-file; single encoder assumed)
static uint8_t s_lastState = 0;
static int8_t  s_accum = 0;

// add: raw switch previous sample for proper debounce
static uint8_t s_lastRawSw = HIGH;

// NOTE: no s_lastBtnEvent here — rely on _btnDown and debounce to avoid duplicates.
// Это упрощает логику и предотвращает возможные рассинхроны.

// таблица переходов: index = (prev<<2) | curr
// значения: -1, 0, +1 для соответствующих переходов
static const int8_t TRANS_TABLE[16] = {
    0,  1, -1,  0,
   -1,  0,  0,  1,
    1,  0,  0, -1,
    0, -1,  1,  0
};

void RotaryEncoder::init() {
    pinMode(_clkPin, INPUT_PULLUP);
    pinMode(_dtPin, INPUT_PULLUP);
    pinMode(_swPin, INPUT_PULLUP);
    _lastClk = digitalRead(_clkPin);
    _swState = digitalRead(_swPin);

    // Инициализируем state-machine
    s_lastState = (digitalRead(_clkPin) << 1) | digitalRead(_dtPin);
    s_accum = 0;

    // Инициализируем raw switch tracker для дебаунса
    s_lastRawSw = digitalRead(_swPin);
    _lastSwMillis = millis();

    // Note: file-scope state supports only single encoder instance.
}

void RotaryEncoder::setValue(int v) {
    if (v < _minV) v = _minV;
    if (v > _maxV) v = _maxV;
    _value = v;
}

void RotaryEncoder::setBoundaries(int minV, int maxV, bool wrap) {
    _minV = minV;
    _maxV = maxV;
    _wrap = wrap;
    if (_value < _minV) _value = _minV;
    if (_value > _maxV) _value = _maxV;
}

void RotaryEncoder::setSteps(int steps) {
    if (steps < 1) steps = 1;
    _steps = steps;
}

void RotaryEncoder::attachListener(IEncoderListener* l) {
    if (!l) return;
    for (int i = 0; i < _listenerCount; ++i) if (_listeners[i] == l) return;
    if (_listenerCount < MAX_LISTENERS) _listeners[_listenerCount++] = l;
}

void RotaryEncoder::detachListener(IEncoderListener* l) {
    if (!l) return;
    for (int i = 0; i < _listenerCount; ++i) {
        if (_listeners[i] == l) {
            for (int j = i; j < _listenerCount - 1; ++j) _listeners[j] = _listeners[j + 1];
            _listeners[--_listenerCount] = nullptr;
            return;
        }
    }
}
 
void RotaryEncoder::notify(Event ev, int value) {
    for (int i = 0; i < _listenerCount; ++i) if (_listeners[i]) _listeners[i]->onEvent(ev, value);
}

// Основная поллинг-логика: вызвать часто (loop)
void RotaryEncoder::update() {
    // read inputs once
    int clk = digitalRead(_clkPin);
    int dt  = digitalRead(_dtPin);
    int sw  = digitalRead(_swPin);
    unsigned long now = millis();

    // rotation: используем таблицу переходов (quadrature state machine)
    uint8_t cur = (clk << 1) | dt;
    if (cur != s_lastState) {
        uint8_t idx = (s_lastState << 2) | cur;
        int8_t delta = TRANS_TABLE[idx];
        if (delta != 0) {
            s_accum += delta;
            // clamp to avoid overflow/strange accumulation
            if (s_accum > 4) s_accum = 4;
            if (s_accum < -4) s_accum = -4;

            // 4 transitions == one full step (в одну сторону)
            if (s_accum >= 4 || s_accum <= -4) {
                int dir = (s_accum > 0) ? 1 : -1;
                int old = _value;
                _value += dir * _steps;
                if (_value > _maxV) {
                    if (_wrap) _value = _minV;
                    else _value = _maxV;
                } else if (_value < _minV) {
                    if (_wrap) _value = _maxV;
                    else _value = _minV;
                }
                if (_value != old) {
                    notify(dir > 0 ? INCREMENT : DECREMENT, _value);
                }
                s_accum = 0;
            }
        } else {
            // некорректный/скачкообразный переход — сбрасываем аккум для безопасности
            s_accum = 0;
        }
        s_lastState = cur;
        _lastClk = (cur >> 1) & 1;
    }

    // button: debounce, выдаём PRESS_START и PRESS_END, без блокировки и без long-press
    // track raw changes separately; update debounced state only after stable interval
    if (sw != s_lastRawSw) {
        s_lastRawSw = sw;
        _lastSwMillis = now;
    } else {
        if (sw != _swState && (now - _lastSwMillis) > DEBOUNCE_MS) {
            // confirmed stable change -> update debounced state and notify
            _swState = sw;
            if (!_btnDown && _swState == LOW) {
                _btnDown = true;
                notify(PRESS_START, 0);
            } else if (_btnDown && _swState == HIGH) {
                _btnDown = false;
                notify(PRESS_END, 0);
            }
        }
    }
}
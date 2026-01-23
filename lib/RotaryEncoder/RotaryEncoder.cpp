#include "RotaryEncoder.hpp"
#include <stdint.h>

// изменённый конструктор: steps по умолчанию = 1
RotaryEncoder::RotaryEncoder()
    : _clkPin(ROTARY_CLK_PIN), _dtPin(ROTARY_DT_PIN), _swPin(ROTARY_SW_PIN),
      _steps(1),
      _value(0), _minV(0), _maxV(127), _wrap(false),
      _listenerCount(0), _lastClk(HIGH), _lastSwMillis(0),
      _swState(HIGH), _btnDown(false),
      // init new per-instance state
      _lastState(0), _accum(0), _lastRawSw(HIGH),
      // velocity/accel defaults
      _lastStepMillis(0), _vel(0.0f), _velFilterAlpha(0.3f),
      _accelMedMs(100), _accelFastMs(40), _accelMedMult(2), _accelFastMult(3) {
    for (int i = 0; i < MAX_LISTENERS; ++i) _listeners[i] = nullptr;
}

// таблица переходов остаётся в файле
static const int8_t TRANS_TABLE[16] = {
    0,  1, -1,  0,
   -1,  0,  0,  1,
    1,  0,  0, -1,
    0, -1,  1,  0
};

// added: position mapping for states 00,01,11,10 -> sequence indices 0..3
static const int8_t POS_OF_STATE[4] = { 0, 1, 2, 3 }; // map by sequence order: 00,01,11,10
// Note: states are encoded as (clk<<1)|dt -> values {0,1,3,2}, we'll index via a small remap
static const uint8_t STATE_TO_POS_IDX[4] = { 0, 1, 3, 2 }; // index by state value -> position in seq(0..3)

void RotaryEncoder::setAccelParams(unsigned long med_ms, unsigned long fast_ms, int med_mult, int fast_mult, float filterAlpha) {
    if (med_ms == 0) med_ms = 1;
    if (fast_ms == 0) fast_ms = 1;
    if (filterAlpha <= 0.0f || filterAlpha > 1.0f) filterAlpha = 0.3f;
    _accelMedMs = med_ms;
    _accelFastMs = fast_ms;
    _accelMedMult = (med_mult > 0) ? med_mult : 2;
    _accelFastMult = (fast_mult > 0) ? fast_mult : 4;
    _velFilterAlpha = filterAlpha;
}

void RotaryEncoder::init() {
    pinMode(_clkPin, INPUT_PULLUP);
    pinMode(_dtPin, INPUT_PULLUP);
    pinMode(_swPin, INPUT_PULLUP);
    _lastClk = digitalRead(_clkPin);
    _swState = digitalRead(_swPin);

    // Инициализируем state-machine (per-instance)
    _lastState = (digitalRead(_clkPin) << 1) | digitalRead(_dtPin);
    _accum = 0;

    // Инициализируем raw switch tracker для дебаунса
    _lastRawSw = digitalRead(_swPin);
    _lastSwMillis = millis();

    // Note: state is now per-instance (supports multiple encoders)
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
    if (cur != _lastState) {
        uint8_t idx = (_lastState << 2) | cur;
        int8_t delta = TRANS_TABLE[idx];

        if (delta == 0) {
            // попытка восстановления: оценить насколько продвинулись по кольцу состояний
            int8_t posLast = STATE_TO_POS_IDX[_lastState];
            int8_t posCur  = STATE_TO_POS_IDX[cur];
            int8_t diff = posCur - posLast;
            if (diff > 2) diff -= 4;
            if (diff < -2) diff += 4;
            // diff теперь в диапазоне -2..2 (0, ±1, ±2)
            delta = diff;
        }

        if (delta != 0) {
            _accum += delta;
            // process all full steps accumulated (4 transitions == 1 full step)
            int fullSteps = _accum / 4; // may be >1 or < -1 on fast spin
            if (fullSteps != 0) {
                // compute dt and update smoothed velocity (steps/sec),
                // account for number of steps processed
                unsigned long dt_ms = (_lastStepMillis == 0) ? 0UL : (unsigned long)(now - _lastStepMillis);
                if (dt_ms != 0UL) {
                    float inst_vel = (1000.0f / (float)dt_ms) * (float)abs(fullSteps); // steps/sec
                    _vel = _velFilterAlpha * inst_vel + (1.0f - _velFilterAlpha) * _vel;
                }

                // choose multiplier by dt (smaller dt => faster => larger mult)
                int mult = 1;
                if (dt_ms != 0UL) {
                    if (dt_ms <= _accelFastMs) mult = _accelFastMult;
                    else if (dt_ms <= _accelMedMs) mult = _accelMedMult;
                }

                int old = _value;
                // apply all full steps at once (fullSteps signed)
                _value += fullSteps * _steps * mult;
                if (_value > _maxV) {
                    if (_wrap) _value = _minV;
                    else _value = _maxV;
                } else if (_value < _minV) {
                    if (_wrap) _value = _maxV;
                    else _value = _minV;
                }
                if (_value != old) {
                    notify((fullSteps > 0) ? INCREMENT : DECREMENT, _value);
                }

                // consume processed transitions, keep remainder (-3..3)
                _accum -= fullSteps * 4;
                _lastStepMillis = now;
            }
        } else {
            // delta == 0 even after recovery -> неявное шумовое изменение состояния
            // НЕ сбрасываем _accum чтобы не потерять накопленные переходы
        }

        _lastState = cur;
        _lastClk = (cur >> 1) & 1;
    }

    // button: debounce, выдаём PRESS_START и PRESS_END, без блокировки и без long-press
    // track raw changes separately; update debounced state only after stable interval
    if (sw != _lastRawSw) {
        _lastRawSw = sw;
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
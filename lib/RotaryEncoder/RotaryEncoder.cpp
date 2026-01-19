#include "RotaryEncoder.hpp"

// изменённый конструктор: steps по умолчанию = 1
RotaryEncoder::RotaryEncoder()
    : _clkPin(ROTARY_CLK_PIN), _dtPin(ROTARY_DT_PIN), _swPin(ROTARY_SW_PIN),
      _steps(1), _value(0), _minV(0), _maxV(127), _wrap(false),
      _listenerCount(0), _lastClk(HIGH), _lastSwMillis(0),
      _swState(HIGH), _btnDown(false), _btnPressedMillis(0) {
    for (int i = 0; i < MAX_LISTENERS; ++i) _listeners[i] = nullptr;
}

void RotaryEncoder::init() {
    pinMode(_clkPin, INPUT_PULLUP);
    pinMode(_dtPin, INPUT_PULLUP);
    pinMode(_swPin, INPUT_PULLUP);
    _lastClk = digitalRead(_clkPin);
    _swState = digitalRead(_swPin);
}

void RotaryEncoder::setSteps(int steps) {
    if (steps < 1) steps = 1;
    _steps = steps;
}

void RotaryEncoder::setBoundaries(int minV, int maxV, bool wrap) {
    _minV = minV;
    _maxV = maxV;
    _wrap = wrap;
    if (_value < _minV) _value = _minV;
    if (_value > _maxV) _value = _maxV;
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


void RotaryEncoder::setValue(int v) {
    if (v < _minV) v = _minV;
    if (v > _maxV) v = _maxV;
    _value = v;
}

// Основная поллинг-логика: вызвать часто (loop)
void RotaryEncoder::update() {
    // rotation
    int clk = digitalRead(_clkPin);
    int dt = digitalRead(_dtPin);

    if (clk != _lastClk) {
        // реагируем на фронт (rising)
        if (clk == HIGH) {
            int dir = (dt == LOW) ? 1 : -1;
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
        }
        _lastClk = clk;
    }

    // button with debounce and long-press detection
    int sw = digitalRead(_swPin);
    unsigned long now = millis();
    if (sw != _swState) {
        // state changed -> debounce timer reset
        _lastSwMillis = now;
        _swState = sw;
    } else {
        // stable
        if (!_btnDown && _swState == LOW && (now - _lastSwMillis) > DEBOUNCE_MS) {
            // press detected
            _btnDown = true;
            _btnPressedMillis = now;
        }
        if (_btnDown) {
            // long press check
            if ((now - _btnPressedMillis) >= LONGPRESS_MS) {
                // notify long press once and mark handled by setting _btnDown = false and waiting for release
                notify(LONG_PRESS, _value);
                // wait until release; avoid repeating
                _btnDown = false;
                // consume until release
                while (digitalRead(_swPin) == LOW) { /* spinner until release to avoid duplicate press */ delay(1); }
                _lastSwMillis = millis();
                _swState = digitalRead(_swPin);
            } else if (_swState == HIGH && (now - _lastSwMillis) > DEBOUNCE_MS) {
                // release before long press -> short press
                notify(PRESS, _value);
                _btnDown = false;
            }
        }
    }
}
#include "button.h"

#include <Arduino.h>

namespace controlino
{

namespace duration
{

// values are in ms

constexpr unsigned short Touch       = 20;
constexpr unsigned short Slip        = 20;
constexpr unsigned short Press       = 800;
constexpr unsigned short DoubleClick = 220;

} // duration

Button::Button(Pin pin) : Control(pin, Mode::Pullup)
{}

Button::Button(Multiplexer & multiplexer, Pin pin) : Control(multiplexer, pin, Mode::Pullup)
{}

Button::State Button::check()
{
    const auto now = static_cast<unsigned short>(millis());

    static_assert(sizeof(now) == sizeof(Button::_when), "Sizes must match"); // otherwise calculating the difference would be misleading

    const auto pressed = digitalRead() == LOW;

    if (pressed == true && _what == What::Idle)
    {
        // we tell the user nothing as we want to make sure this is not just a touch but an actual press

        _what = What::Touch;
        _when = now;
        return {Event::None, pressed};
    }

    if (pressed == false && _what == What::Touch)
    {
        // go back to idle mode as the touch was probably just a glitch

        _what = What::Idle;
        return {Event::None, pressed};
    }

    if (pressed == true && _what == What::Touch && (unsigned short)(now - _when) > duration::Touch)
    {
        // now we are sure this is an actual press

        _what = What::Press;
        // don't change '_when' as we want to count from the touch
        return {Event::Down, pressed};
    }

    if (pressed == true && _what == What::Press && (unsigned short)(now - _when) >= duration::Press)
    {
        // the button is being pressed long enough for it to count as a long press

        _what = What::Drain;
        return {Event::Press, pressed};
    }


    if (pressed == false && _what == What::Press)
    {
        // we tell the user nothing as we want to make sure this is not just a slip but an actual release

        _what = What::Slip;
        _when = now;
        return {Event::None, pressed};
    }

    if (pressed == true && _what == What::Slip)
    {
        // ignore this slip and act as if the button is still being pressed as it was repressed right away

        _what = What::Press;
        // although '_when' was modified when the slip took place, it is the best we could do
        return {Event::None, pressed};
    }

    if (pressed == false && _what == What::Slip && (unsigned short)(now - _when) > duration::Slip)
    {
        // now we are sure this is an actual release

        _what = What::Release;
        // don't change '_when' as we want to count from the slip
        return {Event::Up, pressed};
    }

    if (pressed == false && _what == What::Release && (unsigned short)(now - _when) > duration::DoubleClick)
    {
        // this is a single click as there was no second click after waiting for a long enough period of time

        _what = What::Idle;
        return {Event::Click, pressed};
    }

    if (pressed == true && _what == What::Release)
    {
        // a second click in the allowed period of time for a double click

        _what = What::SecondPress;
        _when = now;
        return {Event::Down, pressed};
    }

    if (pressed == true && _what == What::SecondPress && (unsigned short)(now - _when) >= duration::Press)
    {
        // the button is being pressed long enough for it to count as a long press

        _what = What::Drain;
        return {Event::ClickPress, pressed};
    }

    if (pressed == false && _what == What::SecondPress)
    {
        _what = What::SecondRelease;
        return {Event::Up, pressed};
    }

    if (pressed == false && _what == What::SecondRelease)
    {
        // although there is no need to wait after the 'Up' event,
        // as we do not support more than two clicks, we implement this
        // stage in order to fire both 'Up' and 'ClickClick' events

        _what = What::Idle;
        return {Event::ClickClick, pressed};
    }

    if (pressed == false && _what == What::Drain)
    {
        _what = What::Idle;
        return {Event::Up, pressed};
    }

    if (pressed == true)
        return {Event::Held, pressed};

    return {Event::None, pressed};
}

} // controlino

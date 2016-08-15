#include "pa_object.hpp"

#include <string.h>

#include "pa.hpp"

PaObject::PaObject() : type(pa_object_t::SINK)
{
    index = 0;
    memset(name, 0, sizeof(name));
    channels = 0;
    mute = false;
    monitor_index = 0;
    monitor_stream = nullptr;
    peak = 0;
    pa_set_volume = nullptr;
    pa_set_mute = nullptr;
    pa_move = nullptr;
    pa_set_profile = nullptr;

}

PaObject::~PaObject()
{
    if (active_profile != nullptr)  {
        delete active_profile;
    }

    if (active_port != nullptr) {
        delete active_port;
    }

    clearProfiles();
    clearPorts();
}

void PaObject::set_volume(float perc)
{

    if (pa_set_volume != nullptr) {
        pa_threaded_mainloop_lock(pa.pa_ml);

        int vol = (PA_VOLUME_NORM * perc);
        pa_cvolume cvol;

        pa_cvolume_init(&cvol);
        pa_cvolume_set(&cvol, channels, vol);


        pa_operation *o = pa_set_volume(
                              pa.pa_ctx,
                              index,
                              &cvol,
                              NULL,
                              NULL
                          );
        pa_operation_unref(o);
        pa_threaded_mainloop_unlock(pa.pa_ml);
    }
}

void PaObject::step_volume(int dir)
{
    if (volume <= 1000 && dir == -1) {
        return;
    }

    set_volume(static_cast<float>(volume + (1000 * dir)) / PA_VOLUME_NORM);
}


void PaObject::toggle_mute()
{
    if (pa_set_mute != nullptr) {
        pa_operation *o = pa_set_mute(
                              pa.pa_ctx,
                              index,
                              !mute,
                              NULL,
                              NULL
                          );
        pa_operation_unref(o);
    }
}


void PaObject::move(uint32_t dest)
{
    if (pa_move != nullptr) {
        pa_operation *o = pa_move(
                              pa.pa_ctx,
                              index,
                              dest,
                              NULL,
                              NULL
                          );
        pa_operation_unref(o);
    }
}


void PaObject::set_profile(const char *profile)
{
    if (pa_set_profile != nullptr) {
        pa_operation *o = pa_set_profile(
                              pa.pa_ctx,
                              index,
                              profile,
                              NULL,
                              NULL
                          );
        pa_operation_unref(o);
    }
}

void PaObject::clearPorts()
{
    for (int i = 0; i < ports.size(); i++) {
        delete ports[i];
    }

    ports.clear();
}

void PaObject::clearProfiles()
{
    for (int i = 0; i < profiles.size(); i++) {
        delete profiles[i];
    }

    profiles.clear();
}
/*
 *  fileinput.cpp
 *
 *  Copyright (c) 2010 Motiejus Jakštys
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3.

 *  This program is distributed in the hope that it will be usefu
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.

 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "fileinput.h"

int FileInput::giveInput(buffer_t *buf_prop) {
    if (reading_over) { return 0; }

    unsigned bufferlength = (unsigned)(s->signal.rate+0.5) * BUFFERSIZE;

    sox_sample_t buf [bufferlength]; // Take BUFFERSIZE seconds
    size_t read_size = sox_read(s, buf, bufferlength);

    /* TODO: if length of buffer is smaller then always,
     * make reading_over = true here too
     */
    if (read_size < bufferlength) {
        LOG_INFO("EOF reached. Read_size: %d, bufferlength: %d",
                read_size, bufferlength);
        reading_over = true;
    }
	buf_prop->buf = (sample_t*) calloc(read_size, sizeof(sample_t));

	for (unsigned i = 0; i < read_size; i++) {
		buf_prop->buf[i] = (sample_t)buf[i];
	}
    buf_prop->nframes = read_size;
    return 1;
};


FileInput::FileInput(const void * args, all_cfg_t *cfg) {
    // Filename given
    reading_over = false;
    s = sox_open_read((char*)args, NULL, NULL, NULL);
    LOG_DEBUG("Sox initialized. Sampling rate: %.6f, channels: %d, bitrate: %d, samples: %d",
            s->signal.rate, s->signal.channels, s->signal.precision, s->signal.length);
    SAMPLE_RATE = (int)s->signal.rate;

    for (vector<sVolumes>::iterator vol = cfg->second.begin(); vol != cfg->second.end(); vol++) {
        vol->min *= (1<<30);
        vol->max *= (1<<30);
    }
};


FileInput::~FileInput() {
    LOG_INFO("Closing sox instance");
    sox_close(s);
}
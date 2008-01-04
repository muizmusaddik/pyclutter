#!/usr/bin/env python
# -*- coding: utf-8 -*-

# cluttergst.VideoSink example
# Copyright (C) 2007  Florent Thiery

import sys
import clutter, gst
from clutter import cluttergst

def main (args):
    stage = clutter.stage_get_default()
    stage.set_size(320,240)
    stage.set_color(clutter.color_parse('DarkSlateGrey'))
    stage.connect('key-press-event', clutter.main_quit)

    # Creates a gstreamer texture
    video_texture = clutter.Texture()
    video_texture.show()

    pipeline = gst.Pipeline()

    src = gst.element_factory_make("audiotestsrc")
    goom = gst.element_factory_make("goom")
    colorspace = gst.element_factory_make("ffmpegcolorspace")

    # Attaches the output texture to gstreamer sink
    sink = cluttergst.VideoSink(video_texture)

    pipeline.add(src, goom, colorspace, sink)
    gst.element_link_many(src, goom, colorspace, sink)
    pipeline.set_state(gst.STATE_PLAYING)

    stage.add(video_texture)
    stage.show_all()
    clutter.main()

    return 0

if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
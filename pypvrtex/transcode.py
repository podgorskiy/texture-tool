# Copyright 2020 Stanislav Pidhorskyi
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#  http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ==============================================================================

import pypvrtex


def transcode(texture, format, channel_type=None, colour_space=None, quality=pypvrtex.Quality.Normal, dither=False):
    newtex = pypvrtex.copy(texture)
    if isinstance(format, str):
        if format in pypvrtex.PixelFormat.__entries:
            format = pypvrtex.PixelFormat.__entries[format][0]
        else:
            format = pypvrtex.format_from_string(format)
    if channel_type is None:
        channel_type = texture.channel_type
    if colour_space is None:
        colour_space = texture.colour_space
    res = pypvrtex.inplace_transcode(newtex, format, channel_type, colour_space, quality, dither)
    if not res:
        raise RuntimeError('Operation failed')
    return newtex
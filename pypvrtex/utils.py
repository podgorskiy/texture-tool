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


def resize(texture, width, height, depth=1, resize_mode=pypvrtex.ResizeMode.Cubic):
    if texture.num_mip_levels > 1:
        raise RuntimeError('"resize" operation is not permitted on texture with mipmap layers. '
                           'Generate mipmaps after all transformations are done.')
    newtex = pypvrtex.copy(texture)
    res = pypvrtex.inplace_resize(newtex, width, height, depth, resize_mode)
    if not res:
        raise RuntimeError('Operation failed')
    return newtex


def resize_canvas(texture, width, height, depth, x_offset, y_offset, z_offset):
    newtex = pypvrtex.copy(texture)
    res = pypvrtex.inplace_resize_canvas(newtex, width, height, depth, x_offset, y_offset, z_offset)
    if not res:
        raise RuntimeError('Operation failed')
    return newtex


def rotate90(texture, axis, forward):
    newtex = pypvrtex.copy(texture)
    res = pypvrtex.inplace_rotate90(newtex, axis, forward)
    if not res:
        raise RuntimeError('Operation failed')
    return newtex


def flip(texture, axis=pypvrtex.Axis.y):
    newtex = pypvrtex.copy(texture)
    res = pypvrtex.inplace_flip(newtex, axis)
    if not res:
        raise RuntimeError('Operation failed')
    orientation = newtex.get_orientation(axis)
    newtex.set_orientation(axis, not orientation)
    if newtex.num_faces == 6:
        v1 = pypvrtex.view(newtex, 0, 2)
        v2 = pypvrtex.view(newtex, 0, 3)
        v1_ = v1 + 0
        v2_ = v2 + 0
        v2[:] = v1_
        v1[:] = v2_

    return newtex


def colour_mipmaps(texture):
    newtex = pypvrtex.copy(texture)
    res = pypvrtex.inplace_colour_mipmaps(newtex)
    if not res:
        raise RuntimeError('Operation failed')
    return newtex


def bleed(texture):
    newtex = pypvrtex.copy(texture)
    res = pypvrtex.inplace_bleed(newtex)
    if not res:
        raise RuntimeError('Operation failed')
    return newtex


def premultiply_alpha(texture):
    newtex = pypvrtex.copy(texture)
    res = pypvrtex.inplace_premultiply_alpha(newtex)
    if not res:
        raise RuntimeError('Operation failed')
    return newtex

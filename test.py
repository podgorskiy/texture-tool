import gc
import time
import imageio
import sys
import PIL
from PIL import Image
import io
import numpy as np
import texture_tool as tt


print('imported')
############################


def timeit(method):
    def timed(*args, **kw):
        ds = 0.0
        trials = 6
        for i in range(trials):
            ts = time.time()
            result = method(*args, **kw)
            te = time.time()
            delta = te - ts
            ds += delta
            print('%r  %2.2f ms' % (method.__name__, delta * 1000))
        ds /= trials
        print(method.__name__)
        print('Total: %r  %2.2f ms' % (method.__name__, ds * 1000))
        return result
    return timed


print('Supported formats:')
print(dir(tt.PixelFormat))


tex = tt.imread('test.jpg')

# tex = pvr.imread('test.pvr')

print(tex)

# tex.save_pvr('test.pvr')

#print(tex)

# tex = pvr.utils.resize(tex, 600, 700)

tex = tt.generate_mipmaps(tex)

tex = tt.utils.flip(tex)

print(tex)

print(tex)

tex.save_pvr('test.pvr')

tex.save_dds('test_2.dds')

tex = tt.transcode(tex, 'RGBA8888', dither=True)

tex.colour_space = tt.ColourSpace.sRGB

print(tex)

tex.save_pvr('test_2.pvr')


# tex = pvr.generate_mipmaps(tex, mipmaps=2)
#
# print(tex)

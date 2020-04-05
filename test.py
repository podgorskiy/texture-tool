import gc
import time
import imageio
import sys
import PIL
import PIL.Image
import io
import numpy as np
import pypvrtex as pvr


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


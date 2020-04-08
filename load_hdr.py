import pypvrtex


tex = pypvrtex.imread('railway_bridge_02_1k.hdr')

print(tex)

tex.save_pvr('test_hdr.pvr')

texr = pypvrtex.utils.resize(tex, 600, 700)
tex = pypvrtex.utils.flip(tex)

tex = pypvrtex.generate_mipmaps(tex)

print(tex)


tex.save_pvr('test_hdr.pvr')

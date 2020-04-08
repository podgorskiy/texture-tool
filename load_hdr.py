import pypvrtex


tex = pypvrtex.imread('railway_bridge_02_1k.hdr')

print(tex)

tex = pypvrtex.cubemap_from_equirectangular(tex)
tex = pypvrtex.utils.flip(tex)

print(tex)


tex.save_pvr('test_hdr.pvr')

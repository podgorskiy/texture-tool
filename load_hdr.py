import textool


tex = textool.imread('railway_bridge_02_1k.hdr')

print(tex)

tex = textool.cubemap_from_equirectangular(tex)
tex = textool.utils.flip(tex)

print(tex)


tex.save_pvr('test_hdr.pvr')

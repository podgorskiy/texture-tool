import texture_tool


tex = texture_tool.imread('railway_bridge_02_1k.hdr')

print(tex)

tex = texture_tool.cubemap_from_equirectangular(tex)
tex = texture_tool.utils.flip(tex)

print(tex)


tex.save_pvr('test_hdr.pvr')

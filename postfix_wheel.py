import os
import zipfile
import tempfile
import sys


def updateZip(zipname, filename, replacement):
    # generate a temp file
    tmpfd, tmpname = tempfile.mkstemp(dir=os.path.dirname(zipname))
    os.close(tmpfd)

    # create a temp copy of the archive without filename
    with zipfile.ZipFile(zipname, 'r') as zin:
        with zipfile.ZipFile(tmpname, 'w') as zout:
            zout.comment = zin.comment # preserve the comment
            for item in zin.infolist():
                if item.filename != filename:
                    zout.writestr(item, zin.read(item.filename))

    # replace with the temp archive
    os.remove(zipname)
    os.rename(tmpname, zipname)

    # now add filename with its new data
    with zipfile.ZipFile(zipname, mode='a', compression=zipfile.ZIP_DEFLATED) as zf:
        zf.write(replacement, filename)


wheel = sys.argv[1]
print('Replacing libPVRTexLib with backup in %s' % wheel)
updateZip(wheel, 'libPVRTexLib.so', '3dparty/PVRTexTool/Linux_x86_64/libPVRTexLib.so')

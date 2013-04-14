import fnmatch
import shutil
import sys
import os.path
import fragmon

minilib = os.path.join(fragmon.MODPATH, 'minilib')


print
print 'mod deps'
for key in sys.modules:
    mod = sys.modules[key]
    if hasattr(mod, '__file__'):
        if not fnmatch.fnmatch(mod.__file__, '*gamequery*'):
            print mod.__file__
            shutil.copy(mod.__file__, minilib)
print


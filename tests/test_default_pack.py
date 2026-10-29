"""Exercise the real installer and bundled SDK inflater on the host."""
from pathlib import Path
import os,subprocess,tempfile,sys
root=Path(__file__).resolve().parents[1]
sdk=Path(os.environ.get('PLATFORMIO_CORE_DIR',Path.home()/'.platformio'))/'packages/framework-arduinoespressif8266/tools/sdk/uzlib/src'
subprocess.run([sys.executable,str(root/'tools/embed_web.py')],check=True)
with tempfile.TemporaryDirectory(prefix='pixelmb-defaults-') as temp:
    temp=Path(temp);objects=[]
    for name in ['tinflate','tinfzlib','adler32','crc32']:
        obj=temp/(name+'.o');objects.append(str(obj))
        subprocess.run(['cc','-std=c99','-I'+str(sdk),'-c',str(sdk/(name+'.c')),'-o',str(obj)],check=True)
    executable=temp/'test-defaults'
    subprocess.run(['c++','-std=c++17','-Wall','-Wextra','-I'+str(root/'tests/default_store_stubs'),'-I'+str(root/'include'),'-I'+str(sdk),str(root/'tests/test_default_store.cpp'),str(root/'src/DefaultStore.cpp'),*objects,'-o',str(executable)],check=True)
    subprocess.run([str(executable),str(temp/'fs'),str(root/'data/animations')],check=True)

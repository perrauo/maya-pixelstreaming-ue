
* You've got to add these in your Maya.env

MAYA_PLUG_IN_PATH=C:\Users\<Username>\devkitBase\plug-ins\plug-ins
MAYA_SCRIPT_PATH=C:\Users\<Username>\devkitBase\plug-ins\scripts
XBMLANGPATH=C:\Users\<Username>\devkitBase\plug-ins\icons

* You've got to set those envs

set DEVKIT_LOCATION=C:\Users\<Username>\devkitBase\
set MAYA_LOCATION="C:\Program Files\Autodesk\<maya_version>"
set PATH=%PATH%;%MAYA_LOCATION%\bin


UFE_INCLUDE_ROOT
UFE_LIBRARY
UFE_INCLUDE_DIR


  Could not find Qt 6.5 in Maya devkit directory: .  You must extract
  Qt.tar.gz.


cmake -G "Visual Studio 17 2022" -A x64 -DUFE_INCLUDE_DIR=C:\Users\olivi\devkitBase\devkit\ufe\include -DUFE_LIBRARY=C:\Users\olivi\devkitBase\devkit\ufe\lib .
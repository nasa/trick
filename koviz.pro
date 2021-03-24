
include (koviz.pri)

CONFIG += ordered
TEMPLATE = subdirs
SUBDIRS = libkoviz \
          koviz

SOURCES += blender/koviz.py \
           blender/koviz-hello-world.py

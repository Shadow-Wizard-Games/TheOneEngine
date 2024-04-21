@echo off
call tools\ozz\fbx2ozz.exe --file=%1 --config_file="Tools\import_anim.json"
call tools\ozz\fbx2mesh.exe --file=%1 --mesh=%2 --skeleton=%3
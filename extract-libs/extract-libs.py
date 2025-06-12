
#!/usr/bin/python
#encoding: utf-8

import sys
import os
import re
import shutil
import argparse
import logging

FILE_NOT_EXISTS_MSG = "%s is not exists!"

def extract_qt_plugins_libs(dest_path):
    logging.info("Start to extract qt plugins libs...");
    
    qt_plugin_path = "/usr/lib64/qt5/"
    #add plugins dir whitch you needed
    plugins ={"plugins/bearer", "plugins/iconengines", "plugins/platformthemes", "plugins/styles", "plugins/imageformats", "plugins/platforminputcontexts", "plugins/platforms", "plugins/designer", "plugins/xcbglintegrations"}
    
    dest_path = dest_path + "/plugins"
    if not os.path.exists(dest_path):
        os.system("mkdir -p %s" % (dest_path))
        
    for plugin in plugins:
        path = qt_plugin_path + plugin
        if os.path.exists(path):
            os.system("cp -r %s %s" % (path,dest_path))
        else:
            logging.debug(FILE_NOT_EXISTS_MSG %(path))

# for qtwebengine, resource files must be in the same directory as the running files
def extract_qt_resources(dest_path):
    logging.info("Start to extract qt resource...")
    qt_resource_path = "/usr/share/qt5/resources"
    dest_path = dest_path + "/bin"
    for root, _, files in os.walk(qt_resource_path):
        for file in files:
            source_path = os.path.join(root, file)
            shutil.copy(source_path, dest_path)

def extract_specific_libs(deps_file, dest_path):
    logging.info("Start to extract specific libs...");
    
    f  =  open(deps_file,  "r")
    info  =  f.readlines()
    full_path = dest_path
    
    for line in info:
        line = line.strip()
        if len(line) == 0:
            continue

        if line.startswith("#"):
            continue
        
        if line.startswith("[") and line.endswith("]"):
            line = line.lstrip("[").rstrip("]")
            sub_path = line.lstrip("/")
            full_path = dest_path + sub_path
            if not os.path.exists(full_path):
                os.system("mkdir -p %s" % (full_path))

            logging.info("extract libs to %s\n" %(full_path))
        else:
            if os.path.exists(line):
                cmd =  "cp -L %s %s" % (line,full_path)
                os.system(cmd)
            else:
                logging.debug(FILE_NOT_EXISTS_MSG %(line))
    f.close()

def main():
    parser = argparse.ArgumentParser(description='Extract depends libs of project to specific path.')
    parser.add_argument('dest_path', type=str, help='The path to store dependens libs.')
    parser.add_argument('--need_qt_plugins', action='store_true', default=False, help='Need qt plugins libs or not.')
    parser.add_argument('--need_qt_resources', action='store_true', default=False, help='Need qt resources or not.')
    args = parser.parse_args()
    
    #get deps lib txt file
    cur_path = os.path.split(os.path.realpath(__file__))[0]
    if not cur_path.endswith("/"):
        cur_path = cur_path + "/"
    deps_file = cur_path + "deps-libs.txt"

    if not os.path.exists(deps_file):
        logging.debug(FILE_NOT_EXISTS_MSG %(deps_file))
        exit(0)

    #get dest path
    dest_path = args.dest_path
    if not dest_path.endswith("/"):
    	dest_path = dest_path + "/"
    if not os.path.isdir(dest_path):
        os.makedirs(dest_path)

    #extract libs
    extract_specific_libs(deps_file, dest_path);
    if(args.need_qt_plugins):
        extract_qt_plugins_libs(dest_path+"/lib64");
    if(args.need_qt_resources):
        extract_qt_resources(dest_path)


if __name__ == "__main__":
    logger = logging.getLogger()
    logger.setLevel(logging.DEBUG)
    formatter = logging.Formatter('%(asctime)s - %(filename)s[line:%(lineno)d] - %(levelname)s: %(message)s')

    stream_handler = logging.StreamHandler()
    stream_handler.setLevel(logging.DEBUG)
    stream_handler.setFormatter(formatter)
    logger.addHandler(stream_handler)
    main()


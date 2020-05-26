#!/usr/bin/env python

import os
import sys
import re
import fnmatch

sys.path.append(os.path.normpath(
  os.path.join(os.path.dirname(os.path.realpath(__file__)),
               "../../../code_generator")))
import code_generator as cg


#### Generator helper functions ###############################################

def isCompatible(module, context):
  # host compatibility
  compHost = False
  if "hostCompat" not in module \
    or not module["hostCompat"] \
    or context["PLATFORM"] in module["hostCompat"]:
    compHost = True
  # target compatibility
  compTarget = False
  if "targetCompat" not in module \
    or not module["targetCompat"]:
    compTarget = True
  else:
    for target in module["targetCompat"]:
      if cg.isArchCompatible(target, context["ARCHITECTURE_ID"]):
        compTarget = True
        break
  return compHost and compTarget


#### Generator logic ##########################################################

def generate(cgm, rcgm, context, step):
  # Switch on/off verbose logging
  cg.setVerbose(cgm["verbose"])
  # Get framework version
  cgVer = cg.getVersion()
  if cgm["verbose"]:
    cg.printLog("**** Using Code Generator Framework %s to %s project ****" % (cgVer, step))
  if cgVer != "1.0.0":
    cg.reportError("Incompatible Code Generator Framework version: %s" % cgVer)
    return cg.getResult()
  # run only if compatible
  if isCompatible(cgm, context):
    if cgm["verbose"]:
      cg.printLog("**** Running %s Code Generator ****" % cgm["name"])
    # add incremental generation command
    if step == "configure":
      cg.incrementalGeneration()
    # Iterate over required file groups
    for rGroup in rcgm["groups"]:
      for group in cgm["groups"]:
        # skip until code generator group found
        if not fnmatch.fnmatch(group["id"], rGroup["id"]):
          continue
        # skip if not relevant in actual project creation step
        # if 'step' key is not present, group is considered relevant
        if "step" in group and group["step"] != step:
          continue
        # compatible group of files
        if isCompatible(group, context):
          if cgm["verbose"]:
            cg.printLog("* Group: %s " % group["name"])
          srcDir = os.path.join(os.path.dirname(os.path.abspath(__file__)),
                                cgm["srcDir"],
                                group["srcDir"])
          dstDir = os.path.join(context["PROJECT_INFO"]["rootDir"],
                                cgm["dstDir"])
          # copy files
          if group["command"] == "copy":
            cg.setupEnvironment(srcDir, dstDir)
            if "files" in group:
              for f in group["files"]:
                if os.path.isfile(os.path.join(srcDir, f)):
                  # copy file, keep its name
                  if not cg.copyFile(f, os.path.join(group["dstDir"], f)):
                    return cg.getResult()
                  if f.endswith(".h"):
                    if not cg.addIncludeDirectory(os.path.join(group["dstDir"], os.path.dirname(f))):
                      return cg.getResult()
                # if os.path.isdir(os.path.join(srcDir, f)):
                #   for root, dirs, files in os.walk(os.path.join(srcDir, f)):
                #     for file in files:
                #       file_path = (os.path.join(root,file))
                #       if not cg.copyFile(f, os.path.join(group["dstDir"], os.path.relpath(file_path,srcDir))):
                #         return cg.getResult()
          # link files
          elif group["command"] == "link":
            cg.setupEnvironment(srcDir, dstDir)
            if "files" in group:
              for f in group["files"]:
                if os.path.isfile(os.path.join(srcDir, f)):
                  # link file
                  if not cg.linkFile(f, os.path.join(group["dstDir"], f)):
                    return cg.getResult()
                  if f.endswith(".h"):
                    if not cg.addIncludeDirectory(os.path.join(group["dstDir"], os.path.dirname(f))):
                      return cg.getResult()
          # add include path
          elif group["command"] == "include":
            cg.setupEnvironment(srcDir, dstDir)
            if "incDirs" in group:
              for d in group["incDirs"]:
                if not cg.addIncludeDirectory(os.path.join(group["dstDir"], d)):
                  return cg.getResult()
          # render templates
          elif group["command"] == "render":
            if "imports" in group:
              # get import directories
              impDirs = [os.path.join(os.path.dirname(__file__), \
                                      cgm["srcDir"], \
                                      group["srcDir"], \
                                      os.path.dirname(f)) \
                          for f in group["imports"]]
            else:
              impDirs = []
            cg.setupEnvironment(srcDir, dstDir, impDirs)
            if "files" in group:
              for f in group["files"]:
                # render template, remove ".template" from end of file
                if not cg.renderTemplate(f, context, os.path.join(group["dstDir"],
                                                                  os.path.splitext(f)[0])):
                  return cg.getResult()
          # add dashd macro to studio project
          elif group["command"] == "macro":
            if "macros_dashd" in group:
              for m in group["macros_dashd"]:
                if not cg.addMacro(m):
                  return cg.getResult()

  return cg.getResult()


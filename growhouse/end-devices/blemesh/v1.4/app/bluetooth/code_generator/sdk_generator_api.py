#!/usr/bin/env python


import os
import sys
import re
import json
import fnmatch
import platform
import argparse
import traceback


#### MODULE DATA BLOCK START ##################################################

genName = "BLE SDK External Generator"
genVersion = "1.0.0.0"
modPathList = ["../../../tool/code_generator/modules"]
genRequired = True
inputDataKeyList = ["ARCHITECTURE_ID",
                   "OPN_ID",
                   "TOOLCHAIN_PATH",
                   "TOOLCHAIN_GCC_PATH",
                   "BGBUILD_EXECUTABLE",
                   "COMMANDER_PATH",
                   "BLE_CONFIG",
                   "DEVICE_NAME",
                   "PROJECT_INFO",
                   "CALLBACKS",
                   "PLUGINS",
                   "MACROS",
                   "MPSI",
                   "SETUP_CONFIG"]
# unused, but available (only in configure step!) keys:
# "PART_ID"
# "BOARD_ID"
# "TOOLCHAIN_ID"
genConfiguration = ["configuration.json"]

#### MODULE DATA BLOCK END ####################################################


# Supported version is only 2.7 for now.
supportedPythonMajorVersion = 2
supportedPythonMinorVersion = 7

# Supported platforms.
supportedPlatforms = ["windows",
                      "linux",
                      "darwin"]

supportedCygwinPlatforms = ["cygwin_nt-6.1-wow",
                            "cygwin_nt-10.0-wow"]

supportedJavaPlatforms = ["java"]

exitCodeSuccess                   = 0
exitCodeWrongPythonVersion        = 1
exitCodeNotSupportedPlatform      = 2
exitCodeResourceNotFound          = 3
exitCodeXmlError                  = 4

successMessage = "Done."
errorMessageWrongPythonVersion = "This script was tested to work with python " + \
str(supportedPythonMajorVersion) + "." + str(supportedPythonMinorVersion) + \
" and may not work properly with other versions!"
errorMessageNotSupportedPlatform = "Not supported platform: "
errorMessageResourceNotFound = "Resource not found: "
errorMessageXmlError = "Error in XML."

# SDK generator dict
sdkGeneratorResult = \
{
  "success" : True,
  "message" : "Generation successful!",
  "content" : []
}

sdkGeneratorDefaultGenFunction = "generate"

# global used to hold script directory path
scriptDir = ""
optVerbose = False


def checkPythonVersion():
  if optVerbose:
    print "You are using python %d.%d.%d" % \
          (sys.version_info[0], sys.version_info[1], sys.version_info[2])
  if sys.version_info[0] != supportedPythonMajorVersion or \
    sys.version_info[1] != supportedPythonMinorVersion:
    print errorMessageWrongPythonVersion
    sys.exit(exitCodeWrongPythonVersion)


def getPlatform():
  detOs = platform.system().lower()
  # not supported
  if detOs not in supportedPlatforms and \
     detOs not in supportedCygwinPlatforms and \
     detOs not in supportedJavaPlatforms:
    print "Detected platform: " + detOs
    print "Supported platforms: "
    for selOs in supportedPlatforms:
      print selOs
    for selOs in supportedCygwinPlatforms:
      print selOs
    print errorMessageNotSupportedPlatform + "'" + detOs + "'"
    sys.exit(exitCodeNotSupportedPlatform)
  # *cygwin* --> windows
  elif detOs in supportedCygwinPlatforms:
    detOs = supportedPlatforms[0]
  elif detOs in supportedJavaPlatforms:
    import java.lang
    javaDetOs = java.lang.System.getProperty("os.name").lower()
    if "mac os" in javaDetOs:
      detOs = supportedPlatforms[2]
    elif "windows" in javaDetOs:
      detOs = supportedPlatforms[0]
    elif "linux" in javaDetOs:
      detOs = supportedPlatforms[1]
    else:
      print errorMessageNotSupportedPlatform + "'" + javaDetOs + "'"
      sys.exit(exitCodeNotSupportedPlatform)
  return detOs


def convertXmlToDict(xmlContent):
  import xml.etree.ElementTree as ET
  from xmljson import BadgerFish
  from collections import OrderedDict
  # parse XML DOM into dictionary
  bf = BadgerFish(dict_type=OrderedDict, xml_fromstring=False)
  # dump into dict string, then load back to dict
  return json.loads(json.dumps(bf.data(ET.fromstring(xmlContent))))


def debugWrite(content):
    sdkGeneratorResult["content"].append({"commandType" : "fileCreate",
                                          "filePath" : "debug.txt",
                                          "fileContent" : content})


def getCodegenContext():
  global scriptDir
  # global
  scriptDir = os.path.dirname(os.path.realpath(__file__))
  # read in configuration data, e.g.:
  #   - configuration.json
  #   - sampleApps_ble.json
  configData = []
  for configFile in genConfiguration:
    configFile = os.path.join(scriptDir, configFile)
    if configFile.endswith(".json"):
      configData.append(json.loads(open(configFile, "r").read()))
  assert any(["generators" in c for c in configData])
  assert any(["sampleApps" in c for c in configData])
  # Collect ID's of required Code Generator Modules
  rcgmIds = []
  for cd in configData:
    if "sampleApps" in cd:
      for sa in cd["sampleApps"]:
        if "requiredCgm" in sa:
          for rcgm in sa["requiredCgm"]:
            if rcgm["id"] not in rcgmIds:
              rcgmIds.append(rcgm["id"])
  # Read in all .json files found under each Code Generator's directory
  for cd in configData:
    if "generators" in cd:
      for g in cd["generators"]:
        if g["id"] in rcgmIds:
          configDir = os.path.join(scriptDir, g["path"])
          configFiles = os.listdir(configDir)
          for configFile in configFiles:
            if configFile.endswith(".json"):
              configFile = os.path.join(configDir, configFile)
              configData.append(json.loads(open(configFile, "r").read()))
  return configData


def generatorSuccess(msg, res):
  sdkGeneratorResult["success"] = True
  sdkGeneratorResult["message"] = msg
  sdkGeneratorResult["content"] = res
  return sdkGeneratorResult


def generatorError(msg):
  sdkGeneratorResult["success"] = False
  sdkGeneratorResult["message"] = msg
  sdkGeneratorResult["content"] = []
  return sdkGeneratorResult


def getCgmCgmg(codegenContext, cgmId, cgmgId=None):
  cgms = []
  for configModule in codegenContext:
    if "codeGenModules" in configModule:
      for cgm in configModule["codeGenModules"]:
        if fnmatch.fnmatch(cgm["id"], cgmId):
          cgms.append(cgm)
  if not cgmgId:
    return cgms
  cgmgs = []
  for cgm in cgms:
    if "groups" in cgm:
      for cgmg in cgm["groups"]:
        if fnmatch.fnmatch(cgmg["id"], cgmgId):
          cgmgs.append(cgmg)
  return cgmgs


def getRCgms(sa):
  rcgms = []
  if "requiredCgm" in sa:
    for rcgm in sa["requiredCgm"]:
      if rcgm in rcgms:
        return generatorError("Error: duplicate required CGM: '%s'!" \
                              % rcgm["id"])
      rcgms.append(rcgm)
  return rcgms


def getRCgmgs(rcgm):
  rcgmgs = []
  if "groups" in rcgm:
    for rcgmg in rcgm["groups"]:
      if rcgmg in rcgmgs:
        return generatorError("Error: duplicate required CGMG: '%s'!" \
                              % rcgmg["id"])
      rcgmgs.append(rcgmg)
  return rcgmgs


def sanityCheckConfiguration(codegenContext, sampleApp=None):
  ret = False
  # check for sample app duplicates
  sas = []
  for configModule in codegenContext:
    if "sampleApps" in configModule:
      for sa in configModule["sampleApps"]:
        if sa["id"] in sas:
          return generatorError("Sanity check error: duplicate SA: '%s'!" \
                                % sa["id"])
        sas.append(sa["id"])
        # check for required code generator module duplicates
        rcgms = []
        if "requiredCgm" in sa:
          for rcgm in sa["requiredCgm"]:
            if rcgm["id"] in rcgms:
              return generatorError("Sanity check error: duplicate required CGM: '%s'!" \
                                    % rcgm["id"])
            rcgms.append(rcgm["id"])
            # check for required code generator module group duplicates
            rcgmgs = []
            if "groups" in rcgm:
              for rcgmg in rcgm["groups"]:
                if rcgmg["id"] in rcgmgs:
                  return generatorError("Sanity check error: duplicate required CGMG: '%s'!" \
                                        % rcgmg["id"])
                rcgmgs.append(rcgmg["id"])
  # check for code generator group id duplicates
  for configModule in codegenContext:
    if "codeGenModules" in configModule:
      for cgm in configModule["codeGenModules"]:
        if "groups" in cgm:
          cgmgIds = []
          for cgmg in cgm["groups"]:
            if cgmg["id"] in cgmgIds:
              return generatorError("Sanity check error: duplicate CGMG: '%s' in CGM: '%s'!" \
                                    % (cgmg["id"], cgm["id"]))
            cgmgIds.append(cgmg["id"])
  # check group: command vs file or dirlist
  for configModule in codegenContext:
    if "codeGenModules" in configModule:
      for cgm in configModule["codeGenModules"]:
        if "groups" in cgm:
          for cgmg in cgm["groups"]:
            l = [{"command" : "include", "list" : "incDirs"},
                 {"command" : "copy", "list" : "files"}]
            for e in l:
              if cgmg["command"] == e["command"] and e["list"] not in cgmg:
                generatorError("Error: '%s' CGMG uses '%s' command but misses '%s' list!" % \
                               (cgmg["id"], e["command"], e["list"]))
  # check required code generator module id and group id references
  for configModule in codegenContext:
    if "sampleApps" in configModule:
      for sa in configModule["sampleApps"]:
        if not sampleApp or sa["id"] == sampleApp:
          rcgms = getRCgms(sa)
          for rcgm in rcgms:
            rcgmgs = getRCgmgs(rcgm)
            for rcgmg in rcgmgs:
              cgmgs = getCgmCgmg(codegenContext, rcgm["id"], rcgmg["id"])
              if not cgmgs:
                return generatorError("Sanity check error: '%s' CGMG is required by '%s' CGM of '%s' SA, but not defined!" \
                                      % (rcgmg["id"], rcgm["id"], sa["id"]))
  # populate paths to check
  pathsToCheck = []
  # compulsory code generator keys
  cgKeys = {"sampleAppRoot" : ""}
  for cgKey in cgKeys:
    foundKey = False
    for configModule in codegenContext:
      if cgKey in configModule:
        if foundKey:
          # As a workaround, this shouldn't be treated as an error.
          pass
        else:
          foundKey = True
          cgPath = os.path.join(scriptDir, configModule[cgKey])
          cgKeys[cgKey] = cgPath
          pathsToCheck.append(cgPath)
    if not foundKey:
      return generatorError("Sanity check error: %s key not defined!" % cgKey)
  # sample app dirs
  for configModule in codegenContext:
    if "sampleApps" in configModule:
      for sa in configModule["sampleApps"]:
        if not sampleApp or sa["id"] == sampleApp:
          pathsToCheck.append(os.path.join(scriptDir,
                                           cgKeys["sampleAppRoot"],
                                           sa["rootDir"]))
  # code generator module paths
  rcgmIds = {}
  if sampleApp:
    for configModule in codegenContext:
      if "sampleApps" in configModule:
        for sa in configModule["sampleApps"]:
          if sa["id"] == sampleApp:
            rcgms = getRCgms(sa)
            for rcgm in rcgms:
              rcgmIds[rcgm["id"]] = []
              rcgmgs = getRCgmgs(rcgm)
              for rcgmg in rcgmgs:
                rcgmIds[rcgm["id"]].append(rcgmg["id"])
            break
  for configModule in codegenContext:
    if "codeGenModules" in configModule:
      for cgm in configModule["codeGenModules"]:
        if not sampleApp \
          or [cgm["id"] for rcgmId in rcgmIds if fnmatch.fnmatch(cgm["id"], rcgmId)]:
          pathsToCheck.append(os.path.join(scriptDir,
                                           cgm["rootDir"]))
          # generator script file
          pathsToCheck.append(os.path.join(scriptDir,
                                           cgm["rootDir"],
                                           cgm["generator"]))
          # CGM group keys to check: files, incDirs, imports
          if "groups" in cgm:
            cgmgKeys = ["imports", "files", "incDirs"]
            for cgmg in cgm["groups"]:
              if not sampleApp \
                or [cgmg["id"] for rcgmgId in rcgmIds[cgm["id"]] if fnmatch.fnmatch(cgmg["id"], rcgmgId)]:
                for cgmgKey in cgmgKeys:
                  if cgmgKey in cgmg:
                    for f in cgmg[cgmgKey]:
                      pathsToCheck.append(
                        os.path.normpath(
                          os.path.join(scriptDir,
                                       cgm["rootDir"],
                                       cgm["srcDir"],
                                       cgmg["srcDir"],
                                       f)))
  # check paths
  for path in pathsToCheck:
    if not os.path.exists(path):
      return generatorError("Sanity check error: %s doesn't exist!" % path)
  return generatorSuccess("Sanity check successful!", [])


# Studio: Setup environment and create or configure
def setupGenerateStudio(context, step, verbose):
  # TODO: replace this with a meta_util.py call
  sys.path.append(os.path.normpath(
    os.path.join(os.path.dirname(os.path.realpath(__file__)),
                 "../../../tool/code_generator")))
  import code_generator
  result = []
  # find sample app
  for configModule in context["CODEGEN"]:
    # skip if no sampleApps key found
    if "sampleApps" not in configModule:
      continue
    for sa in configModule["sampleApps"]:
      # skip until sample app found
      if sa["id"] != context["DEVICE_NAME"]["id"]:
        continue
      # find required code generator module
      for rcgm in sa["requiredCgm"]:
        for configModule in context["CODEGEN"]:
          # skip if no codeGenModules key found
          if "codeGenModules" not in configModule:
            continue
          for cgm in configModule["codeGenModules"]:
            # skip until code generator module found
            if cgm["id"] != rcgm["id"]:
              continue
            # add CGM directory to sys.path
            cgmDir = os.path.join(scriptDir,
                                  cgm["rootDir"])
            sys.path.append(cgmDir)
            # remove '.py' from the end of the filename
            genModule = __import__(os.path.splitext(cgm["generator"])[0])
            # 'generate' function doesn't exist in script
            if not hasattr(genModule, sdkGeneratorDefaultGenFunction):
              # de-import module
              del genModule
              sys.modules.pop(os.path.splitext(cgm["generator"])[0])
              # remove dir from sys path
              sys.path.remove(cgmDir)
              # return error
              return generatorError("'%s' function is not implemented in '%s' module!" \
                                    % (sdkGeneratorDefaultGenFunction,
                                       os.path.splitext(cgm["generator"])[0]))
            # run generator script
            try:
              tmpResult = getattr(genModule, sdkGeneratorDefaultGenFunction)(cgm, rcgm, context, step)
              if tmpResult["success"]:
                if tmpResult["content"]:
                  result.extend(tmpResult["content"])
              else:
                # return error
                return generatorError(tmpResult["message"])
            except Exception as e:
              # de-import module
              del genModule
              sys.modules.pop(os.path.splitext(cgm["generator"])[0])
              # remove dir from sys path
              sys.path.remove(cgmDir)
              # get error trace
              trType, trValue, trTraceback = sys.exc_info()
              # return error
              return generatorError("".join(traceback.format_exception(trType, trValue, trTraceback)))
            # de-import module
            del genModule
            sys.modules.pop(os.path.splitext(cgm["generator"])[0])
            # remove dir from sys path
            sys.path.remove(cgmDir)
            break
      break
  # all went fine, return generation result
  return generatorSuccess("Generation successful!", result)


# resulting Arch ID: <part>~<opn>+<board>+<toolchain>
def massageArchId(sc):
  # both ARCHITECTURE_ID and OPN_ID keys have to exist
  assert "ARCHITECTURE_ID" in sc and "OPN_ID" in sc
  # ensure architecture is all lower case
  sc["ARCHITECTURE_ID"] = sc["ARCHITECTURE_ID"].lower()
  arch = sc["ARCHITECTURE_ID"].split("+")
  # glue OPN specifier to the end of part ID
  arch[0] = "%s~opn[%s]" % (arch[0], sc["OPN_ID"])
  # Only architecture and toolchain are stored in arch, board is missing
  # Note: Second element is expected to be the toolchain in this case
  #       If this is not true, there is a bug here...
  if len(arch) == 2:
    arch.insert(1, "none")
    print "Warning: missing board ID, adding 'none'!"
  assert len(arch) == 3
  sc["ARCHITECTURE_ID"] = "+".join(arch)


#### SDK Generator Public API #################################################

def name():
  """Return name of the generator as string.

  Returns:
      Name of generator.
  """
  return genName


def version():
  """Return version of the generator as string.

  Returns:
      Version of generator.
  """
  return genVersion


def searchPathForModules():
  """Return search paths of python modules required by the generator.

  Returns:
      List of search paths of python modules.
  """
  return modPathList


def requiresGeneration():
  """Return whether running generator is required.

  Returns:
      True if generator should be run.
  """
  return genRequired


def requiredInputDataKeys():
  """Return context keys required by the generator.

  Returns:
      List of context keys required by generator or template.
  """
  return inputDataKeyList


def configuration():
  """Return path of json file defining configuration for generator.

  Returns:
      Path of json file defining configuration for generator.
  """
  return genConfiguration


def createSampleApp(studioContext):
  checkPythonVersion()
  studioContext["PLATFORM"] = getPlatform()
  studioContext["CODEGEN"] = getCodegenContext()
  studioContext["DEVICE_NAME"] = {"id" : studioContext["SAMPLE_APP_ID"]}
  studioContext["PROJECT_INFO"] = {"rootDir" : studioContext["PROJECT_DIR"]}
  massageArchId(studioContext)
  print "########## CREATE PROJECT ##############################"
  if optVerbose:
    print "# name:", name()
    print "# version:", version()
    print "# searchPathForModules:", searchPathForModules()
    print "# requiresGeneration:", requiresGeneration()
    print "# requiredInputDataKeys:", requiredInputDataKeys()
    print "# configuration:", configuration()
    print "####################"
    print "# Sample App:", studioContext["SAMPLE_APP_ID"]
  print "# Project directory:", studioContext["PROJECT_DIR"]
  print "# OPN:", studioContext["OPN_ID"]
  print "# Arch ID:", studioContext["ARCHITECTURE_ID"]
  if optVerbose:
    print "# Studio Context:"
    print json.dumps(studioContext, indent=2)
  print "########################################################"
  result = sanityCheckConfiguration(studioContext["CODEGEN"],
                                    studioContext["DEVICE_NAME"]["id"])
  if not result["success"]:
    return result
  return setupGenerateStudio(studioContext, "create", True)


def generateContent(studioContext):
  """Call generator and return generation result.
     Before running the generator python/jython major and minor version are
     checked for compatibility. Configuration data is sanity checked too.

  Returns:
      Result of generation.
  """
  checkPythonVersion()
  studioContext["PLATFORM"] = getPlatform()
  studioContext["CODEGEN"] = getCodegenContext()
  if "BLE_CONFIG" in studioContext:
    studioContext["BLE_CONFIG"] = convertXmlToDict(studioContext["BLE_CONFIG"])
  if "BT_MESH_CONFIG" in studioContext:
    studioContext["BT_MESH_CONFIG"] = json.loads(studioContext["BT_MESH_CONFIG"])
  massageArchId(studioContext)
  print "########## CONFIGURE PROJECT ###########################"
  if optVerbose:
    print "# name:", name()
    print "# version:", version()
    print "# searchPathForModules:", searchPathForModules()
    print "# requiresGeneration:", requiresGeneration()
    print "# requiredInputDataKeys:", requiredInputDataKeys()
    print "# configuration:", configuration()
    print "####################"
    print "# Sample App directory:", studioContext["DEVICE_NAME"]["rootDir"]
  print "# Project directory:", studioContext["PROJECT_INFO"]["rootDir"]
  print "# OPN:", studioContext["OPN_ID"]
  print "# Arch ID:", studioContext["ARCHITECTURE_ID"]
  if optVerbose:
    print "# Studio Context:"
    print json.dumps(studioContext, indent=2)
  print "########################################################"
  result = sanityCheckConfiguration(studioContext["CODEGEN"],
                                    studioContext["DEVICE_NAME"]["id"])
  if not result["success"]:
    return result
  return setupGenerateStudio(studioContext, "configure", True)


def main():
  studioContext = {}
  studioContext["PLATFORM"] = getPlatform()
  studioContext["CODEGEN"] = getCodegenContext()
  result = sanityCheckConfiguration(studioContext["CODEGEN"])
  print result
  # if not result["success"]:
  #   sys.exit(1)

  studioContext["SAMPLE_APP_ID"] = "soc-rangetest-rail-dmp"
  studioContext["PROJECT_DIR"] = "/Users/tahornos/silabs/git/embsw/tmp-gen/proj"
  studioContext["OPN_ID"] = "efr32mg12p433f1024gl125"
  studioContext["ARCHITECTURE_ID"] = "efr32~family[m]~series[1]~device_configuration[2]~performance[p]~radio[433]~flash[1024k]~temp[g]~package[l]~pins[125]+brd4164a+gcc"
  d = createSampleApp(studioContext)
  # print d
  if not d["success"]:
    print d["message"]
    sys.exit(1)
  for e in d["content"]:
    if e["commandType"] == "fileCreate":
      # if e["filePath"].endswith(".isc"):
      #   print e["filePath"]
      #   with open(e["filePath"], "w") as f:
      #     f.write(e["fileContent"])

      if e["filePath"].endswith("board_features.h"):
        print e["filePath"]
        with open(e["filePath"], "w") as f:
          f.write(e["fileContent"])
  sys.exit(0)


if __name__ == "__main__":
  main()


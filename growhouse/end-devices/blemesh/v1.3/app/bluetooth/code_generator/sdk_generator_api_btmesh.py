#!/usr/bin/env python

import sdk_generator_api


#### MODULE DATA BLOCK START ##################################################

sdk_generator_api.inputDataKeyList.extend(["BT_MESH_CONFIG"])

sdk_generator_api.genConfiguration.extend(["sampleApps_btmesh.json"])

#### MODULE DATA BLOCK END ####################################################


#### SDK Generator Public API #################################################

def name():
  """Return name of the generator as string.

  Returns:
      Name of generator.
  """
  return sdk_generator_api.name()


def version():
  """Return version of the generator as string.

  Returns:
      Version of generator.
  """
  return sdk_generator_api.version()


def searchPathForModules():
  """Return search paths of python modules required by the generator.

  Returns:
      List of search paths of python modules.
  """
  return sdk_generator_api.searchPathForModules()


def requiresGeneration():
  """Return whether running generator is required.

  Returns:
      True if generator should be run.
  """
  return sdk_generator_api.requiresGeneration()


def requiredInputDataKeys():
  """Return context keys required by the generator.

  Returns:
      List of context keys required by generator or template.
  """
  return sdk_generator_api.requiredInputDataKeys()


def configuration():
  """Return path of json file defining configuration for generator.

  Returns:
      Path of json file defining configuration for generator.
  """
  return sdk_generator_api.configuration()


def generateContent(studioContext):
  """Call generator and return generation result.
     Before running the generator python/jython major and minor version are
     checked for compatibility. Configuration data is sanity checked too.

  Returns:
      Result of generation.
  """
  return sdk_generator_api.generateContent(studioContext)


def main():
  sdk_generator_api.main()


if __name__ == "__main__":
  main()


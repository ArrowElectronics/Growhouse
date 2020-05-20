/*
 * Copyright © 2019 Silicon Labs, http://www.silabs.com. All rights reserved.
 */

package com.siliconlabs.bluetoothmesh.App.Models

import com.siliconlab.bluetoothmesh.adk.data_model.model.ModelIdentifier
import com.siliconlab.bluetoothmesh.adk.data_model.model.SigModel
import com.siliconlab.bluetoothmesh.adk.data_model.node.Node

/**
 * @author Comarch S.A.
 */
class DeviceFunctionality {
    enum class FUNCTIONALITY(val functionalityName: String, vararg val model: ModelIdentifier) {
        // light
        Unknown(""),
        OnOff("OnOff", ModelIdentifier.GenericOnOffServer),
        Level("Level", ModelIdentifier.GenericLevelServer),
        Lightness("Lightness", ModelIdentifier.LightLightnessServer),
        CTL("CTL", ModelIdentifier.LightCTLServer),
        // switch
        OnOffClient("OnOff Client", ModelIdentifier.GenericOnOffClient),
        LevelClient("Level Client", ModelIdentifier.GenericLevelClient),
        LightnessClient("Lightness Client", ModelIdentifier.LightLightnessClient),
        CTLClient("CTL Client", ModelIdentifier.LightCTLClient),
    }

    companion object {
        private fun getAdditionalModels(functionality: FUNCTIONALITY): Set<ModelIdentifier> {
            return when (functionality) {
                // light
                FUNCTIONALITY.OnOff -> setOf(ModelIdentifier.LightLightnessServer)
                FUNCTIONALITY.Lightness -> setOf(ModelIdentifier.GenericOnOffServer)
                FUNCTIONALITY.CTL -> setOf(ModelIdentifier.LightCTLTemperatureServer, ModelIdentifier.GenericOnOffServer, ModelIdentifier.LightLightnessServer)
                // switch
                FUNCTIONALITY.OnOffClient -> setOf(ModelIdentifier.LightLightnessClient)
                FUNCTIONALITY.LightnessClient -> setOf(ModelIdentifier.GenericOnOffClient)
                FUNCTIONALITY.LevelClient -> setOf(ModelIdentifier.GenericOnOffClient)
                FUNCTIONALITY.CTLClient -> setOf(ModelIdentifier.GenericOnOffClient, ModelIdentifier.LightLightnessClient)
                else -> emptySet()
            }
        }

        fun getFunctionalities(node: Node): Set<FUNCTIONALITY> {
            val functionalities = mutableSetOf<FUNCTIONALITY>()
            functionalities.add(FUNCTIONALITY.Unknown)
            node.elements?.get(0)?.let { element ->
                element.sigModels.forEach { sigModel ->
                    val modelIdentifier = ModelIdentifier.values().find { it.id == sigModel.id }
                    if (modelIdentifier != null) {
                        val functionality = FUNCTIONALITY.values().find { it.model.contains(modelIdentifier) }
                        if (functionality != null) {
                            functionalities.add(functionality)
                        }
                    }
                }
            }

            return functionalities
        }

        fun getSigModels(node: Node, functionality: FUNCTIONALITY): Set<SigModel> {
            val modelsToBind = mutableSetOf<SigModel>()

            val functionalityModels = mutableSetOf<ModelIdentifier>()
            functionalityModels.addAll(functionality.model)
            functionalityModels.addAll(getAdditionalModels(functionality))

            node.elements?.get(0)?.let { element ->
                element.sigModels?.forEach { sigModel ->
                    val modelIdentifier = functionalityModels.find { it.id == sigModel.id }
                    if (modelIdentifier != null) {
                        modelsToBind.add(sigModel)
                    }
                }
            }

            return modelsToBind
        }

    }
}
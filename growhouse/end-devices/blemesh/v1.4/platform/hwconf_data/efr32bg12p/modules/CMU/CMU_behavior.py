from . import ExporterModel
from . import CMU_model
from . import RuntimeModel


class CMU(ExporterModel.Module):
    def __init__(self, name=None):
        if not name:
            name = self.__class__.__name__
        super(CMU, self).__init__(name, visible=True, core=True)
        self.model = CMU_model

    def set_runtime_hooks(self):

        ctune_token = self.get_property("BSP_CLK_HFXO_CTUNE_TOKEN")
        if ctune_token:
            RuntimeModel.set_change_handler(ctune_token, CMU.ctune_use_token)

        hfclk_selection = self.get_property("HAL_CLK_HFCLK_SOURCE")
        pll_config = self.get_property("HAL_CLK_PLL_CONFIGURATION")
        if pll_config:
            RuntimeModel.set_change_handler(hfclk_selection, CMU.pll_config_visible)

    @staticmethod
    def ctune_use_token(studio_module, property, state):
        hide_ctune_input = RuntimeModel.get_property_value(property, module=studio_module) == '1'
        RuntimeModel.set_property_hidden("BSP_CLK_HFXO_CTUNE", hide_ctune_input, module=studio_module, state=state)

    @staticmethod
    def pll_config_visible(studio_module, property, state):
        clk_src = RuntimeModel.get_property_value(property, module=studio_module)
        pll_config = RuntimeModel.get_property_reference("HAL_CLK_PLL_CONFIGURATION", module=studio_module)
        if clk_src == 'HAL_CLK_HFCLK_SOURCE_HFRCO':
            RuntimeModel.set_property_hidden(pll_config, False, state, studio_module)
        else:
            RuntimeModel.set_property_hidden(pll_config, True, state, studio_module)

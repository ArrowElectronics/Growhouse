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

    @staticmethod
    def ctune_use_token(studio_module, property, state):
        hide_ctune_input = RuntimeModel.get_property_value(property, module=studio_module) == '1'
        RuntimeModel.set_property_hidden("BSP_CLK_HFXO_CTUNE", hide_ctune_input, module=studio_module, state=state)
